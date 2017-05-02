#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const float maxSize = 5.0f;
static const unsigned int NbCubeVertices = 36;
static const glm::vec3 CubeVertices[] =
{
    glm::vec3(-maxSize, -maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, -maxSize),
    glm::vec3(maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, maxSize, maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, -maxSize),
    glm::vec3(maxSize, maxSize, maxSize),
    glm::vec3(-maxSize, maxSize, maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(-maxSize, maxSize, maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, -maxSize),
    glm::vec3(maxSize, -maxSize, maxSize),
    glm::vec3(maxSize, -maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, -maxSize),
    glm::vec3(-maxSize, -maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, maxSize),
    glm::vec3(-maxSize, -maxSize, maxSize),
    glm::vec3(maxSize, maxSize, -maxSize),
    glm::vec3(-maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, maxSize, maxSize),
    glm::vec3(-maxSize, maxSize, -maxSize),
    glm::vec3(maxSize, maxSize, maxSize),
    glm::vec3(-maxSize, maxSize, maxSize)
};

static const unsigned int NbCubeUVs = 36;

static const glm::vec2 CubeUVs[] =
{
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.333, 0.0),

    glm::vec2(0.0, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 1.0),

    glm::vec2(0.666, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(1.0, 0.75)
};


class Skybox {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;
        GLboolean isReflection;

    public:
        void Init(GLboolean isReflection = false) {

            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                                  "skybox_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeVertices * sizeof(glm::vec3),
                             &CubeVertices[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeUVs * sizeof(glm::vec2),
                             &CubeUVs[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            this->isReflection = isReflection;

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string texture_filename = "../../textures/skybox.tga";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(texture_filename.c_str(),
                                                 &width, &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(std::string("Failed to load texture"));
                }

                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX){
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            glUniform1i(glGetUniformLocation(program_id_, "isReflection"), this->isReflection);

            GLint model_id = glGetUniformLocation(program_id_, "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));

            GLint view_id = glGetUniformLocation(program_id_, "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(program_id_, "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

            // draw
            glDrawArrays(GL_TRIANGLES,0, NbCubeVertices);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
