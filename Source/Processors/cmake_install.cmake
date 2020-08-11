# Install script for directory: C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors

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
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/AudioNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/AudioResamplingNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Channel/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/DataThreads/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Dsp/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Editors/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Events/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/FileReader/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/GenericProcessor/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Merger/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/MessageCenter/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Parameter/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/PlaceholderProcessor/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/PluginManager/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/ProcessorGraph/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/ProcessorManager/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/RecordNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Serial/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/SourceNode/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Splitter/cmake_install.cmake")
  include("C:/Users/nkinsky/Documents/plugin-GUI2/Source/Processors/Visualization/cmake_install.cmake")

endif()

