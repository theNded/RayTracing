//
// Created by Neo on 16/9/13.
//

#ifndef RAYTRACING_CL_GRADIENT_H
#define RAYTRACING_CL_GRADIENT_H

#include <string>

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "volume_data.h"

class CLGradient {
  CLGradient(std::string        kernel_path,
             std::string        kernel_name,
             cl_utils::Context *cl_context);
  ~CLGradient();

  void Init(VolumeData &volume_data);
  void Compute();

private:
  size_t             global_work_size[3];

  cl_utils::Context *context_;
  cl_kernel          kernel_;
  cl_mem             volume_raw_;
  cl_mem             volume_gradient_;

  cl_float3          dims_;
};


#endif //RAYTRACING_CL_GRADIENT_H
