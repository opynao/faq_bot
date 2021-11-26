cmake_minimum_required(VERSION 3.10)

#set(CMAKE_CXX_COMPILER g++-11)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
#    -Werror
    -Wnon-virtual-dtor
    -Wunused
#   -Wshadow //tgbot
#    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
#    -Wconversion
#    -Wsign-conversion 
    -Wnull-dereference 
    -Wdouble-promotion
    -Wformat=2
    -lpthread
    -lstdc++fs
    -lgmock
    -lgtest 
)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${CMAKE_PROJECT_NAME}_lib ${SRC_FILES})
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_lib TgBot)

target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
# find_package(CURL)
# include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
# if (CURL_FOUND)
#     include_directories(${CURL_INCLUDE_DIRS})
#     add_definitions(-DHAVE_CURL)
# endif()

add_subdirectory(tgbot-cpp)
