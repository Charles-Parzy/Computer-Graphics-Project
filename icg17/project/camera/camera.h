#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    float* texture_data;
    //vec3* bezierPoints;
    int numberOfPathPoints = 6;
    int numberOfCamPoints = 6;
    bool isInFpsMode = false;
    bool isInBezierMode = false;
    float t = 0;

public:

    void Init(int heightmap_width, int heightmap_height, GLuint heightMap) {
        // set heightmap size
        heightmap_width_ = heightmap_width;
        heightmap_height_ = heightmap_height;
        this->heightmap_texture_id_ = heightMap;

        texture_data = new float[heightmap_width * heightmap_height];
        glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, texture_data);

        //initBezierCurve();
    }

    void Cleanup() {
        delete[] texture_data;
    }

    vec3* initPathPoints() {
        vec3* bezierPoints = new vec3[numberOfPathPoints];
        bezierPoints[0] = vec3(-0.8,0.4,0.0);
        bezierPoints[1] = vec3(-0.8,0.4,-0.8);
        bezierPoints[2] = vec3(0.8,0.4,-0.8);
        bezierPoints[3] = vec3(0.8,0.4,0.8);
        bezierPoints[4] = vec3(-0.8,0.4,0.8);
        bezierPoints[5] = vec3(-0.8,0.4,0.0);
        return bezierPoints;
    }

    vec3* initCamPoints() {
        vec3* bezierPoints = new vec3[numberOfCamPoints];
        bezierPoints[0] = vec3(-1.0,0.25,0.0);
        bezierPoints[1] = vec3(-1.0,0.25,-1.0);
        bezierPoints[2] = vec3(1.0,0.25,-1.0);
        bezierPoints[3] = vec3(1.0,0.25,1.0);
        bezierPoints[4] = vec3(-1.0,0.25,1.0);
        bezierPoints[5] = vec3(-1.0,0.25,0.0);
        return bezierPoints;
    }

    vec3 getBezierLocation(vec3* points, int size, const float t){
        if (size == 1) {
            vec3 point = points[0];
            delete [] points;
            return point;
        } else {
            vec3* newpoints = new vec3[size - 1];
            for (int i = 0; i < size - 1; i++) {
                newpoints[i] = (1 - t) * points[i] + t * points[i+1];
            }
            delete [] points;
            return getBezierLocation(newpoints, size - 1, t);
        }
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

            float fakeTmpX = tmpX;
    		//3 rotate on Z axis
    		rotate2D(0,0,fakeTmpX,tmpY, angle);

            look.x = tmpX + pos.x;
    		look.y = tmpY + pos.y;
    		look.z = tmpZ + pos.z;
    }

    void moveFrontBack(vec3 &look, vec3 &pos, float speed) {
      
        if(speed != 0) {
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
                    if (height > 0) {
                        pos.y = height + 0.02;
                    } else {
                        pos.y = 0.02;
                    }
                }
            } else if(isInBezierMode) {
                t += speed;
                //cout << "t=" << t << endl;
                //cout << "before " << look.x << " " << look.y << " " << look.z << endl;

                look = getBezierLocation(initCamPoints(), numberOfCamPoints, mod(t, 1.0f));
                //cout << "before " << look.x << " " << look.y << " " << look.z << endl;
                pos = getBezierLocation(initPathPoints(), numberOfPathPoints, mod(t, 1.0f));
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
    }

    void switchInFpsMode() {
        cout << "FPS MODE " << isInFpsMode << endl;
        isInBezierMode = false;
        isInFpsMode = !isInFpsMode;
    }

    void switchInBezierMode() {
        cout << "Bezier MODE " << isInFpsMode << endl;
        isInFpsMode = false;
        isInBezierMode = !isInBezierMode;
    }
};
