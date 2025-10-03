# Install script for directory: D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/BulletSoftBody

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/pkgs/bullet3_x64-windows/debug")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/x64-windows-dbg/lib/BulletSoftBody_Debug.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/bullet" TYPE DIRECTORY FILES "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/BulletSoftBody" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/\\.svn$" EXCLUDE REGEX "/cmakefiles$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletSoftBodyTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletSoftBodyTargets.cmake"
         "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/x64-windows-dbg/src/BulletSoftBody/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletSoftBodyTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletSoftBodyTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletSoftBodyTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/bullet" TYPE FILE FILES "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/x64-windows-dbg/src/BulletSoftBody/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletSoftBodyTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/bullet" TYPE FILE FILES "D:/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/vcpkg/blds/bullet3/x64-windows-dbg/src/BulletSoftBody/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletSoftBodyTargets-debug.cmake")
  endif()
endif()

