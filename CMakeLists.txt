cmake_minimum_required(VERSION 3.13)
project(EWaveCore)
#SETTING opelCL library locations
#set(OpenCL_INCLUDE_DIR "G://APPDATAIII//NVIDIA//CUDA//v102//include//")
#set(OpenCL_LIBRARY "G://APPDATAIII//NVIDIA//CUDA//v102//lib//x64//OpenCL.lib")
set(cudap $ENV{CUDA_PATH})
string(REPLACE "\\" "/" cudap "${cudap}")
set(ocpath $ENV{OCLH_PATH})
string(REPLACE "\\" "/" ocpath "${ocpath}")
message("bla ${ocpath} ${cudap}")

set(OpenCL_INCLUDE_DIR "${cudap}//include//")

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")#check 32 bit v
    set(OpenCL_LIBRARY "${cudap}//lib//win32//OpenCL.lib")

else()
    set(OpenCL_LIBRARY "${cudap}//lib//x64//OpenCL.lib")

endif()


#set(OpenCL_LIBRARY "C://Program Files//NVIDIA GPU Computing Toolkit//CUDA//v11.0//lib//win32//OpenCL.lib")#32 bit

#SETTING Helper library location
link_directories("${ocpath}//lib")
include_directories("${ocpath}")


set(verbose 0)
set(CMAKE_CXX_STANDARD 14)
find_package(OpenCL REQUIRED)

# MAKE LIBRARY
ADD_LIBRARY( EWaveCore STATIC
        formatDefinitions.cpp formatDefinitions.h raySpawn.cpp raySpawn.h CLFUNC.cpp CLFUNC.h envSpawn.cpp envSpawn.h eWaveCore.h)

# MAKE EXECUTABLE
add_executable(EWaveCoreTest main.cpp main.h)

target_link_libraries(EWaveCoreTest EWaveCore) #LINK Helper library
target_link_libraries(EWaveCore oclhelper) #LINK Helper library
target_link_libraries(EWaveCore OpenCL::OpenCL) #LINK openCL library