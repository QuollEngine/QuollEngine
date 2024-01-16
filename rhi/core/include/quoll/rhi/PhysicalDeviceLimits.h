#pragma once

namespace quoll::rhi {

struct PhysicalDeviceLimits {
  u32 maxImageDimension1D;

  u32 maxImageDimension2D;

  u32 maxImageDimension3D;

  u32 maxImageDimensionCube;

  u32 maxImageArrayLayers;

  u32 maxTexelBufferElements;

  u32 maxUniformBufferRange;

  u32 maxStorageBufferRange;

  u32 maxPushConstantsSize;

  u32 maxMemoryAllocationCount;

  u32 maxSamplerAllocationCount;

  u64 bufferImageGranularity;

  u64 sparseAddressSpaceSize;

  u32 maxBoundDescriptorSets;

  u32 maxPerStageDescriptorSamplers;

  u32 maxPerStageDescriptorUniformBuffers;

  u32 maxPerStageDescriptorStorageBuffers;

  u32 maxPerStageDescriptorSampledImages;

  u32 maxPerStageDescriptorStorageImages;

  u32 maxPerStageDescriptorInputAttachments;

  u32 maxPerStageResources;

  u32 maxDescriptorSetSamplers;

  u32 maxDescriptorSetUniformBuffers;

  u32 maxDescriptorSetUniformBuffersDynamic;

  u32 maxDescriptorSetStorageBuffers;

  u32 maxDescriptorSetStorageBuffersDynamic;

  u32 maxDescriptorSetSampledImages;

  u32 maxDescriptorSetStorageImages;

  u32 maxDescriptorSetInputAttachments;

  u32 maxVertexInputAttributes;

  u32 maxVertexInputBindings;

  u32 maxVertexInputAttributeOffset;

  u32 maxVertexInputBindingStride;

  u32 maxVertexOutputComponents;

  u32 maxFragmentInputComponents;

  u32 maxFragmentOutputAttachments;

  u32 maxFragmentDualSrcAttachments;

  u32 maxFragmentCombinedOutputResources;

  u32 maxComputeSharedMemorySize;

  glm::uvec3 maxComputeWorkGroupCount;

  u32 maxComputeWorkGroupInvocations;

  glm::uvec3 maxComputeWorkGroupSize;

  u32 subPixelPrecisionBits;

  u32 subTexelPrecisionBits;

  u32 mipmapPrecisionBits;

  u32 maxDrawIndexedIndexValue;

  u32 maxDrawIndirectCount;

  f32 maxSamplerLodBias;

  f32 maxSamplerAnisotropy;

  u32 maxViewports;

  glm::uvec2 maxViewportDimensions;

  glm::vec2 viewportBoundsRange;

  u32 viewportSubPixelBits;

  usize minMemoryMapAlignment;

  u64 minTexelBufferOffsetAlignment;

  u64 minUniformBufferOffsetAlignment;

  u64 minStorageBufferOffsetAlignment;

  i32 minTexelOffset;

  u32 maxTexelOffset;

  i32 minTexelGatherOffset;

  u32 maxTexelGatherOffset;

  f32 minInterpolationOffset;

  f32 maxInterpolationOffset;

  u32 subPixelInterpolationOffsetBits;

  u32 maxFramebufferWidth;

  u32 maxFramebufferHeight;

  u32 maxFramebufferLayers;

  u32 framebufferColorSampleCounts;

  u32 framebufferDepthSampleCounts;

  u32 framebufferStencilSampleCounts;

  u32 maxColorAttachments;

  u32 sampledImageColorSampleCounts;

  u32 sampledImageIntegerSampleCounts;

  u32 sampledImageDepthSampleCounts;

  u32 sampledImageStencilSampleCounts;

  u32 storageImageSampleCounts;

  u32 maxSampleMaskWords;

  bool timestampComputeAndGraphics = false;

  f32 timestampPeriod;

  u32 maxClipDistances;

  u32 maxCullDistances;

  u32 maxCombinedClipAndCullDistances;

  u32 discreteQueuePriorities;

  glm::vec2 pointSizeRange;

  glm::vec2 lineWidthRange;

  f32 pointSizeGranularity;

  f32 lineWidthGranularity;

  u32 strictLines;

  u32 standardSampleLocations;

  u64 optimalBufferCopyOffsetAlignment;

  u64 optimalBufferCopyRowPitchAlignment;

  u64 nonCoherentAtomSize;
};

} // namespace quoll::rhi
