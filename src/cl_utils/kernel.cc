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

cl::Kernel LoadKernel(std::string file_path,
                      std::string program_name,
                      cl::Device  &device,
                      cl::Context &context) {
  cl::Program::Sources source;
  std::string src = ReadKernel(file_path);
  source.push_back({src.c_str(), src.length()});

  cl::Program program(context, source);
  if (CL_SUCCESS != program.build({device})) {
    std::cout << " Error building: "
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
              << std::endl;
    exit(1);
  }

  cl::Kernel kernel(program, program_name.c_str());
  return kernel;
}
}