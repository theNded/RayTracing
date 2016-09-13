//
// Created by Neo on 16/9/13.
//

#include "cl_gradient.h"

#include <iostream>
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

  cl_image_format gradient_format = {CL_RGBA,
                                     volume_data.format().image_channel_data_type};
  cl_image_desc   gradient_desc   = volume_desc;
  volume_gradient_ =
      clCreateImage(context_->context(),
                    CL_MEM_WRITE_ONLY,
                    &gradient_format, &gradient_desc,
                    NULL, NULL);

  global_work_size[0] = (size_t)volume_data.dims().s[0];
  global_work_size[1] = (size_t)volume_data.dims().s[1];
  global_work_size[2] = (size_t)volume_data.dims().s[2];
}

void CLGradient::Compute() {
  cl_int step = 1;

  clSetKernelArg(kernel_, 0, sizeof(cl_mem),    &volume_raw_);
  clSetKernelArg(kernel_, 1, sizeof(cl_mem),    &volume_gradient_);
  clSetKernelArg(kernel_, 2, sizeof(cl_int),    &step);

  clEnqueueNDRangeKernel(context_->queue(), kernel_, 3, NULL,
                         global_work_size, NULL,
                         0, NULL, NULL);

  clFinish(context_->queue());

  size_t origin[3] = {0, 0, 0};
  size_t size[3]   = {global_work_size[0],
                      global_work_size[1],
                      global_work_size[2]};
  volume_gradient = new unsigned char [size[0] * size[1] * size[2] * 4];
  clEnqueueReadImage(context_->queue(), volume_gradient_, CL_TRUE,
                     origin, size,
                     0, 0, volume_gradient, 0, NULL, NULL);

}

CLGradient::~CLGradient() {
  delete[] volume_gradient;
  clReleaseMemObject(volume_raw_);
  clReleaseMemObject(volume_gradient_);

  // TODO: change api of LoadKernel and
  // allow "program_" to involve more than 1 kernel
  // clRelease(program_);
  clReleaseKernel(kernel_);
}