#pragma once
#include "SelectionHandler.h"
#include "TrellisManager.h"
#include "Module.h"
#include "Patcher.h"
#include "SDManager.h"
#include "ModuleBuilder.h"
#include "LEDManager.h"
#include "StevesAwesomeButton.h"
#include "DisplayManager.h"
#include "EncoderManager.h"
#include "UIElements.h"
#include "ButtonManager.h"

namespace App
{
  AudioOutputI2S i2s; //start audio interrupts

  Patcher patcher;

  Module* g_LockedModule = nullptr;

  TrellisCallback trellisPressHandler(keyEvent evt);

  std::string getKeyboardInput(const char* prompt)
  {
    constexpr static size_t MAX_FILENAME_LENGTH = 15;
    constexpr static unsigned long KEYBOARD_TIMEOUT_MS = 20000;

    DisplayManager::clearScreen();
    DisplayManager::draw<KeyboardElement>(prompt);
    DisplayManager::renderFrame();

    LEDManager::displayKeyboard();
    LEDManager::renderFrame();

    std::string input;
    input.reserve(MAX_FILENAME_LENGTH);

    Timer timer;
    timer.start();

    TrellisManager::waitForInput();

    while (TrellisManager::isWaiting()) 
    {
      if (timer.hasReached(KEYBOARD_TIMEOUT_MS)) 
      {
        TrellisManager::waitForInput(false);
        break;
      }

      TrellisManager::update();
      if (!TrellisManager::isNewEvent()) 
      {
        continue;
      }

      timer.reset();
      size_t keyIndexOffset = Grid::COLS * 2; //keyboard starts at row 3
      size_t keyNum = TrellisManager::getKeyEvent().bit.NUM - keyIndexOffset;

      if (keyNum >= KeyboardElement::NUM_KEYS) 
      {
        continue;
      }

      if (keyNum == KeyboardElement::ENTER_KEY || keyNum == KeyboardElement::ESCAPE_KEY)
      {
        TrellisManager::waitForInput(false);
        break;
      }

      if (keyNum == KeyboardElement::BACKSPACE_KEY) 
      {
        if (input.empty()) 
        {
          continue;
        }

        input.pop_back();
        DisplayManager::draw<KeyboardElement>(input);
        DisplayManager::renderFrame();
        continue;
      }

      if (input.size() < MAX_FILENAME_LENGTH) 
      {
        input += KeyboardElement::KEY_LABELS[keyNum];
        DisplayManager::draw<KeyboardElement>(input);
        DisplayManager::renderFrame();
      }
    }

    return input;
  }

  void loadButtonPress(int num)
  {
    static int numPresses = 0;
    numPresses++;

    auto& directoryContents = SDManager::getDirectoryContents();
    static size_t directoryIndex = 0;

    auto& filePath = directoryContents[directoryIndex];
    
    if (numPresses == 2)
    {
      numPresses = 0;
      EncoderManager::waitForInput(false);

      if (Serialize::loadPatchFromFile(filePath, &patcher)) 
      {
        DisplayManager::draw<Dialog>("Loaded patch", "\"" + filePath.substr(0, filePath.length() - 4) + "\"", ILI9341_GREEN);
        g_LockedModule = nullptr;
        SelectionHandler::unlock();
        SelectionHandler::exitEditMode();
      }
      else 
      {
        DisplayManager::draw<ErrorDialog>("Failed to load patch\"" + filePath.substr(0, filePath.length() - 4) + "\"");
        SelectionHandler::enterEditMode();
      }

      LEDManager::refreshGrid();
      return;
    }

    EncoderManager::waitForInput();

    constexpr static size_t SCROLL_KNOB = 2;

    const size_t numFiles = directoryContents.size();
    while (EncoderManager::isWaiting())
    {
      directoryIndex = ((EncoderManager::getInput(SCROLL_KNOB) / 4) + numFiles) % numFiles;

      DisplayManager::draw<DirectoryDisplayPage>(directoryContents, directoryIndex);
      DisplayManager::renderFrame();
      ButtonManager::update();
    }
  }

  void shiftButtonPress(int num)
  {
    if (SelectionHandler::isLocked())
    {
      SelectionHandler::unlock();
      g_LockedModule = nullptr;
    }
    else
    {
      SelectionHandler::lock();
      g_LockedModule = Module::getModuleAt(SelectionHandler::currentSelection());
    }
  }

  void saveButtonPress(int num)
  {
    static int timesPressed = 0;
    static std::string fileName;

    if (timesPressed > 1) { 
      timesPressed = 0;
    }

    if (timesPressed == 0) 
    {
      fileName = getKeyboardInput("Save patch as: ");

      if (fileName.empty()) 
      {
        DisplayManager::clearScreen();
        LEDManager::refreshGrid();
        return;
      }

      fileName += ".txt";

      if (SD.exists(fileName.c_str()))
      {
        DisplayManager::draw<Dialog>(fileName + " already exists.", 
          "Press SAVE again to confirm overwrite.", ILI9341_YELLOW);
        timesPressed++;
        return;
      }
      timesPressed++;
    }

    if (timesPressed == 1)
    {
      Serialize::savePatchToFile(fileName) ? 
        DisplayManager::draw<Dialog>("Saved to " + fileName, "!", ILI9341_GREEN) 
        : DisplayManager::draw<ErrorDialog>("Failed to save to " + fileName);

      timesPressed++;
      LEDManager::refreshGrid();
    } 
  }

  void editButtonPress(int num)
  {
    if (!SelectionHandler::isEditMode())
    {
      SelectionHandler::enterEditMode();
      DisplayManager::draw<Dialog>("EDIT MODE", 
        "Select the bank to create a module     or select two modules to create a        patch.", 
        ILI9341_GREEN);
      LEDManager::refreshBank();
    }
    else
    {
      SelectionHandler::exitEditMode();
      DisplayManager::draw<Dialog>("READ MODE", 
        "Only module parameters may be       changed.", 
        ILI9341_BLUE);
      LEDManager::refreshBank();
    }
  }

  void init()
  {
    Serial.begin(115200);
    randomSeed(analogRead(A0));
    AudioMemory(100);

    DisplayManager::init();
    DisplayManager::draw<SplashScreen>();
    DisplayManager::renderFrame();

    SDManager::init();
    ModuleBuilder::init();
    SelectionHandler::init();
    ButtonManager::init(editButtonPress, saveButtonPress, loadButtonPress, shiftButtonPress);
    TrellisManager::init(trellisPressHandler);
    EncoderManager::init();
    LEDManager::init(&TrellisManager::trellis, &patcher);
    LEDManager::refreshGrid();

    editButtonPress(0);
    editButtonPress(0);
    DisplayManager::draw<SplashScreen>();
  }

  void loop()
  {
    EncoderManager::update();
    TrellisManager::update();
    ButtonManager::update();

    if (Module* m = Module::getModuleAt(SelectionHandler::currentSelection())) 
    {
      m->update(); 
      if (g_LockedModule) g_LockedModule->update();
    }

    for (Module* m: Module::getAllModules())
    {
      if (!m) continue;

      if (!m->getLEDElement().isUpdated)
      {
        LEDManager::refreshGrid(m);
        m->getUIElement().markAsUpdated<ModuleUIElement::LEDElement>();
      }
    }
    DisplayManager::draw<LockIconElement>(10, 300, SelectionHandler::isLocked());

    DisplayManager::renderFrame();
    LEDManager::renderFrame();
  }

  void runAction(SelectionHandler::Action action);

  void createModule(int bankPosition, int gridPosition)
  {
    int bankNum = bankPosition - 56;
    int row = gridPosition / 8;
    int col = gridPosition % 8;

    if (!ModuleBuilder::canCreate(gridPosition)) { 
      return; 
    }

    if (Module* m = ModuleBuilder::createModule(ModuleBuilder::bankType(bankNum), row, col)) 
    { 
      LEDManager::placeLEDElement(m->getLEDElement());
      runAction(SelectionHandler::select(gridPosition, 0));
      DisplayManager::draw<ModuleDisplayPage>(m);
    }
  }


  void deleteModule(int position)
  {
    Module* m = Module::getModuleAt(position);

    //Sequencer is responsible for deleting its steps
    if (m && m->getType() != ModuleConfig::Type::SEQUENCERSTEP) 
    { 
      size_t numConnections = std::count_if(
        patcher.getAllPatches().begin(), 
        patcher.getAllPatches().end(),
        [&](Patch* p){return p->source == m || p->destination == m;});

      std::string title = "Deleted " + std::string(m->getName());
      std::string message = "and " + std::to_string(numConnections) + " connections";
      DisplayManager::draw<Dialog>(title, message, ILI9341_YELLOW);
      ModuleBuilder::deleteModule(m, &patcher);
    }
  }

  void runAction(SelectionHandler::Action action)
  {
    int previousSelectedPos = SelectionHandler::previousSelection();
    int selectedPos = SelectionHandler::currentSelection();

    if (previousSelectedPos < 0 || selectedPos < 0) 
    { 
      return; 
    }

    switch (action)
    {
      case SelectionHandler::Action::PATCH:
        patcher.connectionHandler(
            Module::getModuleAt(previousSelectedPos), 
            Module::getModuleAt(selectedPos));
        break;

      case SelectionHandler::Action::CREATE: 
        createModule(previousSelectedPos, selectedPos); 
        break;

      case SelectionHandler::Action::DELETE: 
        deleteModule(selectedPos); 
        break;

      case SelectionHandler::Action::SELECT_MODULE: 
        LEDManager::refreshGrid();
        return;

      case SelectionHandler::Action::SELECT_GRID:
        return;

      case SelectionHandler::Action::SELECT_BANK:
        ModuleBuilder::displayBank(selectedPos);

      case SelectionHandler::Action::NONE:
        return;

      default: 
        return;
    }

    SelectionHandler::clearSelections();
    LEDManager::refreshGrid();
  }

  TrellisCallback trellisPressHandler(keyEvent evt)
  {
    const uint16_t keyNum = evt.bit.NUM;
    const uint8_t keyEdge = evt.bit.EDGE;
    static unsigned long lastPressedTime[Grid::ROWS * Grid::COLS] = {0};

    if (keyEdge == SEESAW_KEYPAD_EDGE_RISING)
    {
      lastPressedTime[keyNum] = millis();

      if (Module* m = Module::getModuleAt(keyNum)) 
      {
        m->pressRisingEdge();
        if (!SelectionHandler::isLocked()) 
        {
          DisplayManager::draw<ModuleDisplayPage>(m);
        }
      }
    }
    else if (keyEdge == SEESAW_KEYPAD_EDGE_FALLING) 
    {
      unsigned long previousTime = lastPressedTime[keyNum];
      unsigned long heldTime = millis() - previousTime;

      if (TrellisManager::isWaiting()) 
      {
        TrellisManager::saveKeyEvent(evt, previousTime);
        return 0;
      }

      runAction(SelectionHandler::select(keyNum, heldTime));

      if (Module* m = Module::getModuleAt(keyNum)) 
      {
        m->pressFallingEdge();
      }
    }

    return 0;
  }
}