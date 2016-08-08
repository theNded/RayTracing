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

int main(int arg, char* args[]) {
  // Init OpenGL
  gl_utils::Context context("CL x GL demo");
  gl_utils::Control control(context.window());
  GLuint program;
  gl_utils::LoadShaders("cl_gl_vertex.glsl", "cl_gl_fragment.glsl", program);
  GLint sampler = glGetUniformLocation(program, "textureSampler");

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLfloat vertices[] = {
      -1, -1,   -1,  1,
      1,  1,    1, -1
  };
  GLubyte indices[] = {
      0, 1, 2,
      0, 2, 3
  };

  GLuint vbo[2];
  glGenBuffers(2, vbo);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Init OpenCL
  cl_utils::Context cl_context = cl_utils::Context();
  cl_kernel kernel = cl_utils::LoadKernel("raytracing.cl", "raytracing",
                                          cl_context.device(),
                                          cl_context.context());
  size_t w = 512;
  size_t h = 512;
  size_t d = 512;
  size_t size = w * h * d;

  // Prepare input data
  FILE *fp = fopen("/Users/Neo/code/VolumeData/hnut512_uint.raw", "rb");
  unsigned char *volume_data = new unsigned char [size];
  fread(volume_data, sizeof(unsigned char), size, fp);
  fclose(fp);
  for (int i = 0; i < size; ++i) {
      volume_data[i] *= 127;
  }
  const cl_image_format format = {CL_INTENSITY, CL_UNORM_INT8};
  const cl_image_desc desc = {CL_MEM_OBJECT_IMAGE3D, w, h, d};

  cl_mem volume = clCreateImage(cl_context.context(),
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                &format, &desc,
                                volume_data, NULL);  // Prepare output data
  //delete [] volume_data;

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  cl_mem out = clCreateFromGLTexture(cl_context.context(), CL_MEM_WRITE_ONLY,
                                     GL_TEXTURE_2D, 0, texture, NULL);

#define LOOP
#ifdef LOOP
  // Main loop
  const size_t globals[2] = {w, h};
  int t = 0, sign = 1;
  do {
    t += sign;
    if ((sign == 1 && t == 511) || (sign == -1 && t == 1))
      sign *= -1;
    std::cout << t << std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFinish();
    // OpenCL computation
    clEnqueueAcquireGLObjects(cl_context.queue(), 1,  &out, 0, 0, NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &volume);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &out);
    clSetKernelArg(kernel, 2, sizeof(int),    &t);

    clEnqueueNDRangeKernel(cl_context.queue(), kernel, 2, NULL,
                           globals, NULL,
                           0, NULL, NULL);

    clFinish(cl_context.queue());
    clEnqueueReleaseGLObjects(cl_context.queue(), 1,  &out, 0, 0, NULL);

    // OpenGL rendering
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(sampler, 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    glfwSwapBuffers(context.window());
    glfwPollEvents();

  } while( glfwGetKey(context.window(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(context.window()) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
#endif
  return 0;
}