#pragma once
#include "icg_helper.h"

using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) {}

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeingDrag(float x, float y) {
      anchor_pos_ = vec3(x, y, 0.0f);
      ProjectOntoSurface(anchor_pos_);
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
      vec3 current_pos = vec3(x, y, 0.0f);
      ProjectOntoSurface(current_pos);

      mat4 rotation = IDENTITY_MATRIX;

      float angle = acos(glm::min(1.0f, dot(current_pos, anchor_pos_)));
      vec3 rotation_axis = cross(current_pos, anchor_pos_);
      rotation = glm::rotate(rotation, angle, rotation_axis);
      return rotation;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
      GLfloat squaredNorm = p.x * p.x + p.y * p.y;

      if (squaredNorm <= (radius_ * radius_) / 2.0f) {
        p.z = sqrt(radius_ * radius_ - squaredNorm);
      } else {
        p.z = ((radius_ * radius_) / 2.0f)/sqrt(squaredNorm);
      }
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
