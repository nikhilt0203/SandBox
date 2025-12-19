#pragma once
#include "SelectionManager.h"
#include "TrellisManager.h"
#include "Module.h"
#include "Patcher.h"
#include "SDManager.h"
#include "ModuleBuilder.h"
#include "LEDManager.h"
#include "StevesAwesomeButton.h"
#include "DisplayManager.h"
#include "EncoderManager.h"
#include "Pathfinder.h"
#include "Oscilloscope.h"

namespace App
{
  StevesAwesomeButton editButton(16, 0, INPUT_PULLUP);
  StevesAwesomeButton saveButton(15, 1, INPUT_PULLUP);
  StevesAwesomeButton loadButton(9, 2, INPUT_PULLUP);
  StevesAwesomeButton fileButton(8, 3, INPUT_PULLUP);

  Patcher patcher;

  //auto path1 = "keyboard1.txt";
  //auto path2 = "patch.txt";
  //auto path1 = "patch.txt";
  //auto path2 = "test.txt";
  std::string filePath = "test1.txt";
  auto path1 = "test1.txt";
  auto path2 = "test2.txt";

  void loadButtonPress(int num)
  {
    Serialize::loadPatchFromFile(filePath, &patcher);
    DisplayManager::print("Loaded " + filePath + "!", 3, ILI9341_GREEN);
    SelectionManager::exitEditMode();
    LEDManager::refreshGrid();
  }

  void fileButtonPress(int num)
  {
    if (filePath == path2)
      filePath = path1;
    else
      filePath = path2;

    DisplayManager::print("Will load " + filePath, 3);
  }

  void saveButtonPress(int num)
  {
    static int pressCount = 0;

    if (pressCount == 2) 
      pressCount = 0;
    if (pressCount == 0) 
    {
      DisplayManager::print("Saving will override the last saved patch. Press again to confirm.", 2);
      pressCount++;
      return;
    }
    if (pressCount == 1) 
    {
      DisplayManager::print("Saving...", 5);
      Serialize::savePatchToFile(filePath);
      DisplayManager::print("Saved to " + filePath, 2);
      pressCount++;
    }
  }

  void editButtonPress(int num)
  {
    if (!SelectionManager::isEditMode())
    {
      SelectionManager::enterEditMode();
      DisplayManager::print("EDIT MODE", 4, ILI9341_RED);
      LEDManager::refreshBank();
    }
    else
    {
      SelectionManager::exitEditMode();
      DisplayManager::print("READ MODE", 4, ILI9341_GREEN);
      LEDManager::refreshBank();
    }
  }

  TrellisCallback pressHandler(keyEvent evt);

  void init()
  {
    Serial.begin(115200);
    randomSeed(analogRead(A0));
    AudioMemory(400);
 
    saveButton.pressHandler(saveButtonPress);
    editButton.pressHandler(editButtonPress);
    loadButton.pressHandler(loadButtonPress);
    fileButton.pressHandler(fileButtonPress);

    SDManager::init();
    ModuleBuilder::init();
    SelectionManager::init();
    EncoderManager::init();
    TrellisManager::init(pressHandler);
    DisplayManager::init();
    LEDManager::init(&TrellisManager::trellis, &patcher);
    LEDManager::refreshGrid();
  }

  void loop()
  {
    DisplayManager::renderFrame();
    editButton.process();
    saveButton.process();
    loadButton.process();
    fileButton.process();
    EncoderManager::update();
    TrellisManager::update();
    
    for (Module* m: Module::getAllModules())
    {
      if (!m->isDisplayUpdated())
      {
        LEDManager::refreshGrid(m);
        m->markDisplayUpdated(true);
      }
    }
    Module* m = Module::getModule(SelectionManager::getCurrentSelection());
    if (m) m->update();
  }


  void moduleCreate(int bankPosition, int gridPosition)
  {
    int bankNum = bankPosition - 56;
    int row = gridPosition / 8;
    int col = gridPosition % 8;

    if (bankNum < 0 || bankNum > 7 || !ModuleBuilder::canCreate(gridPosition)) return;

    Module* m = ModuleBuilder::createModule(ModuleBuilder::bankType(bankNum), row, col);
    if (!m) return;
    LEDManager::placeModule(m);
  }

  void deleteModuleAt(int position)
  {
    if (!Grid::isValid(position)) return;

    Module* m = Module::getModule(position);
    if (m && m->m_Type != Module::Type::SEQUENCERSTEP)
      ModuleBuilder::deleteModule(m, &patcher);
  }

  void runAction(SelectionManager::Action action)
  {
    int previousSelectedPos = SelectionManager::getPreviousSelection();
    int selectedPos = SelectionManager::getCurrentSelection();
    Module* m1 = nullptr;
    Module* m2 = nullptr;

    if (previousSelectedPos == -1 || selectedPos == -1) return;

    switch (action)
    {
      case SelectionManager::Action::PATCH:
        m1 = Module::getModule(previousSelectedPos);
        m2 = Module::getModule(selectedPos);
        if (!m1 || !m2) 
          return;
        patcher.connectionHandler(Module::getModule(previousSelectedPos), Module::getModule(selectedPos));
        break;
      case SelectionManager::Action::CREATE: 
        moduleCreate(previousSelectedPos, selectedPos); 
        break;
      case SelectionManager::Action::DELETE: 
        deleteModuleAt(selectedPos); 
        break;
      case SelectionManager::Action::SELECT_MODULE: 
        LEDManager::refreshGrid();
        LEDManager::changeSquareBrightness(selectedPos, 0.95f);
        return;
      case SelectionManager::Action::SELECT_GRID:
        return;
      case SelectionManager::Action::SELECT_BANK:
        ModuleBuilder::displayBank(selectedPos - 56);
      case SelectionManager::Action::NONE:
        return;
      default: 
        return;
    }
    SelectionManager::clearSelections();
    LEDManager::refreshGrid();
  }

  TrellisCallback pressHandler(keyEvent evt)
  {
    int keyNum = evt.bit.NUM;
    static unsigned long lastPressedTime[64] = {0};

    //getNextKeyEvent() 
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    {
      lastPressedTime[keyNum] = millis();
      Module* m = Module::getModule(keyNum);
      if (m) {
        m->pressRisingEdge();
      //LEDManager::changeSquareBrightness(m->getPosition(), 0.5);
      }
    }
    else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) 
    {
      unsigned long heldTime = millis() - lastPressedTime[keyNum];
      runAction(SelectionManager::select(keyNum, heldTime));
      Module* m = Module::getModule(keyNum);
      if (m) 
        m->pressFallingEdge();
    }
    return 0;
  }
}