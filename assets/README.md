# Assets Directory

This directory contains textures and other assets for the Ocean FFT simulator.

## Skybox Textures (Optional)

For a proper skybox implementation, place 6 cubemap textures here:
- `skybox/right.jpg` (+X)
- `skybox/left.jpg` (-X)
- `skybox/top.jpg` (+Y)
- `skybox/bottom.jpg` (-Y)
- `skybox/front.jpg` (+Z)
- `skybox/back.jpg` (-Z)

Free skybox resources:
- https://opengameart.org/
- https://polyhaven.com/hdris
- https://www.humus.name/index.php?page=Textures

## Current Implementation

The current version uses a simple procedural sky color gradient instead of a cubemap.
To add skybox support:

1. Implement cubemap loading in `OceanRenderer.cpp`
2. Create `skybox.vert` and `skybox.frag` shaders
3. Render skybox first (with depth test disabled)
4. Use cubemap sampling in `ocean.frag` for reflections

## Notes

Keep texture sizes reasonable (e.g., 1024×1024 per face) to avoid performance issues.
Use compressed formats (DXT/BC) if your platform supports them.
