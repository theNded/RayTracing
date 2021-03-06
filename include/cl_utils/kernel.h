//
// Created by Neo on 16/8/6.
// Compile kernel function with source code and OpenCL context
// Return compiled kernel handler
//

#ifndef RAYTRACING_KERNEL_H
#define RAYTRACING_KERNEL_H

#include <string>

#if __linux__
#include <CL/cl.h>
#elif __APPLE__
#include <OpenCL/opencl.h>
#endif

namespace cl_utils {
cl_kernel LoadKernel(std::string   kernel_path,
                     std::string   kernel_name,
                     cl_device_id &device,
                     cl_context   &context);
}


#endif //RAYTRACING_KERNEL_H
