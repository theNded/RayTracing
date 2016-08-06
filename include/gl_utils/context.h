//
// Created by Neo on 16/7/17.
// GLFW and GLEW initialization
// Provides @window() as an interaction interface
//

#ifndef RAYTRACING_GLUTILS_CONTEXT_H
#define RAYTRACING_GLUTILS_CONTEXT_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gl_utils {
class Context {
public:
  Context(std::string window_name);

  GLFWwindow *window() const;

private:
  int Init(std::string window_name);

  GLFWwindow *window_;
};
}


#endif //RAYTRACING_GLUTILS_CONTEXT_H
