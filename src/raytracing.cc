//
// Created by Neo on 16/8/8.
//

#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <opencv2/opencv.hpp>

#include "gl_utils/context.h"
#include "gl_utils/control.h"
#include "gl_utils/model.h"
#include "gl_utils/shader.h"
#include "gl_utils/texture.h"

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "gl_processor.h"
#include "cl_processor.h"

#include <vector>
#include "transfer_function.h"
// ./raytracing filename width height depth sizeof_unit
int main(int argc, char* args[]) {

#define ManualAdjustTransferFunction
#ifdef ManualAdjustTransferFunction
  typedef TransferFunction::ControlPoint ControlPoint;

  std::vector<ControlPoint> c;
  c.push_back(ControlPoint(.02f, .7f, .61f, 0));
  c.push_back(ControlPoint(.35f, .7f, .61f, 80));
  c.push_back(ControlPoint(.75f, 1.0f, .85f, 182));
  c.push_back(ControlPoint(1.0f, 1.0f, .85f, 256));

  std::vector<ControlPoint> a;
  a.push_back(ControlPoint(0.0f, 0));
  a.push_back(ControlPoint(0.0f, 110));
  //a.push_back(ControlPoint(0.1f, 92));
  a.push_back(ControlPoint(0.4f, 145));
  a.push_back(ControlPoint(0.7f, 192));
  a.push_back(ControlPoint(1.0f, 256));
  TransferFunction tf = TransferFunction(c, a);
#else
  TransferFunction tf = TransferFunction
      ("/Users/Neo/code/Data/transfer_function.txt");
#endif
  VolumeData volume_data = VolumeData("/Users/Neo/code/Data/Bonsai.raw",
                                      512, 512, 189, 2);
  /*
  else
    volume_data = VolumeData(args[1],
                                 (size_t)(args[2]), (size_t)(args[3]),
                                 (size_t)(args[3]), (size_t)(args[4]));*/


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

#define LOOP
#ifdef LOOP
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
#ifdef LOOP
  } while( glfwGetKey(gl_context.window(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(gl_context.window()) == 0 );
#endif
  glfwTerminate();

  return 0;
}