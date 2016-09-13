//
// Created by Neo on 16/8/6.
// Enumerate usable devices, create context and command queue
//

#ifndef RAYTRACING_CLUTILS_CONTEXT_H
#define RAYTRACING_CLUTILS_CONTEXT_H

#include <vector>

#include <OpenCL/opencl.h>

namespace cl_utils {
class Context {
public:
  Context();
  ~Context();

  cl_device_id     &device();
  cl_context       &context();
  cl_command_queue &queue();

private:
  cl_device_id     device_;
  cl_context       context_;
  cl_command_queue queue_;
};
}


#endif //RAYTRACING_CONTEXT_H
