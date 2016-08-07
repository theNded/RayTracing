//
// Created by Neo on 16/8/6.
//

#include "cl_utils/context.h"

#include <iostream>

namespace cl_utils {

Context::Context() {
  // Assume there is at least 1 platform and 1 device on it
  cl_platform_id platform;
  clGetPlatformIDs(1, &platform, NULL);
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_, NULL);
  context_ = clCreateContext(NULL, 1, &device_, NULL, NULL, NULL);
  queue_   = clCreateCommandQueue(context_, device_, 0, NULL);

#define PRINT_CONTEXT_INFO
#ifdef PRINT_CONTEXT_INFO
  size_t info_length;
  char *info;
  clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &info_length);
  info = new char[info_length];
  clGetPlatformInfo(platform, CL_PLATFORM_NAME, info_length, info, NULL);
  std::cout << "Platform name          : " << std::string(info) << std::endl;
  delete[] info;

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
  std::cout << "Device                 : " << std::string(info) << std::endl;
  delete[] info;

  cl_uint component_units;
  clGetDeviceInfo(device_, CL_DEVICE_MAX_COMPUTE_UNITS,
                  sizeof(component_units), &component_units, NULL);
  std::cout << "Parallel compute units : " << component_units << std::endl;
#endif // PRINT_INFO
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