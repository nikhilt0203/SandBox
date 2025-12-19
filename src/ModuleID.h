#include <string>
#include <climits>
#include <Arduino.h>
#include "ModuleUIElement.h"

// Manages module ID numbers and serialization IDs
class ModuleID
{
public:
  static constexpr const char* SERIALIZATION_FORMAT = "{}[,]<>";

  static std::vector<int> allIDNums;

  static void addID(int idNum) { allIDNums.push_back(idNum); }

public:
  ModuleID(const char* name);

  void setName(const char* name) { m_Name = name; };

  void setRow(int row);

  void setCol(int col);

  void setIDNum(int IDNum);

  int getIDNum() const { return m_IDNum; };

  const std::string& getName() const { return m_Name; };

  std::string_view getSerializationID() const { return m_SerializationID; }

  std::string_view makeSerializationID(ModuleUIElement& uiElement);

  template <typename T>
  void addParam(T param)
  {
    if (m_NumParams == 0) { 
      return; 
    }

    if (m_isFirstParam)
    {
      m_SerializationID.insert(m_SerializationID.find("}"), std::to_string(param));
      m_isFirstParam = false;
    }
    else 
    { 
      m_SerializationID.insert(m_SerializationID.find("}"), "," + std::to_string(param)); 
    }
  }
  
  static void resetAllIDs() { allIDNums.clear(); }
  
private:
  size_t findCharacter(const char* c) { return m_SerializationID.find(c); };

private:
  std::string m_Name;
  std::string m_SerializationID;
  int m_NumParams;
  int m_IDNum;
  bool m_isFirstParam;
};