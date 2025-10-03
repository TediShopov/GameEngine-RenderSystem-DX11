# Install script for directory: D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/pkgs/bullet3_x86-windows/debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/bullet" TYPE FILE FILES
    "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/btBulletCollisionCommon.h"
    "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/btBulletDynamicsCommon.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/BulletInverseDynamics/cmake_install.cmake")
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/BulletSoftBody/cmake_install.cmake")
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/BulletCollision/cmake_install.cmake")
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/BulletDynamics/cmake_install.cmake")
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/LinearMath/cmake_install.cmake")
  include("D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-dbg/src/Bullet3Common/cmake_install.cmake")

endif()

