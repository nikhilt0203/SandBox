#include "LEDManager.h"
#include "LEDUIElement.h"
#include "ModuleUIElement.h"
#include "Colors.h"
#include "SelectionHandler.h"
#include "Pathfinder.h"
#include "Grid.h"
#include <cmath>
#include <cstring>

#define LED_BRIGHTNESS 180

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
  constexpr static float brightness = LED_BRIGHTNESS / 255.0f;

  Adafruit_MultiTrellis* trellis = ledManager.m_Trellis;
  for (size_t i{}; i < currentBuffer->size(); i++)
  {
    //skip if the pixel hasn't changed
    if (currentBuffer->at(i) == previousBuffer->at(i)) {
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
  Grid::updateSquare(Grid::Space::MODULE, row, col);
  getFrameBuffer()->drawPixel(row, col, ledElement.color);
}

void LEDManager::placeLed(Module* m)
{
  if (m) { 
    placeLEDElement(m->getLEDElement());
  }
}

uint32_t getWireColor(Module* m, float brightness) 
{ 
  if (!m){
    return 0;
  }
  return Colors::changeBrightness(m->getColor(), brightness); 
}

void LEDManager::displayConnection(Module* src, Module* dest)
{
  if (!src || !dest) { 
    return; 
  }

  auto& destLedElement = dest->getLEDElement();
  auto& srcLedElement = src->getLEDElement();

  Pathfinder::Position destPos{destLedElement.row, destLedElement.col};
  Pathfinder::Position currentPos{srcLedElement.row, srcLedElement.col};

  uint32_t color;
  while (currentPos != destPos)
  {
    Pathfinder::Position nextPos = Pathfinder::findBestMove({currentPos.row, currentPos.col}, destPos);

    if (!nextPos || nextPos == destPos) { 
      break; 
    }

    int row = nextPos.row;
    int col = nextPos.col;

    if (!Grid::isInBounds(row, col) || Grid::isInBank(row, col)) { 
      break; 
    }

    Grid::Space square = Grid::stateAt(row, col);

    if (square == Grid::Space::EMPTY) {
      color = getWireColor(src, 0.08f);
    }
    else if (square == Grid::Space::WIRE) {
      color = Colors::blend(getWireColor(src, 0.08f), 
        getFrameBuffer()->at(row * Grid::COLS + col), 0.2f);
    } 
    else {
      continue;
    }

    Grid::updateSquare(Grid::Space::WIRE, row, col);
    getFrameBuffer()->drawPixel(row, col, color);
  
    currentPos = nextPos;
  }
}

// void LEDManager::displayConnection(Module* src, Module* dest)
// {
//   if (!src || !dest) { 
//     return; 
//   }

//   auto& destLedElement = dest->getLEDElement();
//   auto& srcLedElement  = src->getLEDElement();

//   std::vector<Pathfinder::Position> path = Pathfinder::findShortestPath(
//     {destLedElement.row, destLedElement.col},
//     {srcLedElement.row,  srcLedElement.col}
//   );

//   if (path.empty()) {
//     return;
//   }

//   for (size_t i{1}; i < path.size() - 1; i++)
//   {
//     int row = path[i].row;
//     int col = path[i].col;

//     if (!Grid::isValid(row, col)){
//       continue;
//     }

//     Grid::Space square = Grid::stateAt(row, col);
//     if (square == Grid::Space::MODULE) {
//       continue;
//     }

//     uint32_t color;
//     if (square == Grid::Space::EMPTY) {
//       color = getWireColor(src, 0.08f);
//     }
//     else if (square == Grid::Space::WIRE) {
//       color = Colors::blend(getWireColor(src, 0.08f), 
//         getFrameBuffer()->at(row * Grid::COLS + col), 0.2f);
//     } 
//     else {
//       continue;
//     }
//     Serial.printf("Drawing wire at (%d, %d)\n", row, col);
//     Grid::updateSquare(Grid::Space::WIRE, row, col);
//     getFrameBuffer()->drawPixel(row, col, color);
//   }
// }

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