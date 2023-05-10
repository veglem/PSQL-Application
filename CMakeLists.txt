cmake_minimum_required(VERSION 3.26)
project(BoostBeastServer)

set(CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIR "C:/Program Files/Clibs/boost_1_81_0")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

if (CMAKE_BUILD_TYPE STREQUAL  "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
endif()

find_package (Boost COMPONENTS system REQUIRED)

if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
    add_executable(BoostBeastServer main.cpp Server/Server.cpp Server/Server.h Server/Session.cpp Server/Session.h Server/Heandler.cpp Server/Heandler.h modules/testResponce.cpp modules/testResponce.h)
    target_link_libraries (BoostBeastServer ${Boost_LIBRARIES} wsock32 ws2_32)
endif ()
