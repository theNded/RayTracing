//
// Created by Neo on 16/8/8.
//

#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "cl_raytracer_ext.h"
#include "cl_raytracer.h"

#include "gl_utils/context.h"
#include "gl_utils/control.h"
#include "gl_utils/model.h"
#include "gl_utils/shader.h"
#include "gl_utils/texture.h"

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "gl_processor.h"
#include "cl_raytracer.h"
#include "cl_raytracer_ext.h"
#include "cl_gradient.h"

#if __linux__
std::string kDefaultConfigPath = "/home/wei/data/SLAM_Prototype/";
#elif __APPLE__
std::string kDefaultConfigPath = "/Users/Neo/code/Data/VisMale/";
#endif

int main(int argc, char* args[]) {
  // Open config file:
  std::string config_path = (argc > 1) ? args[1] : kDefaultConfigPath;
  config_path += "config.json";

  FILE* config_fp = fopen(config_path.c_str(), "r");
  char buffer[4096];
  rapidjson::FileReadStream is(config_fp, buffer, sizeof(buffer));
  rapidjson::Document config;
  config.ParseStream(is);
  fclose(config_fp);

  // Parse config file:
  std::string volume_file_path = config["volume data path"].GetString();
  std::string tf_file_path     = config["transfer function path"].GetString();
  size_t dims[] = {
      config["dims"]["x"].GetUint64(),
      config["dims"]["y"].GetUint64(),
      config["dims"]["z"].GetUint64()
  };
  float scales[] = {
      config["scales"]["x"].GetFloat(),
      config["scales"]["y"].GetFloat(),
      config["scales"]["z"].GetFloat()
  };
  size_t unit_size = config["unit size"].GetUint64();

  // Input 3D scalar field to render
  VolumeData volume_data = VolumeData(volume_file_path,
                                      dims[0], dims[1], dims[2],
                                      scales[0], scales[1], scales[2],
                                      unit_size);
  // Specify transfer function
#define ManualAdjustTransferFunction_
#ifdef ManualAdjustTransferFunction
  typedef TransferFunction::ControlPoint ControlPoint;
  std::vector<ControlPoint> c;
r
  c.push_back(ControlPoint(.1f, .8f, .0f, 0));
  c.push_back(ControlPoint(.1f, .8f, .0f, 110));
  c.push_back(ControlPoint(.43f, .24f, .09f, 112));
  c.push_back(ControlPoint(.43f, .24f, .09f, 256));

  std::vector<ControlPoint> a;
  a.push_back(ControlPoint(0.0f, 0));
  a.push_back(ControlPoint(0.0f, 70));
  a.push_back(ControlPoint(0.2f, 111));
  a.push_back(ControlPoint(0.0f, 150));
  a.push_back(ControlPoint(1.0f, 256));
  TransferFunction tf = TransferFunction(c, a);
  tf.Save("/Users/Neo/Desktop/Bonsai2.tf");
#else
  TransferFunction tf = TransferFunction(tf_file_path);
#endif

  // This is the bridge of OpenCL and OpenGL:
  // cl.Compute -> output -> cgl_texture_handler -> input -> gl.Render
  GLuint cgl_texture_handler;

  // Init OpenGL context.
  // This step MUST BE AHEAD OF OpenCL initialization
  gl_utils::Context gl_context("CL x GL demo", 512, 512);
  gl_utils::Control gl_control(&gl_context);

  // Init OpenGL renderer
  GLProcessor gl_processor("cl_gl_vertex.glsl", "cl_gl_fragment.glsl",
                           "texture_sampler",
                           &gl_context);
  gl_processor.Init(cgl_texture_handler);

  // Init OpenCL context.
  cl_utils::Context cl_context = cl_utils::Context();

#define SHADED
#ifdef SHADED
#ifdef COMPUTE_GRADIENT
  // Init OpenCL gradient processor
  CLGradient cl_gradient_solver("gradient.cl",
                                "gradient",
                                &cl_context);
  cl_gradient_solver.Init(volume_data);
  cl_gradient_solver.Compute();

  unsigned char *gradient = cl_gradient_solver.volume_gradient;
#else
  int iterations = dims[0] * dims[1] * dims[2];

  unsigned char *gradient = new unsigned char [iterations * 4];
  char *gradient_tmp = new char [iterations * 3];

  std::string normal_file_path = config["normal data path"].GetString();
  std::ifstream nml_file(normal_file_path, std::ios::in | std::ios::binary);
  nml_file.read(gradient_tmp, iterations * 3 * sizeof(char));

  for (int i = 0; i < iterations; ++i) {
#ifdef NORMALIZE_FOR_SHORT
    unsigned char bx, by, bz;
    float nx = gradient_tmp[i * 3 + 0],
          ny = gradient_tmp[i * 3 + 1],
          nz = gradient_tmp[i * 3 + 2];
    float len = sqrt(nx * nx + ny * ny + nz * nz);
    if (len < 1e-6) {
      bx = by = bz = 0;
    } else {
      bx = (unsigned char)((nx / len) * 128);
      by = (unsigned char)((ny / len) * 128);
      bz = (unsigned char)((nz / len) * 128);
    }
    gradient[i * 4 + 0] = bx;
    gradient[i * 4 + 1] = by;
    gradient[i * 4 + 2] = bz;
    gradient[i * 4 + 3] = 0;
#else
    gradient[i * 4 + 0] = gradient_tmp[i * 3 + 0];
    gradient[i * 4 + 1] = gradient_tmp[i * 3 + 1];
    gradient[i * 4 + 2] = gradient_tmp[i * 3 + 2];
    gradient[i * 4 + 3] = 0;
#endif
  }
#endif
  // Init OpenCL ray tracer
  CLRayTracerShaded cl_raytracer("raytracing_slam.cl",
                                 "raytracing",
                                 &cl_context);
  cl_raytracer.Init(volume_data, tf, gradient,
                    cgl_texture_handler,
                    gl_context.width(), gl_context.height());
#else
  CLRayTracer cl_raytracer("raytracing.cl",
                           "raytracing",
                           &cl_context);
  cl_raytracer.Init(volume_data, tf,
                    cgl_texture_handler,
                    gl_context.width(), gl_context.height());
#endif

#define DEBUG_
#ifndef DEBUG
  do {
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFinish();

    gl_control.UpdateCameraPose();
    glm::mat4 T_inv  = glm::inverse(gl_control.view_mat());
    glm::mat4 P      = gl_control.projection_mat();
    cl_float3 r1     = {{T_inv[0][0], T_inv[1][0], T_inv[2][0]}};
    cl_float3 r2     = {{T_inv[0][1], T_inv[1][1], T_inv[2][1]}};
    cl_float3 r3     = {{T_inv[0][2], T_inv[1][2], T_inv[2][2]}};
    cl_float3 camera = {{T_inv[3][0], T_inv[3][1], T_inv[3][2]}};
    cl_float2 f      = {{P[0][0], P[1][1]}};
    cl_raytracer.Compute(r1, r2, r3, camera, f);
    gl_processor.Render();

    glfwSwapBuffers(gl_context.window());
    glfwPollEvents();
#ifndef DEBUG
  } while( glfwGetKey(gl_context.window(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(gl_context.window()) == 0 );
#endif
  glfwTerminate();

  return 0;
}