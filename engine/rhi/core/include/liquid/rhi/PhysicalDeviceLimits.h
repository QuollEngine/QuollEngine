#pragma once

namespace liquid::rhi {

/**
 * @brief Physical device limits
 */
struct PhysicalDeviceLimits {
  /**
   * Maximum 1D image dimensions
   */
  uint32_t maxImageDimension1D;

  /**
   * Maximum 2D image dimensions
   */
  uint32_t maxImageDimension2D;

  /**
   * Maximum 3D image dimensions
   */
  uint32_t maxImageDimension3D;

  /**
   * Maximum cube image dimensions
   */
  uint32_t maxImageDimensionCube;

  /**
   * Maximum image array layers
   */
  uint32_t maxImageArrayLayers;

  /**
   * Maximum texel buffer elements
   */
  uint32_t maxTexelBufferElements;

  /**
   * Maximum uniform buffer range
   */
  uint32_t maxUniformBufferRange;

  /**
   * Maximum storage buffer range
   */

  uint32_t maxStorageBufferRange;

  /**
   * Maximum push constants size
   */

  uint32_t maxPushConstantsSize;

  /**
   * Maximum memory allocation count
   */
  uint32_t maxMemoryAllocationCount;

  /**
   * Maximum sampler allocation count
   */
  uint32_t maxSamplerAllocationCount;

  /**
   * Buffer image granularity
   */
  uint64_t bufferImageGranularity;

  /**
   * Sparse address space size
   */
  uint64_t sparseAddressSpaceSize;

  /**
   * Maximum bound descriptor sets
   */
  uint32_t maxBoundDescriptorSets;

  /**
   * Maximum number of samplers that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorSamplers;

  /**
   * Maximum number of uniform buffers that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorUniformBuffers;

  /**
   * Maximum number of storage buffers that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorStorageBuffers;

  /**
   * Maximum number of sampler images that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorSampledImages;

  /**
   * Maximum number of storage images that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorStorageImages;

  /**
   * Maximum number of input attachments that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageDescriptorInputAttachments;

  /**
   * Maximum number of resources that are
   * accessible in a single shader stage
   */
  uint32_t maxPerStageResources;

  /**
   * Maximum number of available
   * sampler descriptor sets
   */
  uint32_t maxDescriptorSetSamplers;

  /**
   * Maximum number of available
   * uniform buffer descriptor sets
   */
  uint32_t maxDescriptorSetUniformBuffers;

  /**
   * Maximum number of available dynamic
   * uniform buffer descriptor sets
   */
  uint32_t maxDescriptorSetUniformBuffersDynamic;

  /**
   * Maximum number of available
   * storage buffer descriptor sets
   */
  uint32_t maxDescriptorSetStorageBuffers;

  /**
   * Maximum number of available dynamic
   * storage buffer descriptor sets
   */
  uint32_t maxDescriptorSetStorageBuffersDynamic;

  /**
   * Maximum number of available
   * sampled image descriptor sets
   */
  uint32_t maxDescriptorSetSampledImages;

  /**
   * Maximum number of available
   * storage image descriptor sets
   */
  uint32_t maxDescriptorSetStorageImages;

  /**
   * Maximum number of available
   * input attachment descriptor sets
   */
  uint32_t maxDescriptorSetInputAttachments;

  /**
   * Maximum number of vertex
   * input attributes
   */
  uint32_t maxVertexInputAttributes;

  /**
   * Maximum number of vertex
   * input bindings
   */
  uint32_t maxVertexInputBindings;

  /**
   * Maximum vertex input attribute offset
   */
  uint32_t maxVertexInputAttributeOffset;

  /**
   * Maximum vertex input binding stride
   */
  uint32_t maxVertexInputBindingStride;

  /**
   * Maximum number of vertex output components
   */
  uint32_t maxVertexOutputComponents;

  /**
   * Maximum number of fragment input components
   */
  uint32_t maxFragmentInputComponents;

  /**
   * Maximum number of fragment output attachments
   */
  uint32_t maxFragmentOutputAttachments;

  /**
   * Maximum number of fragment dual source
   * attachments
   */
  uint32_t maxFragmentDualSrcAttachments;

  /**
   * Maximum number of output resources
   * can be used in fragment shader stage
   */
  uint32_t maxFragmentCombinedOutputResources;

  /**
   * Maximum shared memory size available
   * in compute shader stages
   */
  uint32_t maxComputeSharedMemorySize;

  /**
   * Maximum workgroup count allowed
   * in compute shader
   */
  glm::uvec3 maxComputeWorkGroupCount;

  /**
   * Maximum number of workgroup invocations
   */
  uint32_t maxComputeWorkGroupInvocations;

  /**
   * Maximum workgroup size allowed
   * in compute shader
   */
  glm::uvec3 maxComputeWorkGroupSize;

  /**
   * Subpixel precision bits
   */
  uint32_t subPixelPrecisionBits;

  /**
   * Subtexel precision bits
   */
  uint32_t subTexelPrecisionBits;

  /**
   * Mip map precision bits
   */
  uint32_t mipmapPrecisionBits;

  /**
   * Maximum draw indexed index value
   */
  uint32_t maxDrawIndexedIndexValue;

  /**
   * Maximum draw indirect count
   */
  uint32_t maxDrawIndirectCount;

  /**
   * Maximum sampler LOD bias
   */
  float maxSamplerLodBias;

  /**
   * Maximum sampler anisotropy
   */
  float maxSamplerAnisotropy;

  /**
   * Maximum number of viewpoer
   */
  uint32_t maxViewports;

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
  uint32_t viewportSubPixelBits;

  /**
   * Minimum memory map alignment
   */
  size_t minMemoryMapAlignment;

  /**
   * Minimum texel buffer offset alignment
   */
  uint64_t minTexelBufferOffsetAlignment;

  /**
   * Minimum uniform buffer offset alignment
   */
  uint64_t minUniformBufferOffsetAlignment;

  /**
   * Minimum storage buffer offset alignment
   */
  uint64_t minStorageBufferOffsetAlignment;

  /**
   * Minimum texel offset
   */
  int32_t minTexelOffset;

  /**
   * Maximum texel offset
   */
  uint32_t maxTexelOffset;

  /**
   * Minimum texel gather offset
   */
  int32_t minTexelGatherOffset;

  /**
   * Maximum texel gather offset
   */
  uint32_t maxTexelGatherOffset;

  /**
   * Minimum interpolation offset
   */
  float minInterpolationOffset;

  /**
   * Maximum interpolation offset
   */
  float maxInterpolationOffset;

  /**
   * Subpixel interpolation offset bits
   */
  uint32_t subPixelInterpolationOffsetBits;

  /**
   * Maximum framebuffer width
   */
  uint32_t maxFramebufferWidth;

  /**
   * Maximum framebuffer height
   */
  uint32_t maxFramebufferHeight;

  /**
   * Maximum framebuffer layers
   */
  uint32_t maxFramebufferLayers;

  /**
   * Framebuffer color sample counts
   */
  uint32_t framebufferColorSampleCounts;

  /**
   * Framebuffer depth sample counts
   */
  uint32_t framebufferDepthSampleCounts;

  /**
   * Framebuffer stencil sample counts
   */
  uint32_t framebufferStencilSampleCounts;

  /**
   * Maximum color attachments
   */
  uint32_t maxColorAttachments;

  /**
   * Sampled image color sample counts
   */
  uint32_t sampledImageColorSampleCounts;

  /**
   * Sampled image integer sample counts
   */
  uint32_t sampledImageIntegerSampleCounts;

  /**
   * Sampled image depth sample counts
   */
  uint32_t sampledImageDepthSampleCounts;

  /**
   * Sampled image stencil sample counts
   */
  uint32_t sampledImageStencilSampleCounts;

  /**
   * Storage image sample counts
   */
  uint32_t storageImageSampleCounts;

  /**
   * Max sample mask words
   */
  uint32_t maxSampleMaskWords;

  /**
   * Timestamp compute and graphics
   */
  bool timestampComputeAndGraphics = false;

  /**
   * Number of nanoseconds required for a
   * timestamp query to be incremented
   */
  float timestampPeriod;

  /**
   * Maximum clip distances that can be used
   * in shader stages
   */
  uint32_t maxClipDistances;

  /**
   * Maximum cull distances that can be used
   * in shader stages
   */
  uint32_t maxCullDistances;

  /**
   * Maximum clip and cull distances that can
   * be used in shader stages
   */
  uint32_t maxCombinedClipAndCullDistances;

  /**
   * Discrete queue priorities that can be
   * assigned for each queue family
   */
  uint32_t discreteQueuePriorities;

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
  float pointSizeGranularity;

  /**
   * Line width granularity
   */
  float lineWidthGranularity;

  /**
   * Strict lines supported
   */
  uint32_t strictLines;

  /**
   * Standard sample location
   */
  uint32_t standardSampleLocations;

  /**
   * Optimal buffer copy offset alignment
   */
  uint64_t optimalBufferCopyOffsetAlignment;

  /**
   * Optimal buffer copy row pitch alignment
   */
  uint64_t optimalBufferCopyRowPitchAlignment;

  /**
   * Non coherent atom size
   */
  uint64_t nonCoherentAtomSize;
};

} // namespace liquid::rhi
