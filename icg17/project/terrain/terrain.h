#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

struct Light {
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

class Terrain : public Light {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint num_indices_;                    // number of vertices to render

        //Textures
        GLuint heightmap_texture_id_;           // Heightmap texture
        GLuint grass_texture_id_;   
        GLuint rock_texture_id_;   
        GLuint snow_texture_id_;   
        GLuint seabed_texture_id_;   
        GLuint sand_texture_id_;
        GLuint water_texture_id_;
        GLuint reflection_texture_id_;

        //Water drawing
        GLboolean isWater = false;
        GLboolean isReflection = false;
        float heightmap_width_;
        float heightmap_height_;

        void BindShader(float time, GLuint program_id,
                        const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

            Light::Setup(program_id_);

            // setup matrix stack
            GLint model_id = glGetUniformLocation(program_id_,
                                                  "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            GLint view_id = glGetUniformLocation(program_id_,
                                                 "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));

            glUniform1i(glGetUniformLocation(program_id_, "isWater"),
                        this->isWater);
            glUniform1i(glGetUniformLocation(program_id_, "isReflection"),
                        this->isReflection);
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);
        }

        void loadTexture(const string file, GLuint* texture_id, const char* shaderTextureName, GLuint gl_texture_id) {
            int width;
            int height;
            int nb_component;
            string filename = "../../textures/"+file;
            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* image = stbi_load(filename.c_str(), &width,
                                             &height, &nb_component, 0);

            if(image == nullptr) {
                throw(string("Failed to load texture"));
            }

            glGenTextures(1, texture_id);
            glBindTexture(GL_TEXTURE_2D, *texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, image);
            }

            GLuint tex_id = glGetUniformLocation(program_id_, shaderTextureName);
            glUniform1i(tex_id, GLuint(gl_texture_id - GL_TEXTURE0));
            // cleanup
            glBindTexture(GL_TEXTURE_2D, gl_texture_id);
            stbi_image_free(image);
        }

        void activateTexture(GLuint texture_id, GLuint gl_texture_id) {
            glActiveTexture(gl_texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }

    public:
        void Init(float heightmap_width, float heightmap_height, GLuint heightMap, GLboolean isWater, GLboolean isReflection, GLuint reflection) {
            // set heightmap size
            this->heightmap_width_ = heightmap_width;
            this->heightmap_height_ = heightmap_height;

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
                // TODO 5: make a triangle grid with dimension 100x100.
                // always two subsequent entries in 'vertices' form a 2D vertex position.
                int grid_dim = 1200;

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

            this->isWater = isWater;
            this->isReflection = isReflection;

            // load/Assign heightmap texture
            this->heightmap_texture_id_ = heightMap;
            glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
            GLuint heightmap_id = glGetUniformLocation(program_id_, "heightMap");
            glUniform1i(heightmap_id, 0 /*GL_TEXTURE0*/);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);

            // REFLECTION CODE
            this->reflection_texture_id_ = reflection;
            glBindTexture(GL_TEXTURE_2D, reflection_texture_id_);
            GLuint reflection_id = glGetUniformLocation(program_id_, "reflection");
            glUniform1i(reflection_id, 7 /*GL_TEXTURE7*/);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE7);

            
            // Load/assign texures
            loadTexture("grass.tga", &grass_texture_id_, "GrassTex2D", GL_TEXTURE1);
            loadTexture("rock.tga", &rock_texture_id_, "RockTex2D", GL_TEXTURE2);
            loadTexture("seabed.tga", &seabed_texture_id_, "SeabedTex2D", GL_TEXTURE3);
            loadTexture("sand.tga", &sand_texture_id_, "SandTex2D", GL_TEXTURE4);
            loadTexture("snow.tga", &snow_texture_id_, "SnowTex2D", GL_TEXTURE5);
            loadTexture("water.tga", &water_texture_id_, "WaterTex2D", GL_TEXTURE6);

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
            glDeleteTextures(1, &heightmap_texture_id_);
            glDeleteTextures(1, &grass_texture_id_);
            glDeleteTextures(1, &rock_texture_id_);
            glDeleteTextures(1, &snow_texture_id_);
            glDeleteTextures(1, &seabed_texture_id_);
            glDeleteTextures(1, &sand_texture_id_);
            glDeleteTextures(1, &water_texture_id_);
            glDeleteTextures(1, &reflection_texture_id_);
        }

        void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            //Setup up for shading
            BindShader(time, program_id_, model, view, projection);

            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_, "heightmap_width"),
                        this->heightmap_width_);
            glUniform1f(glGetUniformLocation(program_id_, "heightmap_height"),
                        this->heightmap_height_);

            activateTexture(heightmap_texture_id_, GL_TEXTURE0);
            activateTexture(grass_texture_id_, GL_TEXTURE1);
            activateTexture(rock_texture_id_, GL_TEXTURE2);
            activateTexture(seabed_texture_id_, GL_TEXTURE3);
            activateTexture(sand_texture_id_, GL_TEXTURE4);
            activateTexture(snow_texture_id_, GL_TEXTURE5);
            activateTexture(water_texture_id_, GL_TEXTURE6);
            activateTexture(reflection_texture_id_, GL_TEXTURE7);

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glDisable(GL_BLEND);
            glBindVertexArray(0);
            glUseProgram(0);
        }
};
