#include "ButtonManager.h"
#include "StevesAwesomeButton.h"

static StevesAwesomeButton editButton(EDIT_BUTTON_PIN, 0, INPUT_PULLUP);
static StevesAwesomeButton saveButton(SAVE_BUTTON_PIN, 1, INPUT_PULLUP);
static StevesAwesomeButton loadButton(LOAD_BUTTON_PIN, 2, INPUT_PULLUP);
static StevesAwesomeButton shiftButton(SHIFT_BUTTON_PIN, 3, INPUT_PULLUP);


void ButtonManager::init(void (*editCallback)(int), 
                         void (*saveCallback)(int),
                         void (*loadCallback)(int),
                         void (*shiftCallback)(int))
{
  editButton.pressHandler(editCallback);
  saveButton.pressHandler(saveCallback);
  loadButton.pressHandler(loadCallback);
  shiftButton.pressHandler(shiftCallback);
}

void ButtonManager::update()
{
  editButton.process();
  saveButton.process();
  loadButton.process();
  shiftButton.process();
}