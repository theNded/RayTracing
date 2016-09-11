//
// Created by Neo on 16/9/11.
//

#include "volume_data.h"

#include <fstream>

VolumeData::VolumeData(std::string data_path,
                       size_t width, size_t height, size_t depth,
                       size_t unit_size) {
  data_byte_count_ = width * height * depth * unit_size;
  data_ptr_ = new char [data_byte_count_];
  std::ifstream volume_data_file(data_path, std::ios::in | std::ios::binary);
  volume_data_file.read(data_ptr_, data_byte_count_);

  data_channel_type_ = (unit_size == 1) ? CL_UNORM_INT8 : CL_UNORM_INT16;
  data_format_ = {CL_INTENSITY, data_channel_type_};
  data_desc_ = {CL_MEM_OBJECT_IMAGE3D, width, height, depth};

  data_dims_.s[0] = width;
  data_dims_.s[1] = height;
  data_dims_.s[2] = depth;
}

VolumeData::~VolumeData() {
  delete[] data_ptr_;
}

cl_image_desc VolumeData::desc() const {
  return data_desc_;
}

cl_image_format VolumeData::format() const {
  return data_format_;
}

cl_float3 VolumeData::dims() const {
  return data_dims_;
}

void* VolumeData::data() {
  return data_ptr_;
}