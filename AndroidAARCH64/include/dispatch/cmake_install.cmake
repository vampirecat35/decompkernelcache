# Install script for directory: /Users/andyvand/Downloads/libdispatch/dispatch

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/andyvand/Downloads/android-ndk-r21b/aarch64_prefix")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/dispatch" TYPE FILE FILES
    "/Users/andyvand/Downloads/libdispatch/dispatch/base.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/data.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/dispatch.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/group.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/io.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/object.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/once.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/queue.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/semaphore.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/source.h"
    "/Users/andyvand/Downloads/libdispatch/dispatch/time.h"
    )
endif()

