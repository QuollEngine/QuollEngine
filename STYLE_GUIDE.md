# Style Guide

This document covers basics for styling all the projects in this repository.

## Formatting

All C++ source and shader files in this project are formatted using Clang Format
using [LLVM coding standards](https://llvm.org/docs/CodingStandards.html#source-code-formatting). Please
refer to [.clang-format](./.clang-format) to view clang-format properties.

## Docblocks

All the source code, including shaders must have docblocks. The docblocks are automatically verified when
creating Pull requests. Doxygen style commands are allowed inside docblocks but they must **always**
start with `@` character instead of `\` (e.g `@param`).

As a general rule of thumb, descriptions of functions and compound interface **must**
start with `@brief` command while non-static public member variables of classes
do not need to be annotated with `@brief` command.

## Shaders

Spir-V GLSL shaders are used in this project. The following style guides must be followed for all shaders:

### File names

All shader files must be named in kebab-case format:

```
geometry-skinned.vert
geometry-skinned.frag
```

### Code order

The following code order must be followed in all shaders:

```
1. Shader specific pragmas (e.g version, extensions, include)
2. Inputs
3. Outputs
4. Uniforms
5. Push constants
6. Global constants
7. Functions
8. `main` function
```

**Inputs and Outputs:**

All inputs must be ordered by their location in ascending order:

```glsl
layout(location = 0) inPosition;
layout(location = 1) inNormal;
layout(location = 3) inColor;

layout(location = 0) out vec4 outColor;
layout(location = 6) out mat3 outTBN;
```

**Uniforms:**

All uniforms must be ordered by their set, then by their
binding in ascending order:

```
layout(set = 0, binding = 0) uniform CameraData { ... } uCameraData;
layout(set = 0, binding = 1) uniform SceneData { ... } uSceneData;
layout(set = 0, binding = 2) uniform samplerCube uIblMaps[2];

layout(set = 1, binding = 0) uniform MaterialData { ... } uMaterialData;
layout(set = 1, binding = 1) uniform sampler2D uTextures[8];
```

> **Note:**
>
> In order to increase readability, a new line must exist between
> each group of items (e.g inputs, outputs, uniforms etc)

### Naming conventions

In this guide, we will discuss naming conventions for all function and variable types.

**Variables:**

All non-constant variables must be defined in camelCase. Example:

```glsl
vec4 baseColor;
```

> **Exceptions:**
>
> Variables that contain acronyms or vector names
> can use capital cases to represent them.
> Example: `NdotL`, `inTBN`, `uBrdfLUT`

**Constants:**

Constant variables must be defined in CONSTANT_CASE. Example:

```glsl
const float DIELECTRIC_SPECULAR = 0.04;
```

**Structures:**

Structures must always be defined in PascalCase. Example:

```glsl
struct LightCalculation {
  float NdotL;
  float NdotH;
  float VdotH;
  float intensity;
};
```

**Inputs:**

All shader inputs must start with `in` and must be defined in camelCase. Example:

```glsl
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
```

**Outputs:**

All shader outputs must start with `out` and must be defined in camelCase. Example:

```glsl
layout(location = 0) out vec4 outColor;
```

**Uniforms:**

All uniforms (buffers and textures) must start with `u` and must be defined in camelCase. Example:

```glsl
layout(set = 1, binding = 0) uniform CameraData {
    mat4 projection;
    mat4 view;
    mat4 viewProj;
} uCameraData;

layout(set = 1, binding = 1) uniform sampler2D uTextures;
```

> **Note:**
>
> Uniform buffer's layout is written in PascalCase
> because it is treated as any other structure.

**Push constants:**

All push constants must start with `pc` and must be defined in camelCase. Example:

```glsl
layout(push_constant) uniform ModelTransform {
    mat4 modelMatrix
} pcTransform;
```

> **Note:**
>
> Push constant's buffer's layout is written in PascalCase
> because it is treated as any other structure.

**Functions:**

All functions must be defined in camelCase. Example:

```glsl
vec3 lambertianDiffuse(vec3 diffuseColor) { return diffuseColor / PI; }
```

### Comments

**Variables:**

Variable names must be self-descriptive. In rare cases of variables
requiring additional descriptions, single-line comments must be used.

**Functions:**

All functions must have multi-line comments that start with
two stars (`/**`) and must use Doxygen commands that
start with `@` sign. Example:

```glsl
/**
 * Lambertian diffuse
 *
 * @param diffuseColor Diffuse color
 * @return Diffuse value
 */
vec3 lambertianDiffuse(vec3 diffuseColor) { return diffuseColor / PI; }
```

> **Note:**
>
> It is not required, nor encouraged to add comments
> for the `main` function

**Code:**

If a code snippet needs to be explained with comments, single line
comments must be used.
