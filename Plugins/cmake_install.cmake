# Install script for directory: C:/Users/nkinsky/Documents/plugin-GUI2/Plugins

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/open-ephys-GUI")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/ArduinoOutput/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/BasicSpikeDisplay/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/CAR/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/ChannelMappingNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/EvntTrigAvg/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/FilterNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/IntanRecordingController/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/LfpDisplayNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/LfpDisplayNodeBeta/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/PhaseDetector/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/PulsePalOutput/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/RecordControl/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/Rectifier/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/RhythmNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/SerialInput/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/SpikeSorter/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/NatPlugin3/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Plugins/RippleDetector/cmake_install.cmake")

endif()

