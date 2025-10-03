# Install script for directory: D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/BulletCollision

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/pkgs/bullet3_x86-windows")
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
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-rel/lib/BulletCollision.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/bullet" TYPE DIRECTORY FILES "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/BulletCollision" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/\\.svn$" EXCLUDE REGEX "/cmakefiles$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/bullet/BulletCollision" TYPE FILE FILES "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/src/3.25-98f155ca90.clean/src/BulletCollision/../btBulletCollisionCommon.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletCollisionTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletCollisionTargets.cmake"
         "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-rel/src/BulletCollision/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletCollisionTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletCollisionTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/bullet/BulletCollisionTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/bullet" TYPE FILE FILES "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-rel/src/BulletCollision/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletCollisionTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/bullet" TYPE FILE FILES "D:/ProceduralMethods/cmp301_coursework-TediShopov/Coursework/vcpkg_installed/x86-windows/vcpkg/blds/bullet3/x86-windows-rel/src/BulletCollision/CMakeFiles/Export/6bd7685d1c82163fce1fd0e046ce46c5/BulletCollisionTargets-release.cmake")
  endif()
endif()

