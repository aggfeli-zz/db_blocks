cmake_minimum_required(VERSION 3.12)
project(baseis C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-static")
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)


include_directories(include)
link_directories(lib)

add_executable(baseis main.c HashTable.c SecondaryHashTable.c)
target_link_libraries(baseis BF_64)