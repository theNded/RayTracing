//
// Created by Neo on 16/9/11.
//

#ifndef RAYTRACING_VOLUME_DATA_H
#define RAYTRACING_VOLUME_DATA_H

#include <string>

#include <OpenCL/opencl.h>

class VolumeData {
public:
  VolumeData(std::string data_path,
             size_t width, size_t height, size_t depth,
             size_t unit_size);
  ~VolumeData();

  cl_float3       dims()   const;
  cl_image_format format() const;
  cl_image_desc   desc()   const;
  void           *data();

private:
  char *data_ptr_;
  size_t data_byte_count_;

  cl_float3       data_dims_;
  cl_channel_type data_channel_type_;
  cl_image_format data_format_;
  cl_image_desc   data_desc_;
};


#endif //RAYTRACING_VOLUME_DATA_H
