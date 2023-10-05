# Minimum required version of CMake
cmake_minimum_required(VERSION 3.0)

option(CROSS_COMPILE_ARM64 "Cross compile for ARM64" OFF)
option(MOSQUITTO "mosquitto src-sink support" OFF)
option(PAHO "paho.mqtt.c src-sink support" ON)


if(CROSS_COMPILE_ARM64)
    message("Cross-compiling for ARM64")
    set(CMAKE_TOOLCHAIN_FILE "cmake/toolchain-arm64.cmake")
endif()


# Project name
project(xsolarbridge)

# Add the source and include directories
#add_subdirectory(src)
include_directories(include)

# List of source files
set(SOURCES 
    src/main.c
    src/datalog.c
    src/logger.c
    src/squeue.c    
    src/configuration.c
    src/list.c
    src/hashtable.c
)

# List of header files
set(HEADERS
    include/datalog.h
    include/logger.h
    include/squeue.h
)

set(LINK_LIBRARIES
    cjson config pthread)

if(MOSQUITTO)
    message("MOSQUITTO flag is defined.")
    add_definitions(-DMOSQUITTO)
    list(APPEND SOURCES src/mosq_sink.c)
    list(APPEND SOURCES src/mosq_src.c)
    list(APPEND LINK_LIBRARIES mosquitto)
else()
    message("MOSQUITTO flag is not defined!")
endif()

if(PAHO)
    message("PAHO-mqtt flag is defined.")
    add_definitions(-DPAHO)
    list(APPEND SOURCES src/mqtt_sink.c)
    list(APPEND SOURCES src/mqtt_src.c)
    list(APPEND LINK_LIBRARIES paho-mqtt3c)
else()
    message("PAHO-mqtt flag is not defined!")
endif()

# Create an executable from the sources
add_executable(xsolarbridge ${SOURCES} ${HEADERS})
target_link_libraries(xsolarbridge PRIVATE ${LINK_LIBRARIES})
