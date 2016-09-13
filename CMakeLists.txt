CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(RayTracing)

#----------
# C++ version setting
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")
#----------

#----------
# Project variable configurations
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(RayTracing_LIBRARIES "")
SET(RayTracing_SOURCES
        src/raytracing.cc
        src/volume_data.cc
        src/transfer_function.cc
        src/gl_processor.cc
        src/cl_raytracer.cc
        src/cl_raytracer_ext.cc
        src/cl_gradient.cc
        src/cl_utils/context.cc
        src/cl_utils/kernel.cc
        src/gl_utils/context.cc
        src/gl_utils/control.cc
        src/gl_utils/model.cc
        src/gl_utils/shader.cc
        src/gl_utils/texture.cc)
#----------

#----------
# Package configurations
FIND_PACKAGE(PkgConfig)
## Find packages
LIST(APPEND
        CMAKE_MODULE_PATH
        ${PROJECT_SOURCE_DIR}/cmake_modules)
FIND_PACKAGE(GLFW3 REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(RapidJSON)

## Include
INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/include
        ${GLFW3_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${OpenCL_INCLUDE}
        ${RapidJSON_INCLUDE})
## Lib
LIST(APPEND
        RayTracing_LIBRARIES
        ${OPENGL_LIBRARY}
        ${OpenCL_LIBRARY}
        ${GLFW3_LIBRARIES}
        ${GLEW_LIBRARY})
#----------

#---------
# Compile and Link
ADD_EXECUTABLE(RayTracing ${RayTracing_SOURCES})
TARGET_LINK_LIBRARIES(RayTracing
        ${RayTracing_LIBRARIES}
        ${OpenCV_LIBS})
#---------