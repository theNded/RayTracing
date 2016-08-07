//
// Created by Neo on 16/8/6.
//

#include "cl_utils/context.h"

#include <iostream>

namespace cl_utils {

Context::Context() {
  cl_platform_id platform;
  clGetPlatformIDs(1, &platform, NULL);
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_, NULL);
  context_ = clCreateContext(NULL, 1, &device_, NULL, NULL, NULL);
  queue_   = clCreateCommandQueue(context_, device_, 0, NULL);
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