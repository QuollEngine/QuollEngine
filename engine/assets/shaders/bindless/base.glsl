#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_buffer_reference : require

#define Bindless 1
#define BindlessDescriptorSet 0
#define BindlessTexturesBinding 0

#define Buffer(alignment)                                                      \
  layout(buffer_reference, std430, buffer_reference_align = alignment) buffer

Buffer(16) Empty { uint pad0; };
