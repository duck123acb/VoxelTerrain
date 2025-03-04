#ifndef FREECAM_H
#define FREECAM_H

#include "raylib.h"

class FreeCam
{
    float yaw;   // Left/Right rotation
    float pitch; // Up/Down rotation

public:
    Camera3D camera;
    FreeCam();
    void update();
};

#endif