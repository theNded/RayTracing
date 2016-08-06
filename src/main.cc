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
  cl_utils::Context cl_context;
  cl::Kernel kernel = cl_utils::LoadKernel("kernel.cl", "copy",
                                           cl_context.device(),
                                           cl_context.context());

  gl_utils::Context gl_context("CL x GL");
  gl_utils::Control control(gl_context.window());

  cv::Mat input_img = cv::imread("/Users/Neo/Desktop/avatar.png");
  // OpenCL only recognizes RGBA
  cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGBA);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
               input_img.cols, input_img.rows, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, 0);

  const cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);
  cl::Image2D in(cl_context.context(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 format,
                 input_img.cols, input_img.rows, 0, input_img.data);
  cl::Image2D out(cl_context.context(), CL_MEM_WRITE_ONLY,
                  format,
                  input_img.cols, input_img.rows, 0, NULL);
  kernel.setArg(0, in);
  kernel.setArg(1, out);

  //execute kernel
  cl_context.queue().enqueueNDRangeKernel(kernel,
                                       cl::NullRange,
                                       cl::NDRange(input_img.cols, input_img.rows),
                                       cl::NullRange);
  //wait for kernel to finish
  cl_context.queue().finish();

  //start and end coordinates for reading our image
  cl::size_t<3> origin;
  cl::size_t<3> size;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  size[0] = input_img.cols;
  size[1] = input_img.rows;
  size[2] = 1;

  //output png

  //temporary array to store the result from opencl
  auto tmp = new unsigned char[input_img.cols * input_img.rows * 4];
  //CL_TRUE means that it waits for the entire image to be copied before continuing
  cl_context.queue().enqueueReadImage(out, CL_TRUE, origin, size, 0, 0, tmp);

  cv::Mat outPng;
  cv::Mat(input_img.rows, input_img.cols, CV_8UC4, tmp).copyTo(outPng);
  cv::cvtColor(outPng, outPng, cv::COLOR_RGBA2BGR);

  //copy the data from the temp array to the png
  //std::copy(&tmp[0], &tmp[w * h * 3], outPng.data);

  //write the image to file
  cv::imshow("out", outPng);
  cv::waitKey(-1);

  //free the temp array
  delete[] tmp;

  return 0;
}