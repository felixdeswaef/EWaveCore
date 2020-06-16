cmake_minimum_required(VERSION 3.13)
project(RaySim)
#SETTING opelCL library locations
set(OpenCL_INCLUDE_DIR "G://APPDATAIII//NVIDIA//CUDA//v102//include//")
set(OpenCL_LIBRARY "G://APPDATAIII//NVIDIA//CUDA//v102//lib//x64//OpenCL.lib")
#SETTING Helper library location
link_directories("G://Documents//Clionprojects//clprojtmpl - Copy//cmake-build-debug//OCLHELPER")
include_directories("G://Documents//Clionprojects//clprojtmpl - Copy//")


set(verbose 0)
set(CMAKE_CXX_STANDARD 14)
find_package(OpenCL REQUIRED)

# MAKE LIBRARY


# MAKE EXECUTABLE
add_executable(RaySim main.cpp formatDefinitions.cpp formatDefinitions.h raySpawn.cpp raySpawn.h CLFUNC.cpp CLFUNC.h envSpawn.cpp envSpawn.h main.h)
target_link_libraries(RaySim oclhelper) #LINK Helper library
target_link_libraries(RaySim OpenCL::OpenCL) #LINK openCL library