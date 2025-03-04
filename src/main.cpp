#include "raylib.h"
#include <cmath>
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
enum Face
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FRONT,
    BACK
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

struct WorldMesh
{
    std::vector<float> vertices;
    std::vector<float> indices;

    WorldMesh(const std::vector<float>& vertices, const std::vector<float>& indices)
    {
        this->vertices = vertices;
        this->indices = indices;
    }
};

class World
{
    std::vector<std::vector<std::vector<Block>>> blocks;
    std::vector<WorldMesh> worldMeshes;

    int height;

    static int calculateTerrainHeight(const int x, const int y) // generate terrain based on perlin noise
    {
        return 0;
    }
    [[nodiscard]] bool checkBlockExposedFaces(const int i, const int j, const int k) const
    {
        bool exposedFaces[6] = { false, false, false, false, false, false };

        // Up face (i, j, k+1)
        if (k + 1 >= height || blocks[i][j][k + 1].blockType == Air) {
            exposedFaces[UP] = true;
        }

        // Down face (i, j, k-1)
        if (k - 1 < 0 || blocks[i][j][k - 1].blockType == Air) {
            exposedFaces[DOWN] = true;
        }

        // Left face (i-1, j, k)
        if (i - 1 < 0 || blocks[i - 1][j][k].blockType == Air) {
            exposedFaces[LEFT] = true;
        }

        // Right face (i+1, j, k)
        if (i + 1 >= height || blocks[i + 1][j][k].blockType == Air) {
            exposedFaces[RIGHT] = true;
        }

        // Front face (i, j+1, k)
        if (j + 1 >= height || blocks[i][j + 1][k].blockType == Air) {
            exposedFaces[FRONT] = true;
        }

        // Back face (i, j-1, k)
        if (j - 1 < 0 || blocks[i][j - 1][k].blockType == Air) {
            exposedFaces[BACK] = true;
        }

        return exposedFaces;
    }

    void generateWorld(const int width, const int depth, const int height)
    {
        // generate heightmap
        std::vector heightMap(width, std::vector<int>(depth));
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < depth; j++)
            {
                heightMap[i][j] = calculateTerrainHeight(i, j);
            }
        }

        // generate an internal representation of the world based on the heightMap
        for (int i = 0; i < width; i++)// iterate over width (x-axis)
        {
            for (int j = 0; j < depth; j++) // iterate over depth (z-axis)
            {
                const int columnHeight = heightMap[i][j];

                // generate blocks up to the height determined by the heightmap
                for (int k = 0; k < height; k++) // iterate over height (y-axis)
                {
                    if (k <= columnHeight)
                    {
                        blocks[i][k][j] = Block(i, k, j, Grass, true);
                        continue;
                    }
                    blocks[i][k][j] = Block(i, k, j, Air, false);
                }
            }
        }
    }

    void updateMeshes()
    {
        // chunk blocks into 8x8xHeight grids to generate voxel data
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int k = 0; k < height; k++) {
                    Block& currentBlock = blocks[i][j][k];
                    auto exposedFaces = checkBlockExposedFaces(i, j, k);
                    // get voxel data
                    // connect the voxel data
                }
            }
        }
    }

public:
    World(const int world_width, const int world_depth, const int world_height)
    {
        height = world_height;
        generateWorld(world_width, world_depth, world_height);
    }


    // void render()
    // {
    //
    // }
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
