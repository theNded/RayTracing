//
// Created by Neo on 16/8/9.
//

#include "gl_processor.h"

const GLfloat GLProcessor::kVertices[] = {
    -1.0f, -1.0f,     -1.0f,  1.0f,
    1.0f,  1.0f,      1.0f, -1.0f
};

const GLubyte GLProcessor::kIndices[] = {
    0, 1, 2,
    0, 2, 3
};

GLProcessor::GLProcessor(std::string vertex_shader_file,
                         std::string fragment_shader_file,
                         std::string sampler_name) {
  gl_utils::LoadShaders(vertex_shader_file,
                        fragment_shader_file,
                        program_);
  sampler_ = glGetUniformLocation(program_, sampler_name.c_str());
}

void GLProcessor::Init(GLFWwindow *window) {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(2, vbo_);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(kVertices), kVertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(kIndices), kIndices,
               GL_STATIC_DRAW);

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GLProcessor::Render() {
  glUseProgram(program_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glUniform1i(sampler_, 0);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

GLuint & GLProcessor::texture() {
  return texture_;
}