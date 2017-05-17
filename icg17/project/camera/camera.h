#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    float* texture_data;
    vec3* bezierPoints;
    int numberOfPoints = 1000;
    bool isInFpsMode = false;

public:

    void Init(int heightmap_width, int heightmap_height, GLuint heightMap) {
        // set heightmap size
        heightmap_width_ = heightmap_width;
        heightmap_height_ = heightmap_height;
        this->heightmap_texture_id_ = heightMap;

        texture_data = new float[heightmap_width * heightmap_height];
        glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, texture_data);

        bezierPoints = new vec3[numberOfPoints];
        initBezierCurve();
    }

    void Cleanup() {
        delete[] texture_data;
        delete[] bezierPoints;
    }

   /*
    // simple linear interpolation between two points
    void lirp(vec2& dest, vec2& a, vec2& b, float t) {
        dest.x = a.x + (b.x - a.x)*t;
        dest.y = a.y + (b.y - a.y)*t;
    }

    // evaluate a point on a bezier-curve. t goes from 0 to 1.0
    void bezier(vec2 &dest, const float t) {
        vec2 a = vec2(-1.0, -1.0);
        vec2 b = vec2(-0.5, -0.5);
        vec2 c = vec2(0.5, 0.5);
        vec2 d = vec2(1.0, 1.0);
        vec2 ab, bc, cd, abbc, bccd;

        lirp(ab, a, b, t);           // point between a and b
        lirp(bc, b, c, t);           // point between b and c
        lirp(cd, c, d, t);           // point between c and d
        lirp(abbc, ab, bc, t);       // point between ab and bc
        lirp(bccd, bc, cd, t);       // point between bc and cd
        lirp(dest, abbc, bccd, t);   // point on the bezier-curve
    }

    void initBezierCurve() {
        const float height = 0.5;

        for(int i = 0; i < numberOfPoints; i++) {
            vec2 p;
            float t = i/(float(numberOfPoints-1));
            bezier(p, t);
            bezierPoints[i] = vec3(p.x, height, p.y);
        }
    }

    void bezierCurve(vec3 &look, vec3& pos) {
        for (int i = 0; i < 1000)
    }
    */

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

            float fakeTmpX = tmpX;
    		//3 rotate on Z axis
    		rotate2D(0,0,fakeTmpX,tmpY, angle);

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
        //Map into heighmat coordinates
        int tmpX = int((1.0+pos.x) * heightmap_width_/2.0);
        int tmpZ = int((1.0+pos.z) * heightmap_height_/2.0);

        if(tmpX > 0 && tmpX < heightmap_width_ && tmpZ > 0 && tmpZ < heightmap_height_) {
            float height = texture_data[tmpX+heightmap_width_*tmpZ];
            pos.y = height+0.05;
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
