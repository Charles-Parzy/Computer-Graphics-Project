#pragma once
#include "icg_helper.h"

class HeightMap {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer

        GLuint grad_text_id_;
        GLuint perm_text_id_;

        float heightmap_width_;
        float heightmap_height_;

    public:
        void Init(float heightmap_width, float heightmap_height) {

            // set heightmap size
            this->heightmap_width_ = heightmap_width;
            this->heightmap_height_ = heightmap_height;

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

            this-> perm_text_id_ = gen_permutation_table();
            this-> grad_text_id_ = gen_gradient_vectors();

            // load/Assign heightmap texture
            glBindTexture(GL_TEXTURE_1D, perm_text_id_);
            glActiveTexture(GL_TEXTURE0);
            GLuint perm_id = glGetUniformLocation(program_id_, "permTex1D");
            glUniform1i(perm_id, 0 /*GL_TEXTURE2*/);
            glBindTexture(GL_TEXTURE_1D, GL_TEXTURE0);

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
            //glDeleteTexture
        }

        void UpdateSize(int heightmap_width, int heightmap_height) {
            this->heightmap_width_ = heightmap_width;
            this->heightmap_height_ = heightmap_height;
        }

        void Draw() {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                        this->heightmap_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                        this->heightmap_height_);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }

        GLuint gen_permutation_table() {
            /// Pseudo-randomly generate the permutation table.
            const int size(256);
            GLfloat permutationTable[size];
            for(int k=0; k<size; ++k)
                permutationTable[k] = k;

            /// Seed the pseudo-random generator for reproductability.
            std::srand(10);

            /// Fisher-Yates / Knuth shuffle.
            for(int k=size-1; k>0; --k) {
                GLuint idx = int(float(k) * std::rand() / RAND_MAX);
                GLfloat tmp = permutationTable[k];
                permutationTable[k] = permutationTable[idx];
                permutationTable[idx] = tmp;
            }

            /// Create the texture.
            GLuint permTableTexID;
            glGenTextures(1, &permTableTexID);
            glBindTexture(GL_TEXTURE_1D, permTableTexID);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, size, 0, GL_RED, GL_FLOAT, permutationTable);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            
            return permTableTexID;
        }

        GLuint gen_gradient_vectors() {

            /// Gradients for 2D noise.
            /*const GLfloat gradients[] = {
                1.0f,  1.0f,
               -1.0f,  1.0f,
                1.0f, -1.0f,
               -1.0f, -1.0f,
                0.0f,  1.0f,
                0.0f, -1.0f,
                1.0f,  0.0f,
               -1.0f,  0.0f,
            };*/

            const GLfloat gradients[] = {
                1.0f,  1.0f,
               -1.0f,  1.0f,
                1.0f, -1.0f,
               -1.0f, -1.0f,
                0.0f,  1.0f,
                0.0f, -1.0f,
                1.0f,  0.0f,
               -1.0f,  0.0f,
            };

            const int nGradients = (sizeof(gradients) / sizeof(GLfloat)) / 2;

            /// Create the texture.
            GLuint gradVectTexID;
            glGenTextures(1, &gradVectTexID);
            glBindTexture(GL_TEXTURE_1D, gradVectTexID);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 8, 0, GL_RED, GL_FLOAT, gradients);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            return gradVectTexID;
        }
};
