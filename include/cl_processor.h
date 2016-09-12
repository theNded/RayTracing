//
// Created by Neo on 16/8/9.
//

#ifndef RAYTRACING_CL_PROCESSOR_H
#define RAYTRACING_CL_PROCESSOR_H

#include <string>

#include <GL/glew.h>

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "transfer_function.h"
#include "volume_data.h"

class CLProcessor {
public:
  CLProcessor(std::string        kernel_path,
              std::string        kernel_name,
              cl_utils::Context *cl_context);
  ~CLProcessor();

  void Init(VolumeData &volume_data,
            TransferFunction &transfer_function,
            GLuint texture);
  void Compute(cl_float3 r1, cl_float3 r2, cl_float3 r3,
               cl_float3 camera,
               cl_float2 f);

private:
  static const cl_image_format kFormat;
  static const cl_image_desc   kDesc;
  static const size_t          kGlobalWorkSize[];

  cl_utils::Context *context_;
  cl_kernel          kernel_;
  cl_mem             volume_;
  cl_mem             image_;
  cl_mem             transfer_function_;

  cl_float3          dims_;
  cl_float3          scales_;
};


#endif //RAYTRACING_CL_PROCESSOR_H
