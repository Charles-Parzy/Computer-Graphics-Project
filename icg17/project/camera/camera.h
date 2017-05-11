#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    float* texture_data;
    bool isInFpsMode = true;

public:

    void Init(int heightmap_width, int heightmap_height, GLuint heightMap) {
        // set heightmap size
        heightmap_width_ = heightmap_width;
        heightmap_height_ = heightmap_height;
        this->heightmap_texture_id_ = heightMap;

        texture_data = new float[heightmap_width * heightmap_height];
        glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
        glGetTexImage(GL_TEXTURE_2D, heightmap_texture_id_, GL_RED, GL_FLOAT, texture_data);
    }

    void Cleanup() {
        delete[] texture_data;
    }

    void rotate2D(float pos1, float pos2, float& look1, float& look2, float angle) {
    	float tmp1 = (look1-pos1)*cos(angle) - (look2-pos2)*sin(angle) + pos1;
    	float tmp2 = (look1-pos1)*sin(angle) + (look2-pos2)*cos(angle) + pos2;
    	look1=tmp1;
    	look2=tmp2;
    }

    void rotateLeftRight(vec3 &look, vec3 pos, float angle) {
    		//1 move to the origine
    		float tmpX = look.x - pos.x;
    		float tmpY = look.y - pos.y;
    		float tmpZ = look.z - pos.z;

    		//3 rotate on Y axis
    		rotate2D(0,0,tmpX,tmpZ,angle);

    		look.x = tmpX + pos.x;
    		look.y = tmpY + pos.y;
    		look.z = tmpZ + pos.z;
    }

    void rotateUpDown(vec3 &look, vec3 pos, float angle) {
    		//1 move to the origine
            float tmpX = look.x - pos.x;
    		float tmpY = look.y - pos.y;
    		float tmpZ = look.z - pos.z;

    		//3 rotate on Z axis
    		rotate2D(0,0,tmpX,tmpY, angle);

            look.x = tmpX + pos.x;
    		look.y = tmpY + pos.y;
    		look.z = tmpZ + pos.z;
    }

    void moveFrontBack(vec3 &look, vec3 &pos, float speed) {
      if(isInFpsMode) {
            float dispX = (pos.x - look.x)*speed;
            float dispZ = (pos.z - look.z)*speed;

            int tmpX = (int)((pos.x + 1.0) * heightmap_width_);
            int tmpZ = (int)((pos.z + 1.0) * heightmap_height_);
            float height = texture_data[tmpX+heightmap_width_*tmpZ];

            int sign = (tmpX - height) / abs(tmpX - height);
            pos.x = pos.x - dispX;
            pos.z = pos.z - dispZ;
            pos.y = height;
      } else {
    		float dispX = (pos.x - look.x)*speed;
    		float dispY = (pos.y - look.y)*speed;
    		float dispZ = (pos.z - look.z)*speed;

    		pos.x = pos.x - dispX;
    		pos.y = pos.y - dispY;
    		pos.z = pos.z - dispZ;
      }
    }

    void switchInFpsMode() {
        cout << "FPS MODE " << isInFpsMode << endl;
        isInFpsMode = !isInFpsMode;
    }
};
