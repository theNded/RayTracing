//
// Created by Neo on 16/7/17.
//

#include "gl_utils/context.h"

#include <iostream>

namespace gl_utils {
Context::Context(std::string window_name, int width, int height) {
  width_ = width;
  height_ = height;
  Init(window_name);
}

int Context::Init(std::string window_name) {
  // Initialise GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW." << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window_ = glfwCreateWindow(width_, height_, window_name.c_str(), NULL, NULL);
  if (window_ == NULL) {
    std::cerr << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window_);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window_, width_ / 2, height_ / 2);

  // Initialize GLEW
  glewExperimental = GL_TRUE; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW." << std::endl;
    glfwTerminate();
    return -1;
  }

  return 0;
}

GLFWwindow *Context::window() const {
  return window_;
}

int Context::width() {
  return width_;
}

int Context::height() {
  return height_;
}
}