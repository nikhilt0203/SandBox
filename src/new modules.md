Requirements for adding a new module:

  1. Must derive from Module base class

  2. Must have a static Module::ModuleConfig::Info with the required fields
  
  3. New Type must be added in ModuleConfig.h

  4. Must call registerModule() in ModuleBuilder::init()

  5. Must initialize m_UIElement in the constructor before any UI/color changes are called

  6. Must initialize m_Type to INFO.TYPE

If more than one AudioStream object is being used, specifc port/device configurations can be made with 
m_Ports.setInputDevice() and m_Ports.setOutputDevice().
