#include <string>
#include <climits>
#include <Arduino.h>

class IDGenerator
{
public:
  static constexpr const char* SERIALIZATION_FORMAT = "{}[,]<>";
  static std::vector<int> allIDNums;
  static void addID(int idNum) { allIDNums.push_back(idNum); }

public:
  IDGenerator(const char* name);
  const std::string& makeSerializedID(const char* name, int row, int col, int numParams = 0);
  void setName(const char* name) { m_Name = name; };
  void setRow(int row);
  void setCol(int col);
  void setIDNum(int IDNum);

  int getIDNum() const { return m_IDNum; };
  const std::string& getName() const { return m_Name; };
  const std::string& getSerializedID() const { return m_SerializedID; }

  template <typename T>
  void addParam(T param)
  {
    if (m_NumParams == 0)
      return;

    if (m_isFirstParam)
    {
      m_SerializedID.insert(m_SerializedID.find("}"), std::to_string(param));
      m_isFirstParam = false;
    }
    else
    {
      m_SerializedID.insert(m_SerializedID.find("}"), "," + std::to_string(param));
    }
  }
  
  inline static void resetAllIDs() { allIDNums.clear(); }
private:
  size_t findCharacter(const char* c) { return m_SerializedID.find(c); };

private:
  std::string m_Name;
  std::string m_SerializedID;
  int m_NumParams;
  int m_IDNum;
  bool m_isFirstParam;
};