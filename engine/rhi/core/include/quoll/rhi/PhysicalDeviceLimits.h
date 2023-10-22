#pragma once

namespace quoll::rhi {

/**
 * @brief Physical device limits
 */
struct PhysicalDeviceLimits {
  /**
   * Maximum 1D image dimensions
   */
  u32 maxImageDimension1D;

  /**
   * Maximum 2D image dimensions
   */
  u32 maxImageDimension2D;

  /**
   * Maximum 3D image dimensions
   */
  u32 maxImageDimension3D;

  /**
   * Maximum cube image dimensions
   */
  u32 maxImageDimensionCube;

  /**
   * Maximum image array layers
   */
  u32 maxImageArrayLayers;

  /**
   * Maximum texel buffer elements
   */
  u32 maxTexelBufferElements;

  /**
   * Maximum uniform buffer range
   */
  u32 maxUniformBufferRange;

  /**
   * Maximum storage buffer range
   */

  u32 maxStorageBufferRange;

  /**
   * Maximum push constants size
   */

  u32 maxPushConstantsSize;

  /**
   * Maximum memory allocation count
   */
  u32 maxMemoryAllocationCount;

  /**
   * Maximum sampler allocation count
   */
  u32 maxSamplerAllocationCount;

  /**
   * Buffer image granularity
   */
  u64 bufferImageGranularity;

  /**
   * Sparse address space size
   */
  u64 sparseAddressSpaceSize;

  /**
   * Maximum bound descriptor sets
   */
  u32 maxBoundDescriptorSets;

  /**
   * Maximum number of samplers that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorSamplers;

  /**
   * Maximum number of uniform buffers that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorUniformBuffers;

  /**
   * Maximum number of storage buffers that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorStorageBuffers;

  /**
   * Maximum number of sampler images that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorSampledImages;

  /**
   * Maximum number of storage images that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorStorageImages;

  /**
   * Maximum number of input attachments that are
   * accessible in a single shader stage
   */
  u32 maxPerStageDescriptorInputAttachments;

  /**
   * Maximum number of resources that are
   * accessible in a single shader stage
   */
  u32 maxPerStageResources;

  /**
   * Maximum number of available
   * sampler descriptor sets
   */
  u32 maxDescriptorSetSamplers;

  /**
   * Maximum number of available
   * uniform buffer descriptor sets
   */
  u32 maxDescriptorSetUniformBuffers;

  /**
   * Maximum number of available dynamic
   * uniform buffer descriptor sets
   */
  u32 maxDescriptorSetUniformBuffersDynamic;

  /**
   * Maximum number of available
   * storage buffer descriptor sets
   */
  u32 maxDescriptorSetStorageBuffers;

  /**
   * Maximum number of available dynamic
   * storage buffer descriptor sets
   */
  u32 maxDescriptorSetStorageBuffersDynamic;

  /**
   * Maximum number of available
   * sampled image descriptor sets
   */
  u32 maxDescriptorSetSampledImages;

  /**
   * Maximum number of available
   * storage image descriptor sets
   */
  u32 maxDescriptorSetStorageImages;

  /**
   * Maximum number of available
   * input attachment descriptor sets
   */
  u32 maxDescriptorSetInputAttachments;

  /**
   * Maximum number of vertex
   * input attributes
   */
  u32 maxVertexInputAttributes;

  /**
   * Maximum number of vertex
   * input bindings
   */
  u32 maxVertexInputBindings;

  /**
   * Maximum vertex input attribute offset
   */
  u32 maxVertexInputAttributeOffset;

  /**
   * Maximum vertex input binding stride
   */
  u32 maxVertexInputBindingStride;

  /**
   * Maximum number of vertex output components
   */
  u32 maxVertexOutputComponents;

  /**
   * Maximum number of fragment input components
   */
  u32 maxFragmentInputComponents;

  /**
   * Maximum number of fragment output attachments
   */
  u32 maxFragmentOutputAttachments;

  /**
   * Maximum number of fragment dual source
   * attachments
   */
  u32 maxFragmentDualSrcAttachments;

  /**
   * Maximum number of output resources
   * can be used in fragment shader stage
   */
  u32 maxFragmentCombinedOutputResources;

  /**
   * Maximum shared memory size available
   * in compute shader stages
   */
  u32 maxComputeSharedMemorySize;

  /**
   * Maximum workgroup count allowed
   * in compute shader
   */
  glm::uvec3 maxComputeWorkGroupCount;

  /**
   * Maximum number of workgroup invocations
   */
  u32 maxComputeWorkGroupInvocations;

  /**
   * Maximum workgroup size allowed
   * in compute shader
   */
  glm::uvec3 maxComputeWorkGroupSize;

  /**
   * Subpixel precision bits
   */
  u32 subPixelPrecisionBits;

  /**
   * Subtexel precision bits
   */
  u32 subTexelPrecisionBits;

  /**
   * Mip map precision bits
   */
  u32 mipmapPrecisionBits;

  /**
   * Maximum draw indexed index value
   */
  u32 maxDrawIndexedIndexValue;

  /**
   * Maximum draw indirect count
   */
  u32 maxDrawIndirectCount;

  /**
   * Maximum sampler LOD bias
   */
  f32 maxSamplerLodBias;

  /**
   * Maximum sampler anisotropy
   */
  f32 maxSamplerAnisotropy;

  /**
   * Maximum number of viewpoer
   */
  u32 maxViewports;

  /**
   * Maximum viewport dimension
   */
  glm::uvec2 maxViewportDimensions;

  /**
   * Viewport bounds range
   */
  glm::vec2 viewportBoundsRange;

  /**
   * Viewport subpixel bits
   */
  u32 viewportSubPixelBits;

  /**
   * Minimum memory map alignment
   */
  usize minMemoryMapAlignment;

  /**
   * Minimum texel buffer offset alignment
   */
  u64 minTexelBufferOffsetAlignment;

  /**
   * Minimum uniform buffer offset alignment
   */
  u64 minUniformBufferOffsetAlignment;

  /**
   * Minimum storage buffer offset alignment
   */
  u64 minStorageBufferOffsetAlignment;

  /**
   * Minimum texel offset
   */
  i32 minTexelOffset;

  /**
   * Maximum texel offset
   */
  u32 maxTexelOffset;

  /**
   * Minimum texel gather offset
   */
  i32 minTexelGatherOffset;

  /**
   * Maximum texel gather offset
   */
  u32 maxTexelGatherOffset;

  /**
   * Minimum interpolation offset
   */
  f32 minInterpolationOffset;

  /**
   * Maximum interpolation offset
   */
  f32 maxInterpolationOffset;

  /**
   * Subpixel interpolation offset bits
   */
  u32 subPixelInterpolationOffsetBits;

  /**
   * Maximum framebuffer width
   */
  u32 maxFramebufferWidth;

  /**
   * Maximum framebuffer height
   */
  u32 maxFramebufferHeight;

  /**
   * Maximum framebuffer layers
   */
  u32 maxFramebufferLayers;

  /**
   * Framebuffer color sample counts
   */
  u32 framebufferColorSampleCounts;

  /**
   * Framebuffer depth sample counts
   */
  u32 framebufferDepthSampleCounts;

  /**
   * Framebuffer stencil sample counts
   */
  u32 framebufferStencilSampleCounts;

  /**
   * Maximum color attachments
   */
  u32 maxColorAttachments;

  /**
   * Sampled image color sample counts
   */
  u32 sampledImageColorSampleCounts;

  /**
   * Sampled image integer sample counts
   */
  u32 sampledImageIntegerSampleCounts;

  /**
   * Sampled image depth sample counts
   */
  u32 sampledImageDepthSampleCounts;

  /**
   * Sampled image stencil sample counts
   */
  u32 sampledImageStencilSampleCounts;

  /**
   * Storage image sample counts
   */
  u32 storageImageSampleCounts;

  /**
   * Max sample mask words
   */
  u32 maxSampleMaskWords;

  /**
   * Timestamp compute and graphics
   */
  bool timestampComputeAndGraphics = false;

  /**
   * Number of nanoseconds required for a
   * timestamp query to be incremented
   */
  f32 timestampPeriod;

  /**
   * Maximum clip distances that can be used
   * in shader stages
   */
  u32 maxClipDistances;

  /**
   * Maximum cull distances that can be used
   * in shader stages
   */
  u32 maxCullDistances;

  /**
   * Maximum clip and cull distances that can
   * be used in shader stages
   */
  u32 maxCombinedClipAndCullDistances;

  /**
   * Discrete queue priorities that can be
   * assigned for each queue family
   */
  u32 discreteQueuePriorities;

  /**
   * Point size range
   */
  glm::vec2 pointSizeRange;

  /**
   * Line width range
   */
  glm::vec2 lineWidthRange;

  /**
   * Point size granularity
   */
  f32 pointSizeGranularity;

  /**
   * Line width granularity
   */
  f32 lineWidthGranularity;

  /**
   * Strict lines supported
   */
  u32 strictLines;

  /**
   * Standard sample location
   */
  u32 standardSampleLocations;

  /**
   * Optimal buffer copy offset alignment
   */
  u64 optimalBufferCopyOffsetAlignment;

  /**
   * Optimal buffer copy row pitch alignment
   */
  u64 optimalBufferCopyRowPitchAlignment;

  /**
   * Non coherent atom size
   */
  u64 nonCoherentAtomSize;
};

} // namespace quoll::rhi
