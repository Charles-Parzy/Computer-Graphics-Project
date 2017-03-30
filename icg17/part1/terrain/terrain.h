#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Grid {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID

    public:
        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("terrain_vshader.glsl",
                                                  "terrain_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                // Make a triangle grid with dimension 512x512.
                // always two subsequent entries in 'vertices' form a 2D vertex position.
                int grid_dim = 512;

                // the given code below are the vertices for a simple quad.
                // your grid should have the same dimension as that quad, i.e.,
                // reach from [-1, -1] to [1, 1].

                // vertex position of the triangles.
                
                float resolution = 2.0f / grid_dim;
                
                for (float i = (float)(-grid_dim / 2); i <= grid_dim / 2; i += 1.0f) {
                    for (float j = (float)(-grid_dim / 2); j <= grid_dim / 2; j += 1.0f) {
                        vertices.push_back(j * resolution); vertices.push_back(i * resolution);
                    }
                }
                
                // and indices.
                for (int i = 0; i < grid_dim; i++) {
                    for (int j = 0; j < grid_dim; j++) {
                        indices.push_back(j + (grid_dim + 1) * i);
                        indices.push_back(j + (grid_dim + 1) * i + 1);
                        indices.push_back(j + (grid_dim + 1) * (i + 1));
                        indices.push_back(j + (grid_dim + 1) * i + 1);
                        indices.push_back(j + (grid_dim + 1) * (i + 1));
                        indices.push_back(j + (grid_dim + 1) * (i + 1) + 1);
                    }
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            
            glBindVertexArray(vertex_array_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            // draw
            // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
