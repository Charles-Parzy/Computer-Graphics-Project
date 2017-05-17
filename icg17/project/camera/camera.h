#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    GLfloat* texture_data;
    bool isInFpsMode = false;

public:

    void Init(int heightmap_width, int heightmap_height, GLuint heightMap) {
        // set heightmap size
        heightmap_width_ = heightmap_width;
        heightmap_height_ = heightmap_height;
        this->heightmap_texture_id_ = heightMap;

        texture_data = new GLfloat[heightmap_width * heightmap_height];
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

        float tmpPosX = pos.x - dispX;
    	float tmpPosZ = pos.z - dispZ;

    	if (tmpPosX < 1.0 && tmpPosX > -1.0) {
            pos.x = tmpPosX;
            look.x = look.x - dispX;
        }
        if (tmpPosZ < 1.0 && tmpPosZ > -1.0) {
            pos.z = tmpPosZ;
            look.z = look.z - dispZ;
        }

        int tmpX = int((1.0+pos.x) * heightmap_width_/2.0);
        int tmpZ = int((1.0+pos.z) * heightmap_height_/2.0);
        cout << "tmpX " << tmpX << endl;
        cout << "tmpZ " << tmpZ << endl;

        if(tmpX > 0 && tmpX < heightmap_width_ && tmpZ > 0 && tmpZ < heightmap_height_) {
            int height = texture_data[tmpX+heightmap_width_*tmpZ];
            pos.y = float(height+0.06); //fps person height
            cout << "HEIGHT =" << height << endl;
            cout << "HEIGHT 0 =" << texture_data[0] << endl;
            cout << "HEIGHT 1 =" << texture_data[2400] << endl;

        }
      } else {
        float dispX = (pos.x - look.x)*speed;
		float dispY = (pos.y - look.y)*speed;
		float dispZ = (pos.z - look.z)*speed;

		pos.x = pos.x - dispX;
		pos.y = pos.y - dispY;
		pos.z = pos.z - dispZ;

		look.x = look.x - dispX;
		look.y = look.y - dispY;
		look.z = look.z - dispZ;
      }
    }

    void switchInFpsMode() {
        cout << "FPS MODE " << isInFpsMode << endl;
        isInFpsMode = !isInFpsMode;
    }
};
