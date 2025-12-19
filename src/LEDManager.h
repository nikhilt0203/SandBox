#include "TrellisManager.h"
#include "ModuleBuilder.h"
#include "Patcher.h"
#pragma once

class LEDManager
{
public:
  static LEDManager& getInstance();

  static void init(Adafruit_MultiTrellis* t, Patcher* p);

  static void placeLed(int row, int col, uint32_t color);

  static void displayConnection(Module* src, Module* dest, bool show);

  static void refreshBank(bool show = true);;

  static void clearGrid(bool show = true);

  static void refreshGrid(Module* m = nullptr);

  static void placeModule(Module* m);

  static void changeSquareBrightness(int position, float brightness);

  void playPatchAnimation(Module* src, Module* dest);

  void selectAnimation() {}

  void deleteAnimation() {}

private:
  Adafruit_MultiTrellis* trellis;
  ModuleBuilder* builder;
  Patcher* patcher;

  LEDManager() : trellis(nullptr), patcher(nullptr) {}
  
  static bool checkTrellis() { return getInstance().trellis != nullptr; }

  static bool checkPatcher() { return getInstance().patcher != nullptr; }
};