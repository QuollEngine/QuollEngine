#extension GL_EXT_nonuniform_qualifier : enable

#define Bindless 1
#define BindlessDescriptorSet 0
#define BindlessStorageBinding 0
#define BindlessUniformBinding 1

/**
 * @brief Get variable name of a layout
 *
 * @param Name Layout name
 */
#define GetLayoutVariableName(Name) u##Name##Register

/**
 * @brief Register storage buffer
 *
 * @param Layout Layout type (e.g std430)
 * @param BufferAccess Buffer access type (e.g readonly)
 * @param Name Layout name
 * @param Struct Structure
 */
#define RegisterBuffer(Layout, BufferAccess, Name, Struct)                     \
  layout(Layout, set = BindlessDescriptorSet,                                  \
         binding = BindlessStorageBinding) BufferAccess buffer Name Struct     \
  GetLayoutVariableName(Name)[]

/**
 * @brief Register uniform buffer
 *
 * @param Name Layout name
 * @param Struct Structure
 */
#define RegisterUniform(Name, Struct)                                          \
  layout(set = BindlessDescriptorSet, binding = BindlessUniformBinding)        \
      uniform Name Struct                                                      \
      GetLayoutVariableName(Name)[]

/**
 * @brief Get bindless resource by index
 *
 * @param Name Layout name
 * @param Index Resource index
 */
#define GetBindlessResource(Name, Index) GetLayoutVariableName(Name)[Index]

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer DummyGlobalData {
  uint ignore;
}
uGlobalBuffers[];

layout(set = BindlessDescriptorSet,
       binding = BindlessUniformBinding) uniform DummyUniformData {
  uint ignore;
}
uGlobalUniforms[];
