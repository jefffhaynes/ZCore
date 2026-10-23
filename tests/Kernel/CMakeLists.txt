cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(KernelTests)

set(CORE ${CMAKE_CURRENT_LIST_DIR}/../../src)

include_directories(
    ${CORE}
    ${CORE}/Units
    ${CORE}/Streams
    )

target_sources(app PRIVATE
    src/MessageQueueTests.cpp
    )
