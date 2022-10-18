# Documentation

Here lies the preliminary documentation for the engine

## Coordinate system

The engine uses the coordinate system below and normalizes it for
all backends.

NDC: +Y is up. Point(-1, -1) is at the bottom left corner
Framebuffer coordinate: +Y is down. Origin(0, 0) is at the top left corner
Texture coordinate: +Y is down. Origin(0, 0) is at the top left corner.

> **Note:**
> This coordinate system is based on modern APIs such as DX12, Metal, and WebGPU.
> In Vulkan coordinate system, NDC +Y is down; so, the engine will invert this
> automatically when Vulkan is used as a rendering backend.
