//
// Created by Neo on 16/8/6.
//

#include "cl_utils/context.h"

#include <iostream>

namespace cl_utils {

Context::Context() {
  cl::Platform::get(&platforms_);
  if (platforms_.size() == 0) {
    std::cout << "No OpenCL platforms found." << std::endl;
    exit(1);
  }

  platforms_[0].getDevices(CL_DEVICE_TYPE_GPU, &devices_);
  if (devices_.size() == 0) {
    std::cout << "No OpenCL devices found." << std::endl;
    exit(1);
  }

  device_ = devices_[0];
  std::cout << "Using device: "
            << device_.getInfo<CL_DEVICE_NAME>()
            << std::endl;
  std::cout << "Using platform: "
            << platforms_[0].getInfo<CL_PLATFORM_NAME>()
            << std::endl;

  context_ = cl::Context(device_);
  queue_   = cl::CommandQueue(context_, device_, 0, NULL);

}

cl::Device & Context::device() {
  return device_;
}

cl::Context & Context::context() {
  return context_;
}

cl::CommandQueue & Context::queue() {
  return queue_;
}
}