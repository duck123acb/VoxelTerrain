#include "Freecam.h"
#include "raylib.h"
#include <cmath>

#define SPEED 5.0f
#define MOUSE_SENSITIVITY 0.003f

FreeCam::FreeCam()
{
    yaw = 0.0f;
    pitch = 0.0f;

    camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f }; // Start position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; // Looking forward
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void FreeCam::update()
{
    // calculate directions
    auto [x, y] = GetMouseDelta();
    yaw += x * MOUSE_SENSITIVITY;
    pitch -= y * MOUSE_SENSITIVITY;

    if (pitch > 1.5f) pitch = 1.5f;
    if (pitch < -1.5f) pitch = -1.5f;

    const Vector3 forward = {
        cosf(yaw) * cosf(pitch),
        sinf(pitch),
        sinf(yaw) * cosf(pitch)
    };
    const Vector3 right = {
        cosf(yaw - PI / 2.0f), // Rotate 90° counterclockwise
        0.0f,
        sinf(yaw - PI / 2.0f)
    };


    // move cam
    if (IsKeyDown(KEY_W)) {
        camera.position.x += forward.x * GetFrameTime() * SPEED;
        camera.position.z += forward.z * GetFrameTime() * SPEED;
    }
    if (IsKeyDown(KEY_S)) {
        camera.position.x -= forward.x * GetFrameTime() * SPEED;
        camera.position.z -= forward.z * GetFrameTime() * SPEED;
    }
    if (IsKeyDown(KEY_A)) {
        camera.position.x += right.x * GetFrameTime() * SPEED;
        camera.position.z += right.z * GetFrameTime() * SPEED;
    }
    if (IsKeyDown(KEY_D)) {
        camera.position.x -= right.x * GetFrameTime() * SPEED;
        camera.position.z -= right.z * GetFrameTime() * SPEED;
    }
    if (IsKeyDown(KEY_E))
    {
        camera.position.y += GetFrameTime() * SPEED;
    }
    if (IsKeyDown(KEY_Q))
    {
        camera.position.y -= GetFrameTime() * SPEED;
    }

    // rotate cam
    camera.target = (Vector3){
        camera.position.x + forward.x,
        camera.position.y + forward.y,
        camera.position.z + forward.z
    };
}