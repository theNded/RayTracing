//
// Created by Neo on 16/8/9.
//

#ifndef RAYTRACING_GL_OBJECT_H
#define RAYTRACING_GL_OBJECT_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl_utils/shader.h"

class GLProcessor {
public:
  GLProcessor(std::string vertex_shader_path,
              std::string fragment_shader_path,
              std::string sampler_name);
  ~GLProcessor();
  void Init(GLFWwindow *window, int width, int height);
  void Render();

  GLuint & texture();

private:
  static const GLfloat kVertices[];
  static const GLubyte kIndices[];

  GLuint vao_;
  GLuint vbo_[2];
  GLuint program_;
  GLuint texture_;
  GLint  sampler_;
};


#endif //RAYTRACING_GL_OBJECT_H
