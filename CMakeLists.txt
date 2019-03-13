cmake_minimum_required(VERSION 3.13)
project(ComputerVisionProject C)

set(CMAKE_C_STANDARD 11)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.c")

include_directories ("${PROJECT_SOURCE_DIR}/maxtree")
add_subdirectory (src/maxtree)

#add_executable(ComputerVisionProject ${SOURCES})
add_executable(ComputerVisionProject src/main.c)

target_link_libraries(ComputerVisionProject m freeimage maxtree)
