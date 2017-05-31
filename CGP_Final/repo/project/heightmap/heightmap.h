#pragma once
#include "icg_helper.h"

class HeightMap {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        float H_id_ = 0.6f;
        float lacunarity_ = 2.3f;
        int octaves_ = 15;
        float offset_ = 0.7f;
        float gain_ = 2.7f;

    public:
        void Init() {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("heightmap_vshader.glsl",
                                                    "heightmap_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                                 /*V2*/ +1.0f, -1.0f, 0.0f,
                                                 /*V3*/ -1.0f, +1.0f, 0.0f,
                                                 /*V4*/ +1.0f, +1.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 1.0f, 0.0f,
                                                               /*V3*/ 0.0f, 1.0f,
                                                               /*V4*/ 1.0f, 1.0f};

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
        }

        void Draw() {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // Temporary values to allow easier changes
            glUniform1f(glGetUniformLocation(program_id_, "H"),
                        this->H_id_);
            glUniform1f(glGetUniformLocation(program_id_, "Lacunarity"),
                        this->lacunarity_);
            glUniform1i(glGetUniformLocation(program_id_, "Octaves"),
                        this->octaves_);
            glUniform1f(glGetUniformLocation(program_id_, "Offset"),
                        this->offset_);
            glUniform1f(glGetUniformLocation(program_id_, "Gain"),
                        this->gain_);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }

        void setH(float increment) {
            H_id_ += increment;
            cout << "Changing H" << H_id_ << endl;
        }

        void setLacunarity(float increment) {
            lacunarity_ += increment;
            cout << "Changing Lacunarity" << lacunarity_ << endl;
        }

        void setOctaves(int increment) {
            octaves_ += increment;
            cout << "Changing Octaves" << octaves_ << endl;
        }

        void setOffset(float increment) {
            offset_ += increment;
            cout << "Changing Offset" << offset_ << endl;
        }

        void setGain(float increment) {
            gain_ += increment;
            cout << "Changing Gain" << gain_ << endl;
        }
};
