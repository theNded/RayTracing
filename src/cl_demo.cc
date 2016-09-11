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

#include "cl_utils/context.h"
#include "cl_utils/kernel.h"

int main(int arg, char* args[]) {
  gl_utils::Context context("tesxt");

  cl_utils::Context cl_context = cl_utils::Context();
  cl_kernel kernel = cl_utils::LoadKernel("copy.cl", "copy",
                                          cl_context.device(),
                                          cl_context.context());

  cv::Mat input_img = cv::imread("/Users/Neo/Desktop/avatar.png");
  // OpenCL only recognizes RGBA
  cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGBA);
  size_t w = (size_t)input_img.cols;
  size_t h = (size_t)input_img.rows;

  // Preparing buffer
  const cl_image_format format = {CL_RGBA, CL_UNORM_INT8};
  const cl_image_desc desc = {CL_MEM_OBJECT_IMAGE2D, w, h};

  cl_mem in = clCreateImage(cl_context.context(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            &format, &desc, input_img.data, NULL);
  cl_mem out = clCreateImage(cl_context.context(), CL_MEM_WRITE_ONLY,
                             &format, &desc, NULL, NULL);

  clSetKernelArg(kernel, 0, sizeof(cl_mem), &in);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &out);

  //start and end coordinates for reading our image
  size_t globals[2] = {(size_t)input_img.cols, (size_t)input_img.rows};
  //execute kernel
  clEnqueueNDRangeKernel(cl_context.queue(), kernel, 2, NULL,
                         globals, NULL,
                         0, NULL, NULL);

  //wait for kernel to finish
  clFinish(cl_context.queue());

  //start and end coordinates for reading our image
  size_t origin[3] = {0, 0, 0};
  size_t size[3]   = {w, h, 1};

  auto tmp = new unsigned char[input_img.cols * input_img.rows * 4];
  clEnqueueReadImage(cl_context.queue(), out, CL_TRUE,
                     origin, size,
                     0, 0, tmp, 0, NULL, NULL);

  cv::Mat outPng;
  cv::Mat(input_img.rows, input_img.cols, CV_8UC4, tmp).copyTo(outPng);
  cv::cvtColor(outPng, outPng, cv::COLOR_RGBA2BGR);

  //write the image to file
  cv::imshow("out", outPng);
  cv::waitKey(-1);

  //free the temp array
  delete[] tmp;

  return 0;
}