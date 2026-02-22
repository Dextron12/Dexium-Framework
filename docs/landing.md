> WIP V0.16.5-a
---

Dexium-Framework is a rendering-first game development framework for 2D and 3D projects, built around clarity, control, and modular design.

It exists to remove the repetitive boilerplate of modern OpenGL setup while still giving developers full control over rendering state, assets, and pipeline behaviour. No heavy engine hierarchies. No forced ECS. No magic systems you have to fight.

Just structured tools that stay out of your way.

Dexium is built with an engineering mindset: efficiency, modularity, and extensibility come first — whether you're prototyping a game jam project or building toward something much larger.

---
## Project Status
Dexium is currently in **Alpha**
Core rendering systems are stable and usable, but APIs and internal structure may evolve as the framework continues to mature.

> This is a solo-developed project and is actively evolving.
---
# Motivation

Dexium started from a familiar frustration.

Coming from a background of building small SDL2 projects and attempting game jams, I repeatedly found myself rewriting the same foundational systems: window creation, renderer setup, filesystem management, logging, asset loading — over and over again.

Eventually, those pieces were consolidated into a reusable framework.

But the pull toward 3D and lower-level rendering control was always there. After diving deep into OpenGL (with help from [LearnOpenGL](https://learnopengl.com/)), I made the decision to fully rewrite Dexium around OpenGL instead of SDL2.

SDL2 is no longer part of this framework.

The goal now is clear:

- Reduce OpenGL boilerplate
    
- Provide clean abstractions over common rendering workflows
    
- Keep the architecture flexible
    
- Avoid unnecessary engine complexity
    
- Stay strongly typed and explicit
    

Dexium aims to make rendering intentional without being exhausting.

---
## Core Philosophy

Dexium follows a few guiding principles:

- **Rendering First** – The renderer is the backbone. Everything else builds around it.
    
- **Minimal Abstraction, Maximum Clarity** – Abstractions should reduce friction, not hide behaviour.
    
- **Modular Systems** – Components can be used independently.
    
- **No Forced Architecture** – No internal ECS requirement, no rigid scene trees.
    
- **Engineer-Friendly** – Built for developers who want to understand what’s happening under the hood.

---
## Third-Party Libaries

Dexium builds on reliable tools rather than reinventing them:

- **GLFW** – OpenGL context creation and input handling
    
- **GLAD2** – OpenGL loader
    
- **fmt** – Modern formatting and coloured terminal output
    
- **stb_image** – Image loading (PNG, JPEG)
    
- **EnTT** – Currently used primarily for its signal system (Dexium does _not_ use ECS internally)
    

### Planned (Optional)

- **Dear ImGui (Docking branch)**  
    Intended for an optional built-in dev terminal and runtime inspection tools.
    
    This will be completely optional via CMake and disabled by default.  
    Official releases will not depend on it.

---
## Systems in Place

>Dexium is in active development. System names and APIs may change

### WindowContext

Wraps GLFW and OpenGL context creation into a clean, single-entry setup.  
Minimal setup required to create and manage a window.

### Renderer

A command-submission based renderer.

Objects submit `RenderCommand`s describing _what_ and _how_ to render.  
Layers group commands together, functioning similarly to lightweight scenes.

No node trees. No rigid scene hierarchies.

### Layer

An injection point for user-defined game logic.  
Switch between layers like `Menu` and `Game` without complex scene management.

### Shader

Encapsulates GLSL compilation and linking.  
Provides straightforward uniform management for common data types.

### Material

Bridges shaders, textures, and uniform data.  
Designed to be shareable across objects to reduce duplication and improve organisation.

### Mesh

Encapsulates VAO, VBO, and EBO configuration.  
Supports default 2D and 3D primitives (triangle, quad, cube, sphere) with room for expansion.

### Texture

Loads image data and uploads it to OpenGL.  
Designed for straightforward integration into materials.

### Transform

Simple Position / Rotation / Scale structure for object placement.

### VFS (Virtual File System)

Thin wrapper over `std::filesystem` to standardise asset lookup and path resolution.

### Logger

Configurable logging system with bitwise flags controlling:

- Formatting style
    
- Output sinks (stdout, stderr, file)
    
- Future dev terminal integration
    

### Colour

Strongly-typed RGBA structure mapped to `[0,1]` range for OpenGL usage.

### ID

UUIDv4-based identifier system with optional labels for debugging and introspection.

---
## Where Dexium is Headed

Dexium is gradually moving toward:

- Improved material batching
    
- Multi-texture support in the renderer
    
- Expanded mesh generation
    
- Hot-reloadable assets for development builds
    
- Optional in-engine debugging tools
    
- A stronger foundation for 3D workflows
    

The long-term vision is to let Dexium grow naturally into a flexible engine core — without losing the clarity and control that define it.

---
## Why Dexium Exists

Dexium isn't trying to compete with established engines.

It exists because sometimes you want:
- More control than high-level engines offer
- Less boilerplate than raw OpenGL demands
- And an architecture you actually understand

Dexium aims to be that middle ground.