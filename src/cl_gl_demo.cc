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

#define AAA
int main(int arg, char* args[]) {
  gl_utils::Context context("tesxt");
  GLuint program;
  gl_utils::LoadShaders("cl_gl_vertex.glsl", "cl_gl_fragment.glsl", program);
  GLint sampler = glGetUniformLocation(program, "textureSampler");

#ifdef AAA
  cl_utils::Context cl_context = cl_utils::Context();
  cl_kernel kernel = cl_utils::LoadKernel("kernel.cl", "copy",
                                          cl_context.device(),
                                          cl_context.context());
#endif

  cv::Mat input_img = cv::imread("/Users/Neo/Desktop/avatar.png");
  // OpenCL only recognizes RGBA
  cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGBA);
  size_t w = (size_t)input_img.cols;
  size_t h = (size_t)input_img.rows;

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
               w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

#ifdef AAA
  const cl_image_format format = {CL_RGBA, CL_UNORM_INT8};
  const cl_image_desc desc = {CL_MEM_OBJECT_IMAGE2D, w, h};
  cl_mem in = clCreateImage(cl_context.context(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            &format, &desc, input_img.data, NULL);
  cl_mem out = clCreateFromGLTexture(cl_context.context(), CL_MEM_WRITE_ONLY,
                                    GL_TEXTURE_2D, 0, texture, NULL);
#endif

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo[2];
  glGenBuffers(2, vbo);

  std::vector<glm::vec2> vertices;
  vertices.push_back(glm::vec2(-1, -1));
  vertices.push_back(glm::vec2(-1,  1));
  vertices.push_back(glm::vec2(1,   1));
  vertices.push_back(glm::vec2(1,  -1));

  std::vector<unsigned int> indices;
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(0);
  indices.push_back(2);
  indices.push_back(3);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(glm::vec2),
               vertices.data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               indices.size() * sizeof(unsigned int),
               indices.data(),
               GL_STATIC_DRAW);

  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);

  do {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef AAA
    cl_int status;
    glFinish();
    status = clEnqueueAcquireGLObjects(cl_context.queue(), 1,  &out, 0, 0,
                                       NULL);
    std::cout << "Acquire: " << status << std::endl;

    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out);

    size_t globals[2] = {(size_t)input_img.cols, (size_t)input_img.rows};
    status = clEnqueueNDRangeKernel(cl_context.queue(), kernel, 2, NULL,
                           globals, NULL,
                           0, NULL, NULL);
    std::cout << "NDRange: " << status << std::endl;

    clFinish(cl_context.queue());
    status = clEnqueueReleaseGLObjects(cl_context.queue(), 1,  &out, 0, 0,
                                       NULL);
    std::cout << "Release: " << status << std::endl;

#endif
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(sampler, 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(context.window());
    glfwPollEvents();

  } while( glfwGetKey(context.window(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(context.window()) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}