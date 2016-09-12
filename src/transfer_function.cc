//
// Created by Neo on 16/9/11.
//

#include "transfer_function.h"

#include <cassert>
#include <iostream>
#include <fstream>

inline unsigned char float2uchar(float x) {
  x = x < 0.0f ? 0.0f : x;
  x = x > 1.0f ? 1.0f : x;
  return (unsigned char) (x * 255);
}

TransferFunction::float4 TransferFunction::float4::One =
TransferFunction::float4(1.0f, 1.0f, 1.0f, 1.0f);

TransferFunction::TransferFunction(std::string transfer_function_path) {
  std::ifstream tf_stream(transfer_function_path, std::ios::in);
  char dummy;
  float value;
  for (int i = 0; i < 256; ++i) {
    for (int j = 0; j < 3; ++j) {
      tf_stream >> dummy >> dummy >> dummy >> value;
      transfer_function_data_[i * 4 + j] = float2uchar(value);
    }
    for (int j = 0; j < 3; ++j) {
      tf_stream >> dummy >> dummy >> dummy >> value;
      transfer_function_data_[i * 4 + 3] = float2uchar(value);
    }
  }
  for (int i = 0; i < 256; ++i) {
    std::cout << (int)transfer_function_data_[i * 4 + 3] << ",";
  }
}

TransferFunction::TransferFunction(std::vector<ControlPoint>
                                   color_control_points,
                                   std::vector<ControlPoint>
                                   alpha_control_points) {
  // (n + 1) control points
  // return 4 x n cubics
  color_cubics_ = CubicsFromControlPoints(color_control_points);
  // return 4 x n cubics
  alpha_cubics_ = CubicsFromControlPoints(alpha_control_points);

  int color_cubic_iterations_ = (int)color_control_points.size() - 1;
  int alpha_cubic_iterations_ = (int)alpha_control_points.size() - 1;

  // interpolate over color_control_points
  int index = 0;
  for (int i = 0; i < color_cubic_iterations_; ++i) {
    int step = color_control_points[i + 1].index
             - color_control_points[i].index;
    assert(step > 1);
    for (int j = 0; j < step; ++j) {
      float x = float(j) / float(step - 1);
      float4 fx = color_cubics_[i](x);
      transfer_function_data_[index + 0] = float2uchar(fx.r);
      transfer_function_data_[index + 1] = float2uchar(fx.g);
      transfer_function_data_[index + 2] = float2uchar(fx.b);
      index += 4;
    }
  }

  // interpolate over alpha_control_points
  index = 0;
  for (int i = 0; i < alpha_cubic_iterations_; ++i) {
    int step = alpha_control_points[i + 1].index
             - alpha_control_points[i].index;
    assert(step > 1);
    for (int j = 0; j < step; ++j) {
      float x = float(j) / float(step - 1);
      transfer_function_data_[index + 3] = float2uchar(alpha_cubics_[i](x).a);
      index += 4;
    }
  }
}

cl_image_format TransferFunction::format() const {
  return {CL_RGBA, CL_UNORM_INT8};
}

cl_image_desc TransferFunction::desc() const {
  return {CL_MEM_OBJECT_IMAGE1D, 256};
}

unsigned char * TransferFunction::transfer_function_data() {
  return transfer_function_data_;
}

// http://graphicsrunner.blogspot.jp/2009/01/volume-rendering-102-transfer-functions.html
std::vector<TransferFunction::Cubic> TransferFunction::CubicsFromControlPoints(
    std::vector<ControlPoint> v) {

  int n = (int)v.size() - 1;
  float4 *gamma = new float4[n + 1];
  float4 *delta = new float4[n + 1];
  float4 *D     = new float4[n + 1];

  /* We need to solve the equation
   * taken from: http://mathworld.wolfram.com/CubicSpline.html
     [2 1       ] [D[0]]   [3(v[1] - v[0])  ]
     |1 4 1     | |D[1]|   |3(v[2] - v[0])  |
     |  1 4 1   | | .  | = |      .         |
     |    ..... | | .  |   |      .         |
     |     1 4 1| | .  |   |3(v[n] - v[n-2])|
     [       1 2] [D[n]]   [3(v[n] - v[n-1])]

     by converting the matrix to upper triangular.
     The D[i] are the derivatives at the control points.
   */

  //this builds the coefficients of the left matrix
  gamma[0] = 0.5f * float4::One;
  for (int i = 1; i < n; ++i) {
    gamma[i] = float4::One / (4 * float4::One - gamma[i - 1]);
  }
  gamma[n] = float4::One / (2 * float4::One - gamma[n - 1]);

  delta[0] = 3 * (v[1].rgba - v[0].rgba) * gamma[0];
  for (int i = 1; i < n; ++i) {
    delta[i] = (3 * (v[i + 1].rgba - v[i - 1].rgba) - delta[i - 1]) * gamma[i];
  }
  delta[n] = (3 * (v[n].rgba - v[n - 1].rgba) - delta[n - 1]) * gamma[n];

  D[n] = delta[n];
  for (int i = n - 1; i >= 0; --i) {
    D[i] = delta[i] - gamma[i] * D[i + 1];
  }

  // now compute the coefficients of the cubics
  std::vector<Cubic> cubics;
  for (int i = 0; i < n; i++) {
    cubics.push_back(
        Cubic(v[i].rgba,
              D[i],
              3 * (v[i + 1].rgba - v[i].rgba) - 2 * D[i] - D[i + 1],
              2 * (v[i].rgba - v[i + 1].rgba) + D[i] + D[i + 1]));
  }
  return cubics;
}

