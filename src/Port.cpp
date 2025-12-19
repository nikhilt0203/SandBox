#include "Port.h"
#include "Module.h"

Port::Port() : m_ConnectedModule(nullptr), m_Device(nullptr), m_DevicePortNum(0) {}

Port::Port(const char* name) : m_ConnectedModule(nullptr), m_Device(nullptr), m_DevicePortNum(0), m_Name(name) {}

bool Port::isFree() const { return m_ConnectedModule == nullptr; }