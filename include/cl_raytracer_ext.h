//
// Created by Neo on 16/8/9.
// Construct: kernel -> kernel
//                   -> kernel function name
// Init:      input  -> volume_data
//                   -> transfer_function
//                   -> gradient (TODO)
//            output -> texture
//

#ifndef RAYTRACING_CL_RAYTRACER_EXT_H
#define RAYTRACING_CL_RAYTRACER_EXT_H

#include <string>

#include <GL/glew.h>

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "transfer_function.h"
#include "volume_data.h"

class CLRayTracerShaded {
public:
  // Construct format:
  // @kernel path
  // @kernel function name
  // @context
  CLRayTracerShaded(std::string        kernel_path,
                    std::string        kernel_name,
                    cl_utils::Context *cl_context);
  ~CLRayTracerShaded();

  // Init format:
  // @input data ..
  // @output data ..
  // @additional params

  // This specification assumes:
  // @output is stored in arrays or other handlers (NOT OpenCL format)
  // that reads from cl_mem with "clEnqueueReadImage" or similar operations

  // One should provide a specific interface
  // if directly passing cl_mem is needed
  void Init(VolumeData &volume_data,
            TransferFunction &transfer_function,
            unsigned char *gradient,
            GLuint texture,
            size_t texture_width, size_t texture_height);

  // Compute format:
  // Assuming data is prepared
  // @instance parameters
  void Compute(cl_float3 r1, cl_float3 r2, cl_float3 r3,
               cl_float3 camera,
               cl_float2 f);

private:
  size_t             global_work_size[2];

  cl_utils::Context *context_;
  cl_kernel          kernel_;

  // input
  cl_mem             volume_;
  cl_mem             gradient_;
  cl_mem             transfer_function_;

  // output
  cl_mem             image_;

  // parameters
  cl_float3          dims_;
  cl_float3          scales_;
};


#endif //RAYTRACING_CL_PROCESSOR_H
