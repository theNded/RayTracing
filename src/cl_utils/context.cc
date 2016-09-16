//
// Created by Neo on 16/8/6.
//

#include "cl_utils/context.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glx.h>

namespace cl_utils {

// Assume there is at least 1 platform and 1 device on it
Context::Context() {
  size_t info_length;
  char  *info;

  // Platform
  cl_platform_id platform;
  clGetPlatformIDs(1, &platform, NULL);

  clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &info_length);
  info = new char[info_length];
  clGetPlatformInfo(platform, CL_PLATFORM_NAME, info_length, info, NULL);
  std::cout << "Platform name          : " << std::string(info) << std::endl;
  delete[] info;

#if __linux__
    const cl_context_properties props[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
      0
    };

    cl_uint device_count;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &device_count);

    cl_device_id* devices = new cl_device_id[device_count];
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, device_count, devices, NULL);

    context_ = clCreateContext(props, 1, &devices[0], NULL, NULL, NULL);
    device_ = devices[0];

#elif __APPLE__
  // Context
  // If not Apple, use khr etc
  const cl_context_properties props[] = {
      CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
      (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
      0
  };
  context_ = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, 0, 0, NULL);

  // Device query and init
  // If not Apple, use clGetDeviceIDs etc
  clGetGLContextInfoAPPLE(context_, CGLGetCurrentContext(),
                          CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE,
                          sizeof(cl_device_id), &device_, NULL);
#endif

  clGetDeviceInfo(device_, CL_DEVICE_NAME, 0, NULL, &info_length);
  info = new char[info_length];
  clGetDeviceInfo(device_, CL_DEVICE_NAME, info_length, info, NULL);
  std::cout << "Device name            : " << std::string(info) << std::endl;
  delete[] info;

  clGetDeviceInfo(device_, CL_DEVICE_VERSION, 0, NULL, &info_length);
  info = new char[info_length];
  clGetDeviceInfo(device_, CL_DEVICE_VERSION, info_length, info, NULL);
  std::cout << "Device version         : " << std::string(info) << std::endl;
  delete[] info;

  clGetDeviceInfo(device_, CL_DRIVER_VERSION, 0, NULL, &info_length);
  info = new char[info_length];
  clGetDeviceInfo(device_, CL_DRIVER_VERSION, info_length, info, NULL);
  std::cout << "Driver version         : " << std::string(info) << std::endl;
  delete[] info;

  clGetDeviceInfo(device_, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &info_length);
  info = new char[info_length];
  clGetDeviceInfo(device_, CL_DEVICE_OPENCL_C_VERSION, info_length, info, NULL);
  std::cout << "Device opencl version  : " << std::string(info)
            << std::endl;
  delete[] info;

  clGetDeviceInfo(device_, CL_DEVICE_EXTENSIONS, 0, NULL, &info_length);
  info = new char[info_length];
  clGetDeviceInfo(device_, CL_DEVICE_EXTENSIONS, info_length, info, NULL);
  std::cout << "Device extensions      : " << std::string(info) << std::endl;
  delete[] info;

  cl_uint component_units;
  clGetDeviceInfo(device_, CL_DEVICE_MAX_COMPUTE_UNITS,
                  sizeof(component_units), &component_units, NULL);
  std::cout << "Parallel compute units : " << component_units << std::endl;

  // Command queue
  queue_   = clCreateCommandQueue(context_, device_, 0, NULL);
}

Context::~Context() {
  clReleaseCommandQueue(queue_);
  clReleaseContext(context_);
  clReleaseDevice(device_);
}

cl_device_id & Context::device() {
  return device_;
}

cl_context & Context::context() {
  return context_;
}

cl_command_queue & Context::queue() {
  return queue_;
}
}