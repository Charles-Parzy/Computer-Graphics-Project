#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct LighthouseLight {
        glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 2.0f);

        // pass light properties to the shader
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            // given in camera space
            GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

            GLuint La_id = glGetUniformLocation(program_id, "La");
            GLuint Ld_id = glGetUniformLocation(program_id, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id, "Ls");

            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
        }
};

struct LighthouseMaterial {
        glm::vec3 ka = glm::vec3(0.18f, 0.1f, 0.1f);
        glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
        glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
        float alpha = 60.0f;

        // pass material properties to the shaders
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            GLuint ka_id = glGetUniformLocation(program_id, "ka");
            GLuint kd_id = glGetUniformLocation(program_id, "kd");
            GLuint ks_id = glGetUniformLocation(program_id, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);
        }
};

class Lighthouse : public LighthouseMaterial, public LighthouseLight {

    private:
        GLuint vertex_array_id_;        // vertex array object

        GLuint active_program_id_;

        // tinyobjloader shapes (contains points, indices, etc)
        std::vector<tinyobj::shape_t> shapes_;

        GLuint vertex_buffer_object_;           // memory buffer
        GLuint vertex_normal_buffer_object_;    // memory buffer
        GLuint texture_id_;                  // texture ID

    private:
        void BindShader(GLuint program_id) {
            glUseProgram(program_id);
            glBindVertexArray(vertex_array_id_);

            // vertex attribute id for positions
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glEnableVertexAttribArray(vertex_point_id);

                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glVertexAttribPointer(vertex_point_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // vertex attribute id for normals
            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glEnableVertexAttribArray(vertex_normal_id);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
                glVertexAttribPointer(vertex_normal_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // textures
            GLuint tex_id = glGetUniformLocation(program_id, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        }

        void UnbindShader(GLuint program_id) {
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glDisableVertexAttribArray(vertex_point_id);
            }

            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glDisableVertexAttribArray(vertex_normal_id);
            }

            glUseProgram(0);
            glBindVertexArray(0);
        }

    public:
        void Init(const string& filename) {

            // compile the shaders
            active_program_id_ = icg_helper::LoadShaders("lighthouse_vshader.glsl",
                                                          "lighthouse_fshader.glsl");
            if(!active_program_id_) {
                exit(EXIT_FAILURE);
            }

            string error;
            // obj files can contains material informations
            vector<tinyobj::material_t> materials;

            bool objLoadReturn = tinyobj::LoadObj(shapes_, materials, error,
                                                  filename.c_str());

            if(!error.empty()) {
                cerr << error << endl;
            }

            if(!objLoadReturn) {
                exit(EXIT_FAILURE);
            }

            // only load the first shape from the obj file
            // (see tinyobjloader for multiple shapes inside one .obj file)
            int number_of_vertices = shapes_[0].mesh.positions.size();
            int number_of_indices = shapes_[0].mesh.indices.size();
            int number_of_normals = shapes_[0].mesh.normals.size();
            printf("Loaded mesh '%s' (#V=%d, #I=%d, #N=%d)\n", filename.c_str(),
                   number_of_vertices, number_of_indices, number_of_normals);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex buffer
            glGenBuffers(ONE, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, number_of_vertices * sizeof(float),
                         &shapes_[0].mesh.positions[0], GL_STATIC_DRAW);

            // normal buffer
            glGenBuffers(ONE, &vertex_normal_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, number_of_normals * sizeof(float),
                         &shapes_[0].mesh.normals[0], GL_STATIC_DRAW);

            // index buffer
            GLuint vertex_buffer_object_indices;
            glGenBuffers(ONE, &vertex_buffer_object_indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         number_of_indices * sizeof(unsigned int),
                         &shapes_[0].mesh.indices[0], GL_STATIC_DRAW);

            // 2D texture for shader
            glGenTextures(1, &texture_id_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // unbind
            glBindVertexArray(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteBuffers(1, &vertex_normal_buffer_object_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

            float scale = 1.0;
            glm::mat4 M = model;
            M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.5f));
            M = glm::scale(M, glm::vec3(scale));

            BindShader(active_program_id_);

            LighthouseMaterial::Setup(active_program_id_);
            LighthouseLight::Setup(active_program_id_);

            // set up spot light if needed
            GLint spot_dir_id = glGetUniformLocation(active_program_id_, "spot_dir");
            if (spot_dir_id >=0) {
                glm::vec3 spot_dir = light_pos;
                glUniform3fv(spot_dir_id, ONE, glm::value_ptr(spot_dir));
            }
            // setup matrix stack
            GLint model_id = glGetUniformLocation(active_program_id_,
                                                  "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(M));
            GLint view_id = glGetUniformLocation(active_program_id_,
                                                 "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(active_program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));
            glDrawElements(GL_TRIANGLES, /*#vertices*/ shapes_[0].mesh.indices.size(),
                    GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            UnbindShader(active_program_id_);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};