//
// Created by Neo on 16/8/6.
//

#ifndef RAYTRACING_KERNEL_H
#define RAYTRACING_KERNEL_H

#include <string>

#include "cl.hpp"

namespace cl_utils {
cl::Kernel LoadKernel(std::string kernel_path,
                      std::string program_name,
                      cl::Device  &device,
                      cl::Context &context);
}


#endif //RAYTRACING_KERNEL_H
