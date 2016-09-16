//
// Created by Neo on 16/9/11.
//

#ifndef RAYTRACING_VOLUME_DATA_H
#define RAYTRACING_VOLUME_DATA_H

#include <string>

#ifdef __linux__
#include <CL/cl.h>
#elif __APPLE__
#include <OpenCL/opencl.h>
#endif

class VolumeData {
public:
  VolumeData(std::string data_path,
             size_t width, size_t height, size_t depth,
             float scale_x, float scale_y, float scale_z,
             size_t unit_size);
  ~VolumeData();

  size_t          unit_size() const;
  cl_float3       dims()   const;
  cl_float3       scales() const;
  cl_image_format format() const;
  cl_image_desc   desc()   const;
  void           *data();

private:
  char           *data_ptr_;
  size_t          data_byte_count_;
  size_t          data_unit_size_;

  cl_float3       data_dims_;
  cl_float3       data_scales_;
  cl_channel_type data_channel_type_;
  cl_image_format data_format_;
  cl_image_desc   data_desc_;
};


#endif //RAYTRACING_VOLUME_DATA_H
