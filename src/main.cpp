#include "raylib.h"
#include <cmath>
#include <memory>
#include <vector>

#define MOUSE_SENSITIVITY 0.003f
#define SPEED 5.0f
#define BLOCK_SIZE 16

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

    void update()
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
};

enum BlockTypes
{
    Air,
    Grass
};
struct Block
{
    int x, y, z;
    int blockType;
    bool isSolid;

    Block(const int x, const int y, const int z, const int blockType, const bool isSolid)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->blockType = blockType;
        this->isSolid = isSolid;
    }
};

class World
{
    std::vector<std::vector<std::vector<Block>>> blocks;

    float vertices[];
    unsigned short indices[];

    static int generateHeight(const int x, const int y) // generate terrain based on perlin noise
    {
        return 0;
    }

    void generateWorld(const int width, const int depth, const int height)
    {
        // generate heightmap
        std::vector heightMap(width, std::vector<int>(depth));
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < depth; j++)
            {
                heightMap[i][j] = generateHeight(i, j);
            }
        }

        // generate an internal representation of the world based on the heightMap
        blocks = std::vector(64, std::vector(64, std::vector<Block>(64)));
        for (int i = 0; i < 64; i++) // iterate over depth (z-axis)
        {
            for (int j = 0; j < 64; j++) // iterate over width (x-axis)
            {
                const int columnHeight = heightMap[i][j];

                // generate blocks up to the height determined by the heightmap
                for (int k = 0; k < height; k++) // iterate over height (y-axis)
                {
                    if (k <= columnHeight)
                    {
                        blocks[i][k][j] = Block(i, k, i, Grass, true);
                        continue;
                    }
                    blocks[i][k][j] = Block(i, k, i, Air, false);
                }
            }
        }

        // generate voxel data
    }

public:
    World(const int world_width, const int world_depth, const int world_height)
    {
        generateWorld(world_width, world_depth, world_height);
    }
    ~World() = delete;



    void render()
    {

    }
};


int main() {
    InitWindow(800, 600, "Voxel Terrain");
    DisableCursor(); // Hide and lock cursor to the window

    const auto freeCam = std::make_unique<FreeCam>();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        freeCam->update();

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
