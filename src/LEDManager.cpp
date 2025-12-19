#include "LEDManager.h"
#include "LEDUIElement.h"
#include "ModuleUIElement.h"
#include "Colors.h"
#include "SelectionHandler.h"
#include "Pathfinder.h"
#include "Grid.h"
#include <cmath>
#include <cstring>

#define LED_BRIGHTNESS 150

LEDManager& LEDManager::getInstance()
{ 
  static LEDManager instance;
  return instance;
}

void LEDManager::init(Adafruit_MultiTrellis* t, Patcher* p)
{
  getInstance().m_Patcher = p;
  getInstance().m_Trellis = t;

  getInstance().m_LEDDoubleFrameBuffer.push(&getInstance().m_LEDFrameBuffer1);
  getInstance().m_LEDDoubleFrameBuffer.push(&getInstance().m_LEDFrameBuffer2);
}

LEDFrame* LEDManager::getFrameBuffer()
{
  return getInstance().m_LEDDoubleFrameBuffer.front();  
}

LEDFrame* LEDManager::getPreviousFrameBuffer()
{
  return getInstance().m_LEDDoubleFrameBuffer.back();  
}

LEDFrame* LEDManager::popFrame()
{
  auto& frameBuffers = getInstance().m_LEDDoubleFrameBuffer;
  LEDFrame* frame = frameBuffers.front();
  frameBuffers.pop();
  return frame;
}

void drawBuffer(Adafruit_MultiTrellis* trellis, LEDFrameBuffer* buffer, size_t start, size_t numPixels)
{
  for (size_t i{}; i < numPixels; i++)
  {
    size_t index = start + i;
    trellis->setPixelColor(index, (*buffer)[index]);
  }
  trellis->show();
}

void LEDManager::renderFrame()
{
  auto& ledManager = getInstance();
  LEDFrame* currentFrame = ledManager.popFrame();
  LEDFrameBuffer* currentBuffer = currentFrame->data();
  LEDFrameBuffer* previousBuffer = ledManager.getPreviousFrameBuffer()->data();
  constexpr float brightness = LED_BRIGHTNESS / 255.0f;

  Adafruit_MultiTrellis* trellis = ledManager.m_Trellis;
  for (size_t i{}; i < currentBuffer->size(); i++)
  {
    //skip if the pixel hasn't changed
    if (currentBuffer->at(i) == previousBuffer->at(i)) 
    {
      continue;
    }
    previousBuffer->at(i) = currentBuffer->at(i);
    trellis->setPixelColor(i, Colors::changeBrightness(currentBuffer->at(i), brightness));
  }
  trellis->show();
  ledManager.m_LEDDoubleFrameBuffer.push(currentFrame);
}

void LEDManager::placeLed(int row, int col, uint32_t color)
{
  getFrameBuffer()->drawPixel(row, col, color);
}

void LEDManager::placeLEDElement(ModuleUIElement::LEDElement& ledElement)
{
  int row = ledElement.row;
  int col = ledElement.col;
  Grid::updateSquare(Grid::SquareState::MODULE, row, col);
  getFrameBuffer()->drawPixel(row, col, ledElement.color);
}

void LEDManager::placeLed(Module* m)
{
  if (m) 
  { 
    placeLEDElement(m->getLEDElement());
  }
}

uint32_t getWireColor(Module* m, float brightness) 
{ 
  if (!m)
  {
    return 0;
  }
  return Colors::changeBrightness(m->getColor(), brightness); 
}

void LEDManager::displayConnection(Module* src, Module* dest)
{
  if (!src || !dest) { 
    return; 
  }

  uint32_t color = getWireColor(src, 0.08f);

  auto& destLedElement = dest->getLEDElement();
  auto& srcLedElement = src->getLEDElement();

  Pathfinder::Position destPos{destLedElement.col, destLedElement.row};
  Pathfinder::Position currentPos{srcLedElement.col, srcLedElement.row};

  while (currentPos != destPos)
  {
    Pathfinder::Position nextPos = Pathfinder::findBestMove({currentPos.x, currentPos.y}, destPos);

    if (!nextPos || nextPos == destPos) 
    { 
      break; 
    }

    int rowIndex = nextPos.y;
    int colIndex = nextPos.x;

    if (!Grid::isInBounds(rowIndex, colIndex) || Grid::isInBank(rowIndex, colIndex)) 
    { 
      break; 
    }

    Grid::SquareState square = Grid::stateAt(rowIndex, colIndex);

    if (square == Grid::SquareState::EMPTY || square == Grid::SquareState::WIRE)
    {
      Grid::updateSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
      getFrameBuffer()->drawPixel(rowIndex, colIndex, color);
    }
    
    currentPos = nextPos;
  }
}

void LEDManager::refreshBank()
{
  if (SelectionHandler::isEditMode())
  {
    for (uint8_t i{}; i < 8; i++)
    {
      getFrameBuffer()->drawPixel(7, i, Colors::getColor(ModuleBuilder::bankType(i)));
    }
  }
  else
  {
    for (uint8_t i{}; i < 8; i++)
    {
      getFrameBuffer()->drawPixel(7, i, 0x000000);
    }
  }
}

void LEDManager::clearGrid()
{
  getFrameBuffer()->clear();
}

void LEDManager::refreshGrid(Module* m)
{
  if (m)
  {
    placeLed(m);
    return;
  }

  LEDFrame* currentFrame = getFrameBuffer();
  currentFrame->clear();

  if (SelectionHandler::isEditMode())
  {
    for (uint8_t i{}; i < 8; i++)
    {
      currentFrame->drawPixel(7, i, Colors::getColor(ModuleBuilder::bankType(i)));
    }
  }

  //Modules
  for (Module* m : Module::getAllModules()) 
  {
    placeLed(m);
  }

  Grid::update();

  //Connections
  for (Patch* p : Patcher::getAllPatches()) 
  {
    displayConnection(p->source, p->destination);
  }
}

void LEDManager::displayKeyboard()
{
  clearGrid();
  KeyboardLEDElement keyboardElement(getFrameBuffer());
  keyboardElement.draw();
}

void LEDManager::placeModule(Module* m)
{
  if (!m)
  {
    Serial.println("Error: Cannot place module as it does not exist.");
    return;
  }
  placeLEDElement(m->getLEDElement());
  Grid::update();
}