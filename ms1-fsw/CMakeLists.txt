####
# This sets up the build system for the 'ms1' project, including
# components and deployments from project.cmake. In addition, it imports the core F Prime components.
####

cmake_minimum_required(VERSION 3.6)
project(ms1 C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## DEPENDENCIES
#find_program(st-info NAMES st-info REQUIRED)
#find_program(st-flash NAMES st-flash REQUIRED)

#FetchContent_Declare(
#	fuzztest
#	GIT_REPOSITORY https://github.com/google/fuzztest.git 
#	GIT_TAG main
#)
#FetchContet_MakeAvailable(fuzztest)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenSSL REQUIRED)

add_library(libcron STATIC
		${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/CronClock.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/TimeTypes.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/CronData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/CronSchedule.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/Task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/Cron.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/CronRandomization.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/libcron/DateTime.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/src/CronClock.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/src/CronData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/src/CronSchedule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/src/Task.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/src/CronRandomization.cpp
)

# Set include directories properly for both build and install phases
target_include_directories(libcron
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/externals/date/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include>
        $<INSTALL_INTERFACE:include/date>
        $<INSTALL_INTERFACE:include/libcron>
)

link_libraries(libcron)
link_libraries(OpenSSL::SSL OpenSSL::Crypto)

# Install headers to the correct destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/externals/date/include/
        DESTINATION include/date)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libcron/libcron/include/
        DESTINATION include/libcron)

# Properly export libcron for GTestTargets
install(TARGETS libcron
    EXPORT GTestTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(EXPORT GTestTargets
    FILE GTestTargets.cmake
    NAMESPACE GTest::
    DESTINATION lib/cmake/GTest
)


###
# F' Core Setup
# This includes all of the F prime core components, and imports the make-system.

###
include("${CMAKE_CURRENT_LIST_DIR}/fprime/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
fprime_setup_included_code()

# This includes project-wide objects
include("${CMAKE_CURRENT_LIST_DIR}/project.cmake")
