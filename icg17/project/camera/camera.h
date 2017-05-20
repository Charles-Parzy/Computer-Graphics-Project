#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    float* texture_data;
    int numberOfPathPoints = 6;
    int numberOfCamPoints = 6;
    bool isInFpsMode = false;
    bool isInBezierMode = false;
    float t = 0;

public:

    void Init(int heightmap_width, int heightmap_height, GLuint heightMap) {
        heightmap_width_ = heightmap_width;
        heightmap_height_ = heightmap_height;
        this->heightmap_texture_id_ = heightMap;

        texture_data = new float[heightmap_width * heightmap_height];
        glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, texture_data);
    }

    void Cleanup() {
        delete[] texture_data;
    }

    bool isCurrentlyInBezierMode() {
        return isInBezierMode;
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

    // Multiply 2 coordinates of look vector by a rotation matrix
    // cos t, -sin t
    // sin t, cos t
    void rotate(float& look1, float& look2, float angle) {
    	look1 = look1*cos(angle) - look2*sin(angle);
    	look2 = look1*sin(angle) + look2*cos(angle);
    }

    void rotateLeftRight(vec3 &look, vec3 pos, float angle) {
    		float dX = look.x - pos.x;
    		float dZ = look.z - pos.z;

    		rotate(dX,dZ,angle);

            // Add pos to get the new coordinates of look
    		look.x = dX + pos.x;
    		look.z = dZ + pos.z;
    }

    void rotateUpDown(vec3 &look, vec3 pos, float angle) {
            float dX = look.x - pos.x;
    		float dY = look.y - pos.y;

            // Absolute value of x to be independ of the look position. Otherwise commdands are inverted
            dY = abs(dX)*sin(angle) + dY*cos(angle);

            // Add pos to get the new coordinates of look
            look.x = dX + pos.x;
    		look.y = dY + pos.y;
    }

    void moveFrontBack(vec3 &look, vec3 &pos, float speed) {

        if(isInFpsMode) {
            float dX = (look.x - pos.x)*speed;
            float dZ = (look.z - pos.z)*speed;

            float tmpPosX = pos.x + dX;
            float tmpPosZ = pos.z + dZ;

            // check that the user doesn't get out of the terrain
            if (tmpPosX < 1.0 && tmpPosX > -1.0) {
                pos.x = tmpPosX;
                look.x = look.x + dX;
            }
            if (tmpPosZ < 1.0 && tmpPosZ > -1.0) {
                pos.z = tmpPosZ;
                look.z = look.z + dZ;
            }
            //Map camera position (-1, 1) into heighmap coordinates (0,0 to ....)
            int coordX = int((1.0+pos.x) * heightmap_width_/2.0);
            int coordZ = int((1.0+pos.z) * heightmap_height_/2.0);

            // Simple check on heighmap boundaries
            if(coordX > 0 && coordX < heightmap_width_ && coordZ > 0 && coordZ < heightmap_height_) {
                float height = texture_data[coordX+heightmap_width_*coordZ];
                pos.y = height+0.05; // Extra height for fps mode
            }

        } else if(isInBezierMode) {
            t += speed;
            //cout << "t=" << t << endl;
            //cout << "before " << look.x << " " << look.y << " " << look.z << endl;

            look = getBezierLocation(initCamPoints(), numberOfCamPoints, mod(t, 1.0f));
            //cout << "before " << look.x << " " << look.y << " " << look.z << endl;
            pos = getBezierLocation(initPathPoints(), numberOfPathPoints, mod(t, 1.0f));
        } else {
            float dX = (look.x - pos.x)*speed;
    		float dY = (look.y - pos.y)*speed;
    		float dZ = (look.z - pos.z)*speed;

    		pos.x = pos.x + dX;
    		pos.y = pos.y + dY;
    		pos.z = pos.z + dZ;

    		look.x = look.x + dX;
    		look.y = look.y + dY;
    		look.z = look.z + dZ;
        }
    }

    void switchInFpsMode() {
        isInBezierMode = false;
        isInFpsMode = !isInFpsMode;
        cout << "FPS MODE " << isInFpsMode << endl;
    }

    void switchInBezierMode() {
        isInFpsMode = false;
        isInBezierMode = !isInBezierMode;
        cout << "Bezier MODE " << isInBezierMode << endl;
    }
};
