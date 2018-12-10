# This file ensures the appropriate variables are set up for a project extending
# VTK before including vtkModuleMacros. This is the preferred way for a project
# building against VTK to use the CMake infrastructure provided for module
# developers.

if(NOT VTK_FOUND)
  message(FATAL_ERROR "VTK must be found before module macros can be used.")
endif()
if(VTK_VERSION VERSION_LESS "6.0")
  message(FATAL_ERROR "Requires VTK 6.0 or later to work.")
endif()

include(${VTK_CMAKE_DIR}/vtkDirectories.cmake)
include(${VTK_CMAKE_DIR}/vtkInitializeBuildType.cmake)
include(${VTK_CMAKE_DIR}/vtkInitializeWrapPython.cmake)

# Use VTK's flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${VTK_REQUIRED_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VTK_REQUIRED_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${VTK_REQUIRED_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${VTK_REQUIRED_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${VTK_REQUIRED_MODULE_LINKER_FLAGS}")
option(BUILD_SHARED_LIBS "Build VTK module with shared libraries." ${VTK_BUILD_SHARED_LIBS})
if(NOT CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(NOT VTK_MODULES_DIR)
  set(VTK_MODULES_DIR "${VTK_DIR}/${VTK_INSTALL_PACKAGE_DIR}/Modules")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${VTK_MODULES_DIR})


include(vtkEncodeString)
include(vtkModuleMacros)
include(vtkObjectFactory)
include(module.cmake OPTIONAL RESULT_VARIABLE _found)
if(_found)
  set(${vtk-module}-targets ${vtk-module}Targets)
  set(${vtk-module}-targets-install "\${VTK_INSTALL_PREFIX}/${VTK_INSTALL_PACKAGE_DIR}/Modules/Targets/${vtk-module}Targets.cmake")
  set(${vtk-module}_TARGETS_FILE_INSTALL "${${vtk-module}-targets-install}")
  set(${vtk-module}-targets-build-directory "${VTK_MODULES_DIR}/Targets")
  file(MAKE_DIRECTORY ${${vtk-module}-targets-build-directory})
  set(${vtk-module}-targets-build "${${vtk-module}-targets-build-directory}/${vtk-module}Targets.cmake")
  set(${vtk-module}_TARGETS_FILE_BUILD "${${vtk-module}-targets-build}")
  file(WRITE "${${vtk-module}_TARGETS_FILE_BUILD}" "") # Clear targets
endif()
