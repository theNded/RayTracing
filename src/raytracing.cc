//
// Created by Neo on 16/8/8.
//

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

#include "gl_utils/context.h"
#include "gl_utils/control.h"
#include "gl_utils/model.h"
#include "gl_utils/shader.h"
#include "gl_utils/texture.h"

#include "cl_utils/cl.hpp"
#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

#include "gl_processor.h"
#include "cl_processor.h"

int main(int arg, char* args[]) {
  size_t w = 512;
  size_t h = 512;
  size_t d = 512;
  size_t size = w * h * d;

  // Prepare input data
  FILE *fp = fopen("/Users/Neo/code/VolumeData/hnut512_uint.raw", "rb");
  unsigned char *volume_data = new unsigned char [size];
  fread(volume_data, sizeof(unsigned char), size, fp);
  fclose(fp);

  // Init OpenGL
  gl_utils::Context gl_context("CL x GL demo");
  gl_utils::Control gl_control(gl_context.window());
  GLProcessor gl_processor("cl_gl_vertex.glsl", "cl_gl_fragment.glsl",
                           "texture_sampler");
  gl_processor.Init();

  cl_utils::Context cl_context = cl_utils::Context();
  CLProcessor cl_processor("raytracing.cl", "raytracing", &cl_context);
  cl_processor.Init(volume_data, gl_processor.texture());


  delete [] volume_data;

  // Main loop
  int t = 0, sign = 1;
  do {
    t += sign;
    if ((sign == 1 && t == 511) || (sign == -1 && t == 1))
      sign *= -1;
    std::cout << t << std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFinish();
    // OpenCL computation
    cl_processor.Compute(t);
    gl_processor.Render();

    glfwSwapBuffers(gl_context.window());
    glfwPollEvents();

  } while( glfwGetKey(gl_context.window(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(gl_context.window()) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}