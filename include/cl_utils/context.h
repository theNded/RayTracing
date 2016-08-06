//
// Created by Neo on 16/8/6.
//

#ifndef RAYTRACING_CLUTILS_CONTEXT_H
#define RAYTRACING_CLUTILS_CONTEXT_H

#include "cl.hpp"
#include <vector>

namespace cl_utils {
class Context {
public:
  Context();
  cl::Device & device();
  cl::Context & context();
  cl::CommandQueue &queue();

private:
  std::vector<cl::Platform> platforms_;
  std::vector<cl::Device>   devices_;
  cl::Device                device_;
  cl::Context               context_;
  cl::CommandQueue          queue_;
};
}


#endif //RAYTRACING_CONTEXT_H
