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
    float x, y, z;
    int blockType;
    bool isSolid;

    Block(const float x, const float y, const float z, const int blockType, const bool isSolid)
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

    void addPoints(const std::vector<Vector3>& newVertices)
    {
        for (Vector3 vertex : newVertices)
        {
            vertices.push_back(vertex.x);
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);
        }
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
    [[nodiscard]] std::vector<bool> checkBlockExposedFaces(const int i, const int j, const int k) const
    {
        std::vector<bool> exposedFaces = { false, false, false, false, false, false };

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
                    const float x = (i * BLOCK_SIZE) + (BLOCK_SIZE / 2);
                    const float y = (k * BLOCK_SIZE) + (BLOCK_SIZE / 2);
                    const float z = (j * BLOCK_SIZE) + (BLOCK_SIZE / 2);

                    if (k <= columnHeight)
                    {
                        blocks[i][k][j] = Block(x, y, z, Grass, true);
                        continue;
                    }
                    blocks[i][k][j] = Block(x, y, z, Air, false);
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
                for (int i = 0; i < 8 * chunkX; i++)
                {
                    for (int j = 0; j < 8 * chunkY; j++)
                    {
                        for (int k = 0; k < height; k++)
                        {
                            const Block& currentBlock = blocks[i][j][k];
                            std::vector<bool> exposedFaces = checkBlockExposedFaces(i, j, k);

                            if (exposedFaces[UP])
                            {
                                const float upOffset = currentBlock.y - (BLOCK_SIZE / 2);
                                const float left = currentBlock.x - (BLOCK_SIZE / 2);
                                const float right = currentBlock.x + (BLOCK_SIZE / 2);
                                const float backward = currentBlock.z - (BLOCK_SIZE / 2);
                                const float forward = currentBlock.z + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { left, upOffset, backward },   // left-back
                                    { right, upOffset, backward },  // right-back
                                    { left, upOffset, forward },    // left-front
                                    { right, upOffset, forward }    // right-front
                                };

                                worldMesh.addPoints(points);
                            }

                            if (exposedFaces[DOWN])
                            {
                                const float downOffset = currentBlock.y + (BLOCK_SIZE / 2);
                                const float left = currentBlock.x - (BLOCK_SIZE / 2);
                                const float right = currentBlock.x + (BLOCK_SIZE / 2);
                                const float backward = currentBlock.z - (BLOCK_SIZE / 2);
                                const float forward = currentBlock.z + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { left, downOffset, backward },   // left-back
                                    { right, downOffset, backward },  // right-back
                                    { left, downOffset, forward },    // left-front
                                    { right, downOffset, forward }    // right-front
                                };

                                worldMesh.addPoints(points);
                            }

                            if (exposedFaces[RIGHT])
                            {
                                const float rightOffset = currentBlock.x + (BLOCK_SIZE / 2);
                                const float up = currentBlock.y - (BLOCK_SIZE / 2);
                                const float down = currentBlock.y + (BLOCK_SIZE / 2);
                                const float backward = currentBlock.z - (BLOCK_SIZE / 2);
                                const float forward = currentBlock.z + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { rightOffset, up, backward },   // top-back
                                    { rightOffset, down, backward },  // bottom-back
                                    { rightOffset, up, forward },    // top-front
                                    { rightOffset, down, forward }    // bottom-front
                                };

                                worldMesh.addPoints(points);
                            }

                            if (exposedFaces[LEFT])
                            {
                                const float leftOffset = currentBlock.x - (BLOCK_SIZE / 2);
                                const float up = currentBlock.y - (BLOCK_SIZE / 2);
                                const float down = currentBlock.y + (BLOCK_SIZE / 2);
                                const float backward = currentBlock.z - (BLOCK_SIZE / 2);
                                const float forward = currentBlock.z + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { leftOffset, up, backward },   // top-back
                                    { leftOffset, down, backward },  // bottom-back
                                    { leftOffset, up, forward },    // top-front
                                    { leftOffset, down, forward }    // bottom-front
                                };

                                worldMesh.addPoints(points);
                            }

                            if (exposedFaces[FRONT])
                            {
                                const float forwardOffset = currentBlock.z + (BLOCK_SIZE / 2);
                                const float up = currentBlock.y - (BLOCK_SIZE / 2);
                                const float down = currentBlock.y + (BLOCK_SIZE / 2);
                                const float left = currentBlock.x - (BLOCK_SIZE / 2);
                                const float right = currentBlock.x + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { left, up, forwardOffset },   // top-left
                                    { right, up, forwardOffset },  // top-right
                                    { left, down, forwardOffset },    // bottom-left
                                    { right, down, forwardOffset }    // bottom-right
                                };

                                worldMesh.addPoints(points);
                            }

                            if (exposedFaces[BACK])
                            {
                                const float backOffset = currentBlock.z - (BLOCK_SIZE / 2);
                                const float up = currentBlock.y - (BLOCK_SIZE / 2);
                                const float down = currentBlock.y + (BLOCK_SIZE / 2);
                                const float left = currentBlock.x - (BLOCK_SIZE / 2);
                                const float right = currentBlock.x + (BLOCK_SIZE / 2);
                                const std::vector<Vector3> points = {
                                    { left, up, backOffset },   // top-left
                                    { right, up, backOffset },  // top-right
                                    { left, down, backOffset },    // bottom-left
                                    { right, down, backOffset }    // bottom-right
                                };

                                worldMesh.addPoints(points);
                            }
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
