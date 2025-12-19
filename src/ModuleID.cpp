#include "ModuleID.h"
#include <climits>

std::vector<int> ModuleID::allIDNums;

int makeIDNum() 
{
  int newID = ModuleID::allIDNums.size();
  bool isUnique = false;

  while (!isUnique)
  {
    isUnique = true;
    for (int id : ModuleID::allIDNums)
    {
      if (id == newID)
      {
        isUnique = false;
        newID++;
        break;
      }
    }
  }
  return newID;
}

ModuleID::ModuleID(const char* name) 
: m_Name(name), m_SerializationID(SERIALIZATION_FORMAT), m_isFirstParam(true)
{
  setIDNum(makeIDNum());
}

std::string_view ModuleID::makeSerializationID(ModuleUIElement& uiElement)
{
  m_NumParams = uiElement.getDisplayElement().parameters.size();
  m_SerializationID = SERIALIZATION_FORMAT;
  m_SerializationID.insert(ModuleID::findCharacter("{"), uiElement.getDisplayName());
  setRow(uiElement.getLEDElement().row);
  setCol(uiElement.getLEDElement().col);


  setIDNum(m_IDNum);
  m_isFirstParam = true;
  return m_SerializationID;
}

void ModuleID::setRow(int row)
{
  m_SerializationID.insert(m_SerializationID.find(",", findCharacter("[")), std::to_string(row));
}

void ModuleID::setCol(int col)
{
  m_SerializationID.insert(findCharacter("]"), std::to_string(col));
}

void ModuleID::setIDNum(int IDNum)
{
  m_IDNum = IDNum;
  m_SerializationID.insert(findCharacter(">"), std::to_string(IDNum));
  allIDNums.push_back(IDNum);
}