#include "IDGenerator.h"
#include <climits>

std::vector<int> IDGenerator::allIDNums;

int makeIDNum() 
{
  int newID = IDGenerator::allIDNums.size();
  bool isUnique = false;

  while (!isUnique)
  {
    isUnique = true;
    for (int id : IDGenerator::allIDNums)
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

IDGenerator::IDGenerator(const char* name) 
: m_Name(name), m_SerializedID(SERIALIZATION_FORMAT), m_isFirstParam(true)
{
  setIDNum(makeIDNum());
}

const std::string& IDGenerator::makeSerializedID(const char* name, int row, int col, int numParams)
{
  m_NumParams = numParams;
  m_SerializedID = SERIALIZATION_FORMAT;
  m_SerializedID.insert(IDGenerator::findCharacter("{"), name);
  setRow(row);
  setCol(col);
  setIDNum(m_IDNum);
  m_isFirstParam = true;
  return m_SerializedID;
}

void IDGenerator::setRow(int row)
{
  m_SerializedID.insert(m_SerializedID.find(",", findCharacter("[")), std::to_string(row));
}

void IDGenerator::setCol(int col)
{
  m_SerializedID.insert(findCharacter("]"), std::to_string(col));
}

void IDGenerator::setIDNum(int IDNum)
{
  m_IDNum = IDNum;
  m_SerializedID.insert(findCharacter(">"), std::to_string(IDNum));
  allIDNums.push_back(IDNum);
}