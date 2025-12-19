#pragma once

#define EDIT_BUTTON_PIN 32
#define SAVE_BUTTON_PIN 30
#define LOAD_BUTTON_PIN 31
#define SHIFT_BUTTON_PIN 29

namespace ButtonManager
{
  void init(void (*editCallback)(int), 
            void (*saveCallback)(int),
            void (*loadCallback)(int),
            void (*shiftCallback)(int));

  void update();
}