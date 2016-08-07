//
// Created by Neo on 16/8/6.
//

#include "cl_utils/kernel.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace cl_utils {

std::string ReadKernel(std::string file_path) {
  std::string kernel_src = "";
  std::ifstream kernel_stream(file_path, std::ios::in);
  if (kernel_stream.is_open()) {
    for (std::string line; std::getline(kernel_stream, line);) {
      kernel_src += line + "\n";
    }
    kernel_stream.close();
  }

  else {
    std::cout << "Invalid kernel path ..." << std::endl;
    exit(1);
  }
  return kernel_src;
}

cl_kernel LoadKernel(std::string file_path,
                     std::string program_name,
                     cl_device_id &device,
                     cl_context &context) {

  std::string string_src = ReadKernel(file_path);
  const char *src = string_src.c_str();

  cl_program program = clCreateProgramWithSource(context, 1, &src, NULL, NULL);
  if (CL_SUCCESS != clBuildProgram(program, 1, &device, NULL, NULL, NULL)) {
    std::cout << "Error building !" << std::endl;
    size_t info_len;
    char *info;

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                          0, NULL, &info_len);
    info = new char[info_len];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                          info_len, info, NULL);
    std::cout << std::string(info) << std::endl;
    delete[] info;
    exit(1);
  }

  cl_kernel kernel = clCreateKernel(program, program_name.c_str(), NULL);
  return kernel;
}
}