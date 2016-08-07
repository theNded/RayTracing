//
// Created by Neo on 16/8/6.
//

#ifndef RAYTRACING_KERNEL_H
#define RAYTRACING_KERNEL_H

#include <string>

#include <OpenCL/opencl.h>

namespace cl_utils {
cl_kernel LoadKernel(std::string kernel_path,
                      std::string program_name,
                      cl_device_id  &device,
                      cl_context &context);
}


#endif //RAYTRACING_KERNEL_H
