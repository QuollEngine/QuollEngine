# Liquid Engine

![MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat) ![C++](https://img.shields.io/badge/language-C++-orange.svg?style=flat) ![Clang Format](https://img.shields.io/badge/formatter-Clang--Format-red.svg?style=flat)

A game engine hobby project by Gasim Gasimzada.

## Current Status

- [ ] Vulkan RHI
    - [x] Generation of pipeline layouts using Spir-V reflection
    - [x] Single threaded command execution
    - [x] Command buffer abstraction using Render Command List
    - [x] Handling descriptors
    - [ ] Split RHI from renderer
- [ ] Renderer
    - [x] Render Graph support
    - [x] Simple shadow maps
    - [x] Skybox
    - [x] PBR with IBL
    - [ ] Instancing
    - [ ] Deferred rendering
    - [ ] Cascaded shadow mapping
    - [ ] Multi-threaded Render Graph evaluation
    - [ ] Font rendering
    - [ ] Water rendering
    - [ ] HDR
    - [ ] Tonemapping
    - [ ] Raytracing pipeline
- [ ] Editor
    - [x] Load GLTF scenes into scene
    - [x] Simple UI to view entity information
    - [x] Render scene into Imgui panel
    - [ ] Quaternion based arcball camera
    - [x] Editor grid
    - [x] Docking
    - [ ] Gizmos
    - [ ] Trackpad support
    - [ ] Mouse movement improvements
    - [ ] Keyboard shortcuts
    - [ ] Mesh component details
    - [x] Status bar
    - [ ] macOS support
- [x] Core
    - [x] ECS using Sparse Sets
    - [x] Simple logger using streams
- [x] Stats manager
    - [x] Number of allocated resources (images, buffers)
    - [x] Number of draw calls
    - [ ] Number of bound descriptors, pipelines, and buffers
- [x] Profiler 
- [x] Animation
- [ ] Physics
- [ ] Audio
- [ ] Scripting
- [ ] Project
    - [x] Premake based project that support Linux, macOS, and Windows
    - [x] Dependency fetcher script
    - [ ] Increase test coverage
    - [ ] Enable codecov in main branch for comparison
    - [x] Replace exceptions with assertions

## License

Licensed under [MIT](./LICENSE).
