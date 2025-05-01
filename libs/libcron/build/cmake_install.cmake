<<<<<<< HEAD
# Install script for directory: /home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron
=======
# Install script for directory: /home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1
>>>>>>> main

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
<<<<<<< HEAD
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/libcron/out/liblibcron.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/libcron/include/libcron")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/libcron/externals/date/include/date")
=======
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/libcron/out/liblibcron.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/libcron/include/libcron")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/libcron/externals/date/include/date")
>>>>>>> main
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
<<<<<<< HEAD
  include("/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/build/libcron/cmake_install.cmake")
  include("/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/build/test/cmake_install.cmake")
=======
  include("/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/build/libcron/cmake_install.cmake")
  include("/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/build/test/cmake_install.cmake")
>>>>>>> main

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
file(WRITE "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/libs/libcron/build/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc-v3.5.0/ms1/libs/libcron_v1.3.1/build/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> main
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
