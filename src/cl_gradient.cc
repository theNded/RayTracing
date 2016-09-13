//
// Created by Neo on 16/9/13.
//

#include "cl_gradient.h"

CLGradient::CLGradient(std::string kernel_path,
                       std::string kernel_name,
                       cl_utils::Context *cl_context) {
  context_ = cl_context;
  kernel_ = cl_utils::LoadKernel(kernel_path, kernel_name,
                                 context_->device(), context_->context());
}

void CLGradient::Init(VolumeData &volume_data) {
  cl_image_format volume_format = volume_data.format();
  cl_image_desc   volume_desc   = volume_data.desc();
  volume_raw_ =
      clCreateImage(context_->context(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    &volume_format, &volume_desc,
                    volume_data.data(), NULL);

  volume_gradient_ =
      clCreateImage(context_->context(),
                    CL_MEM_WRITE_ONLY,
                    &volume_format, &volume_desc,
                    NULL, NULL);

  dims_   = volume_data.dims();

  global_work_size[0] = (size_t)volume_data.dims().s[0];
  global_work_size[1] = (size_t)volume_data.dims().s[1];
  global_work_size[2] = (size_t)volume_data.dims().s[2];
}