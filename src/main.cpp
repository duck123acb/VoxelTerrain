#include "Freecam.h"

#include "raylib.h"
#include <vector>

#define BLOCK_SIZE 16

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
    float x, z, y;
    int blockType;
    bool isSolid;

    Block(const float x, const float z, const float y, const int blockType, const bool isSolid)
    {
        this->x = x;
        this->z = z;
        this->y = y;
        this->blockType = blockType;
        this->isSolid = isSolid;
    }
};

struct WorldMesh
{
    std::vector<float> vertices;
    std::vector<float> indices;

    void addPoints(const std::vector<Vector3>& newVertices)
    {
        const float startIndex = vertices.size() / 3;

        for (const auto& [x, y, z] : newVertices)
        {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }

        // Create two triangles (0,1,2) and (1,3,2)
        indices.push_back(startIndex);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);

        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 3);
        indices.push_back(startIndex + 2);
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
    [[nodiscard]] std::vector<bool> checkBlockExposedFaces(const int x, const int z, const int y) const
    {
        std::vector exposedFaces = { false, false, false, false, false, false };

        if (y + 1 >= height || blocks[x][z][y + 1].blockType == Air) {
            exposedFaces[UP] = true;
        }

        if (y - 1 < 0 || blocks[x][z][y - 1].blockType == Air) {
            exposedFaces[DOWN] = true;
        }

        if (x - 1 < 0 || blocks[x - 1][z][y].blockType == Air) {
            exposedFaces[LEFT] = true;
        }

        if (x + 1 >= height || blocks[x + 1][z][y].blockType == Air) {
            exposedFaces[RIGHT] = true;
        }

        if (z + 1 >= height || blocks[x][z + 1][y].blockType == Air) {
            exposedFaces[FRONT] = true;
        }

        if (z - 1 < 0 || blocks[x][z - 1][y].blockType == Air) {
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
            for (int k = 0; k < depth; k++) // iterate over depth (z-axis)
            {
                const int columnHeight = heightMap[i][k];
                constexpr float halfBlockSize = BLOCK_SIZE / 2;

                // generate blocks up to the height determined by the heightmap
                for (int j = 0; j < height; j++) // iterate over height (y-axis)
                {
                    const float x = (i * BLOCK_SIZE) + halfBlockSize;
                    const float z = (k * BLOCK_SIZE) + halfBlockSize;
                    const float y = (j * BLOCK_SIZE) + halfBlockSize;

                    if (j <= columnHeight)
                    {
                        blocks[i][k][j] = Block(x, z, y, Grass, true);
                        continue;
                    }
                    blocks[i][k][j] = Block(x, z, y, Air, false);
                }
            }
        }
    }

    void updateMeshes() const
    {
        // chunk blocks into 8x8xHeight grids to generate voxel data
        for (int chunkX = 1; chunkX < 9; chunkX++)
        {
            for (int chunkY = 1; chunkY < 9; chunkY++)
            {
                auto worldMesh = WorldMesh();

                for (int k = 0; k < height; k++)
                {
                    for (int i = 0; i < 8 * chunkX; i++)
                    {
                        for (int j = 0; j < 8 * chunkY; j++)
                        {
                        }
                    }

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
