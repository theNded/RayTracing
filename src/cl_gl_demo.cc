//
// Created by Neo on 16/7/29.
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
  gl_utils::Context context("CL x GL demo", 512, 512);
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
  cl_kernel kernel = cl_utils::LoadKernel("copy.cl", "copy",
                                          cl_context.device(),
                                          cl_context.context());

  // Prepare input data
  cv::Mat input_img = cv::imread("/Users/Neo/Desktop/avatar.png");
  // OpenCL only recognizes RGBA
  cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGBA);
  size_t w = (size_t)input_img.cols;
  size_t h = (size_t)input_img.rows;

  const cl_image_format format = {CL_RGBA, CL_UNORM_INT8};
  const cl_image_desc desc = {CL_MEM_OBJECT_IMAGE2D, w, h};
  cl_mem in = clCreateImage(cl_context.context(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            &format, &desc, input_img.data, NULL);

  // Prepare output data
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

  // Main loop
  const size_t globals[2] = {(size_t)input_img.cols, (size_t)input_img.rows};
  do {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFinish();
    // OpenCL computation
    clEnqueueAcquireGLObjects(cl_context.queue(), 1,  &out, 0, 0, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &in);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &out);

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

  return 0;
}