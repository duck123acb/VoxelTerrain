# Voxel Terrain Generation

this is kinda a test just to figure out freecam/voxel terrain gen before i do stuff in J2ME  
built using [Raylib](https://www.raylib.com/)


## 🌍 World Structure
- The world consists of **64×64×64** blocks.
- It is divided into **8×8×64 chunks**:
  - **8 chunks along the X-axis**
  - **8 chunks along the Z-axis**
  - **Full height (64 blocks) along the Y-axis**
- Each **chunk has its own mesh** for efficient rendering.

---

## 🛠️ Generation & Rendering
### **Step 1: Generate the World**
- A **heightmap** determines the terrain height.
- Blocks are placed up to their respective height values.

### **Step 2: Create Chunk Meshes**
- Each chunk generates a mesh based on its blocks.
- Only **faces exposed to air** are included in the mesh.

### **Step 3: Updating Chunks on Block Edits**
- When the player places or destroys a block:
  - The **affected chunk’s mesh is rebuilt**.
  - Neighboring chunks update if the edited block was on a chunk boundary.

### **Step 4: Efficient Rendering**
- **Only visible chunks are rendered** to optimize performance.
- Each chunk has its own mesh, reducing unnecessary draw calls.

---

## 📝 Optimization Notes
- **Only update the chunk mesh when necessary** (on block edits).
- **Use greedy meshing** to merge adjacent faces and improve performance.
- **Cull hidden faces** (against air) to reduce the number of rendered triangles.
- **Use raycasting** to determine which block the player is interacting with.
