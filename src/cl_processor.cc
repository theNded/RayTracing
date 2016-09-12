//
// Created by Neo on 16/8/9.
//

#include "cl_processor.h"

const size_t CLProcessor::kGlobalWorkSize[] =
    {512, 512};

CLProcessor::CLProcessor(std::string        kernel_path,
                         std::string        kernel_name,
                         cl_utils::Context *cl_context) {
  context_ = cl_context;
  kernel_ = cl_utils::LoadKernel(kernel_path, kernel_name,
                                 context_->device(), context_->context());
}

void CLProcessor::Init(VolumeData &volume_data,
                       TransferFunction &transfer_function,
                       GLuint texture) {
  cl_image_format volume_format = volume_data.format();
  cl_image_desc   volume_desc   = volume_data.desc();
  volume_ =
      clCreateImage(context_->context(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    &volume_format, &volume_desc,
                    volume_data.data(), NULL);
  cl_image_format tf_format = transfer_function.format();
  cl_image_desc   tf_desc   = transfer_function.desc();
  transfer_function_ =
     clCreateImage(context_->context(),
                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                   &tf_format, &tf_desc,
                   transfer_function.transfer_function_data(), NULL);

  image_ = clCreateFromGLTexture(context_->context(),
                                 CL_MEM_WRITE_ONLY,
                                 GL_TEXTURE_2D, 0, texture, NULL);
  range_ = volume_data.dims();
}

void CLProcessor::Compute(cl_float3 r1, cl_float3 r2, cl_float3 r3,
                          cl_float3 camera, cl_float2 f) {
  clEnqueueAcquireGLObjects(context_->queue(), 1,  &image_, 0, 0, NULL);

  clSetKernelArg(kernel_, 0, sizeof(cl_mem),    &volume_);
  clSetKernelArg(kernel_, 1, sizeof(cl_mem),    &transfer_function_);
  clSetKernelArg(kernel_, 2, sizeof(cl_mem),    &image_);

  clSetKernelArg(kernel_, 3, sizeof(cl_float3), &r1);
  clSetKernelArg(kernel_, 4, sizeof(cl_float3), &r2);
  clSetKernelArg(kernel_, 5, sizeof(cl_float3), &r3);

  clSetKernelArg(kernel_, 6, sizeof(cl_float3), &camera);
  clSetKernelArg(kernel_, 7, sizeof(cl_float2), &f);

  clSetKernelArg(kernel_, 8, sizeof(cl_float3), &range_);

  clEnqueueNDRangeKernel(context_->queue(), kernel_, 2, NULL,
                         kGlobalWorkSize, NULL,
                         0, NULL, NULL);

  clFinish(context_->queue());
  clEnqueueReleaseGLObjects(context_->queue(), 1,  &image_, 0, 0, NULL);
}

CLProcessor::~CLProcessor() {
  clReleaseMemObject(volume_);
  clReleaseMemObject(image_);

  // TODO: change api of LoadKernel and
  // allow "program_" to involve more than 1 kernel
  // clRelease(program_);
  clReleaseKernel(kernel_);
}