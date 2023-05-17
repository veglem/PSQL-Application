cmake_minimum_required(VERSION 3.26)
project(BoostBeastServer)

set(CMAKE_CXX_STANDARD 11)

set(Boost_COMPILER mgw12)

set(Boost_INCLUDE_DIR "C:/Program Files/Clibs/boost_1_81_0")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

set(PostgreSQL_INCLUDE_DIRS "C:/Program Files/PostgreSQL/15/include")

set(Boost_SYSTEM_LIBRARY_DEBUG "C:/Program Files/Clibs/boost_1_81_0/lib/libboost_system-mgw12-mt-d-x64-1_81.a")

set(Boost_SYSTEM_LIBRARY_RELAESE "C:/Program Files/Clibs/boost_1_81_0/lib/libboost_system-mgw12-mt-x64-1_81.a")

if (CMAKE_BUILD_TYPE STREQUAL  "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -lpthread")

set(Boost_USE_STATIC_LIBS ON)

find_package(PostgreSQL REQUIRED)

find_package (Boost COMPONENTS system json REQUIRED)

if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR} )
    add_executable(BoostBeastServer main.cpp Server/Server.cpp Server/Server.h Server/Session.cpp Server/Session.h Server/Heandler.cpp Server/Heandler.h modules/testResponce.cpp modules/testResponce.h modules/Database.cpp modules/Database.h modules/testResponseSecond.cpp modules/testResponseSecond.h modules/responceHeaders.h modules/Auth.cpp modules/Company.cpp modules/urlDecode.h modules/Project.cpp modules/urlDecode.cpp modules/Developer.cpp)
    target_link_libraries (BoostBeastServer ${Boost_LIBRARIES} PostgreSQL::PostgreSQL wsock32 ws2_32 )
#    PostgreSQL::PostgreSQL
endif ()
