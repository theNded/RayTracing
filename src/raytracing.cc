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

#include <opencv2/opencv.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "gl_utils/context.h"
#include "gl_utils/control.h"
#include "gl_utils/model.h"
#include "gl_utils/shader.h"
#include "gl_utils/texture.h"

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "gl_processor.h"
#include "cl_processor.h"

std::string kDefaultConfigPath = "/Users/Neo/code/Data/hazelnuts/config.json";

int main(int argc, char* args[]) {
  FILE* config_fp = fopen((argc > 1) ? args[1] : kDefaultConfigPath.c_str(), "r");
  char readBuffer[4096];
  rapidjson::FileReadStream is(config_fp, readBuffer, sizeof(readBuffer));
  rapidjson::Document config;
  config.ParseStream(is);
  fclose(config_fp);

  std::string volume_file_path = config["volume data path"].GetString();
  std::string tf_file_path     = config["transfer function path"].GetString();
  int dims[] = {
      config["dims"]["x"].GetInt(),
      config["dims"]["y"].GetInt(),
      config["dims"]["z"].GetInt()
  };
  float scales[] = {
      config["scales"]["x"].GetFloat(),
      config["scales"]["y"].GetFloat(),
      config["scales"]["z"].GetFloat()
  };
  int unit_size = config["unit size"].GetInt();

#define ManualAdjustTransferFunction_
#ifdef ManualAdjustTransferFunction
  typedef TransferFunction::ControlPoint ControlPoint;
  std::vector<ControlPoint> c;

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
  VolumeData volume_data = VolumeData(volume_file_path,
                                      dims[0], dims[1], dims[2],
                                      scales[0], scales[1], scales[2],
                                      unit_size);

  // Init OpenGL. This step must be ahead of OpenCL
  gl_utils::Context gl_context("CL x GL demo", 512, 512);
  gl_utils::Control gl_control(gl_context.window(),
                               gl_context.width(),
                               gl_context.height());
  GLProcessor gl_processor("cl_gl_vertex.glsl",
                           "cl_gl_fragment.glsl",
                           "texture_sampler");
  gl_processor.Init(gl_context.window(),
                    gl_context.width(),
                    gl_context.height());

  // Init OpenCL
  cl_utils::Context cl_context = cl_utils::Context();
  CLProcessor cl_processor("raytracing.cl",
                           "raytracing",
                           &cl_context);
  cl_processor.Init(volume_data, tf, gl_processor.texture());

#define DEBUG_
#ifndef DEBUG
  do {
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFinish();

    gl_control.UpdateCameraPose();
    glm::mat4 T_inv  = glm::inverse(gl_control.view_mat());
    glm::mat4 P      = gl_control.projection_mat();
    cl_float3 r1     = {T_inv[0][0], T_inv[1][0], T_inv[2][0]};
    cl_float3 r2     = {T_inv[0][1], T_inv[1][1], T_inv[2][1]};
    cl_float3 r3     = {T_inv[0][2], T_inv[1][2], T_inv[2][2]};
    cl_float3 camera = {T_inv[3][0], T_inv[3][1], T_inv[3][2]};
    cl_float2 f      = {P[0][0], P[1][1]};
    cl_processor.Compute(r1, r2, r3, camera, f);
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