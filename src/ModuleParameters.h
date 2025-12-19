#include "Module.h"
#include <functional>

class ModuleParameters 
{
public:
  ModuleParameters(size_t numParams) 
  : m_NumParams(numParams)
  {
    m_ParameterFuncs.reserve(numParams);
  }

  void addParameterFunc(size_t parameterNum, std::function<void(float)>* parameterFunc) 
  {
    if (parameterNum >= m_NumParams)
      return;
    m_ParameterFuncs.push_back(parameterFunc);
  }

private:
  std::vector<std::function<void(float)>*> m_ParameterFuncs;
  size_t m_NumParams;
};