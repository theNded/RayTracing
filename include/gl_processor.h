//
// Created by Neo on 16/8/9.
// Construct: shader -> vertex shader
//                   -> fragment shader
//                   -> uniform variable name
// Init:       input -> texture
//            output -> (frame)
//

#ifndef RAYTRACING_GL_OBJECT_H
#define RAYTRACING_GL_OBJECT_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl_utils/shader.h"
#include "gl_utils/context.h"

class GLProcessor {
public:
  // Construct format:
  // @vertex shader
  // @fragment shader
  // @uniforms
  // @context
  GLProcessor(std::string vertex_shader_path,
              std::string fragment_shader_path,
              std::string sampler_name,
              gl_utils::Context* context);
  ~GLProcessor();

  // Init format:
  // @input data (here empty; vertices are saved as static member)
  // @output data
  // @additional params
  void Init(GLuint& texture);

  // Render format:
  // @parameters (typically camera poses)
  void Render();

private:
  static const GLfloat kVertices[];
  static const GLubyte kIndices[];

  gl_utils::Context* context_;

  GLuint* texture_ptr_;
  GLuint vao_;
  GLuint vbo_[2];
  GLuint program_;
  GLint  sampler_;
};


#endif //RAYTRACING_GL_OBJECT_H
