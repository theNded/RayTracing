//
// Created by Neo on 16/9/11.
//

#ifndef RAYTRACING_TRANSFER_FUNCTION_H
#define RAYTRACING_TRANSFER_FUNCTION_H

#include <vector>
#include <OpenCL/opencl.h>

class TransferFunction {
public:
  struct float4 {
    float r, g, b, a;
    float4() {r = g = b = a = 0.0f;}
    float4(float ri, float gi, float bi, float ai) :
        r(ri), g(gi), b(bi), a(ai) {}
    float4 operator + (float4 o) {
      return float4(r + o.r, g + o.g, b + o.b, a + o.a);
    }
    float4 operator - (float4 o) {
      return float4(r - o.r, g - o.g, b - o.b, a - o.a);
    }
    float4 operator / (float4 o) {
      return float4(r / o.r, g / o.g, b / o.g, a / o.g);
    }
    float4 operator * (float4 o) {
      return float4(r * o.r, g * o.g, b * o.g, a * o.g);
    }
    float4 operator * (float scalar) {
      return float4(r * scalar, g * scalar, b * scalar, a * scalar);
    }
    static float4 One() {
      return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
  };

  struct ControlPoint {
    float4 rgba;
    int    val;

    ControlPoint(float r, float g, float b, int v) {
      rgba = float4(r, g, b, 0.0f);
      val = v;
    }

    ControlPoint(float a, int v) {
      rgba = float4(0, 0, 0, a);
      val = v;
    }
  };

  struct Cubic {
    float4 a;
    float4 b;
    float4 c;
    float4 d;

    Cubic(float4 ai, float4 bi, float4 ci, float4 di) :
    a(ai), b(bi), c(ci), d(di) {}

    float4 operator() (float x) {
      return ((((d * x) + c) * x) + b) * x + a;
    }
  };

  TransferFunction(std::string transfer_function_path);
  TransferFunction(std::vector<ControlPoint> color_control_points,
                   std::vector<ControlPoint> alpha_control_points);

  unsigned char *transfer_function_data();

private:
  std::vector<TransferFunction::Cubic> CubicsFromControlPoints(
      std::vector<TransferFunction::ControlPoint> points);

  std::vector<Cubic> color_cubics_;
  std::vector<Cubic> alpha_cubics_;

  unsigned char transfer_function_data_[256 * 4];
};


#endif //RAYTRACING_TRANSFER_FUNCTION_H
