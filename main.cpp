#include "raylib.h"
#include <cmath>
#include <memory>

#define MOUSE_SENSITIVITY 0.003f
#define SPEED 5.0f

class FreeCam
{
    float yaw = 0.0f;   // Left/Right rotation
    float pitch = 0.0f; // Up/Down rotation

public:
    Camera3D camera = { 0 };
    FreeCam()
    {
        camera.position = (Vector3){ 0.0f, 2.0f, 4.0f }; // Start position
        camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; // Looking forward
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }

    void Update()
    {
        // calculate directions
        Vector2 mouseDelta = GetMouseDelta();
        yaw += mouseDelta.x * MOUSE_SENSITIVITY;
        pitch -= mouseDelta.y * MOUSE_SENSITIVITY;

        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;

        Vector3 forward = {
            cosf(yaw) * cosf(pitch),
            sinf(pitch),
            sinf(yaw) * cosf(pitch)
        };
        Vector3 right = {
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
};

int main() {
    InitWindow(800, 600, "Voxel Terrain");
    DisableCursor(); // Hide and lock cursor to the window

    const auto freeCam = std::make_unique<FreeCam>();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        freeCam->Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(freeCam->camera);
            DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, RED); // A red voxel
            DrawGrid(10, 1.0f);
        EndMode3D();

        DrawFPS(10, 40);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
