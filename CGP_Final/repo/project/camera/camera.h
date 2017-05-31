#pragma once
#include "icg_helper.h"

using namespace glm;

class Camera {
private:
    GLuint heightmap_texture_id_;
    int heightmap_width_;
    int heightmap_height_;
    float* texture_data;
    int numberOfPathPoints = 10;
    int numberOfCamPoints = 10;
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
        bezierPoints[0] = vec3(0.580981,0.193522,0.92752);
        bezierPoints[1] = vec3(0.434318,0.289063,0.613651);
        bezierPoints[2] = vec3(0.129593,0.311542,0.459572);
        bezierPoints[3] = vec3(0.0819179,0.295212,-0.0944423);
        bezierPoints[4] = vec3(0.0997908,0.259328,-0.54052);
        bezierPoints[5] = vec3(0.386055,0.211299,-0.285465);
        bezierPoints[6] = vec3(0.291908,0.307192,0.403216);
        bezierPoints[7] = vec3(0.279414,0.689692,0.678209);
        bezierPoints[8] = vec3(0.287024,0.285801,-0.106626);
        bezierPoints[9] = vec3(0.375611,0.224214,-0.739321);
        return bezierPoints;
    }

    vec3* initCamPoints() {
        vec3* bezierPoints = new vec3[numberOfCamPoints];
        bezierPoints[0] = vec3(0.395785,0.0956253,0.301178);
        bezierPoints[1] = vec3(0.12333,0.0677721,0.0410597);
        bezierPoints[2] = vec3(-0.20801,0.203461,-0.239735);
        bezierPoints[3] = vec3(0.0456409,0.257796,-0.653036);
        bezierPoints[4] = vec3(0.597478,0.0606904,-0.373088);
        bezierPoints[5] = vec3(0.638517,0.230005,0.17006);
        bezierPoints[6] = vec3(0.106359,0.388643,0.715332);
        bezierPoints[7] = vec3(0.354543,0.393108,0.193992);
        bezierPoints[8] = vec3(0.274585,0.174029,-0.594867);
        bezierPoints[8] = vec3(0.411381,0.170017,-1.00387);
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
            t += speed/10;
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

    void printXYZ(vec3 &look, vec3 &pos) {
        cout << "POS x=" << pos.x << " y=" << pos.y << " z=" << pos.z << endl;
        cout << "LOOK x=" << look.x << " y=" << look.y << " z=" << look.z << endl;
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
