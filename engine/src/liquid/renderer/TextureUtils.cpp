#include "liquid/core/Base.h"
#include "TextureUtils.h"

namespace liquid {

void TextureUtils::copyDataToTexture(
    rhi::RenderDevice *device, void *source, rhi::TextureHandle destination,
    rhi::ImageLayout destinationLayout, uint32_t destinationLayers,
    const std::vector<TextureAssetLevel> &destinationLevels) {

  rhi::BufferDescription stagingBufferDesc{};
  stagingBufferDesc.size = getBufferSizeFromLevels(destinationLevels);
  stagingBufferDesc.data = source;
  stagingBufferDesc.usage = rhi::BufferUsage::TransferSource;

  auto stagingBuffer = device->createBuffer(stagingBufferDesc);

  auto commandList = device->requestImmediateCommandList();

  rhi::ImageBarrier barrier{};
  barrier.baseLevel = 0;
  barrier.levelCount = static_cast<uint32_t>(destinationLevels.size());
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::TransferWrite;
  barrier.srcLayout = rhi::ImageLayout::Undefined;
  barrier.dstLayout = rhi::ImageLayout::TransferDestinationOptimal;
  barrier.texture = destination;

  commandList.pipelineBarrier(rhi::PipelineStage::PipeTop,
                              rhi::PipelineStage::Transfer, {}, {barrier});

  std::vector<rhi::CopyRegion> copies(destinationLevels.size());
  for (size_t i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &dstLevel = destinationLevels.at(i);

    copy.bufferOffset = static_cast<uint32_t>(dstLevel.offset);
    copy.imageBaseArrayLayer = 0;
    copy.imageLayerCount = destinationLayers;
    copy.imageExtent = {dstLevel.width, dstLevel.height, 1};
    copy.imageOffset = {0, 0, 0};
    copy.imageLevel = static_cast<uint32_t>(i);
  }

  commandList.copyBufferToTexture(stagingBuffer.getHandle(), destination,
                                  copies);

  barrier.srcLayout = rhi::ImageLayout::TransferDestinationOptimal;
  barrier.dstLayout = destinationLayout;
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::None;
  commandList.pipelineBarrier(rhi::PipelineStage::Transfer,
                              rhi::PipelineStage::PipeTop, {}, {barrier});

  device->submitImmediate(commandList);

  device->destroyBuffer(stagingBuffer.getHandle());
}

void TextureUtils::copyTextureToData(
    rhi::RenderDevice *device, rhi::TextureHandle source,
    rhi::ImageLayout sourceLayout, uint32_t sourceLayers,
    const std::vector<TextureAssetLevel> &sourceLevels, void *destination) {
  rhi::BufferDescription stagingBufferDesc{};
  stagingBufferDesc.size = getBufferSizeFromLevels(sourceLevels);
  stagingBufferDesc.data = nullptr;
  stagingBufferDesc.usage = rhi::BufferUsage::TransferDestination;

  auto stagingBuffer = device->createBuffer(stagingBufferDesc);

  auto commandList = device->requestImmediateCommandList();

  rhi::ImageBarrier barrier{};
  barrier.baseLevel = 0;
  barrier.levelCount = static_cast<uint32_t>(sourceLevels.size());
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::TransferRead;
  barrier.srcLayout = rhi::ImageLayout::Undefined;
  barrier.dstLayout = rhi::ImageLayout::TransferSourceOptimal;
  barrier.texture = source;

  commandList.pipelineBarrier(rhi::PipelineStage::PipeTop,
                              rhi::PipelineStage::Transfer, {}, {barrier});

  std::vector<rhi::CopyRegion> copies(sourceLevels.size());
  for (size_t i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &srcLevel = sourceLevels.at(i);

    copy.bufferOffset = static_cast<uint32_t>(srcLevel.offset);
    copy.imageBaseArrayLayer = 0;
    copy.imageLayerCount = sourceLayers;
    copy.imageExtent = {srcLevel.width, srcLevel.height, 1};
    copy.imageOffset = {0, 0, 0};
    copy.imageLevel = static_cast<uint32_t>(i);
  }

  commandList.copyTextureToBuffer(source, stagingBuffer.getHandle(), copies);

  barrier.srcLayout = rhi::ImageLayout::TransferSourceOptimal;
  barrier.dstLayout = sourceLayout;
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::None;
  commandList.pipelineBarrier(rhi::PipelineStage::Transfer,
                              rhi::PipelineStage::PipeTop, {}, {barrier});

  device->submitImmediate(commandList);

  void *data = stagingBuffer.map();
  memcpy(destination, data, stagingBufferDesc.size);
  stagingBuffer.unmap();

  device->destroyBuffer(stagingBuffer.getHandle());
}

void TextureUtils::generateMipMapsForTexture(rhi::RenderDevice *device,
                                             rhi::TextureHandle texture,
                                             rhi::ImageLayout layout,
                                             uint32_t layers, uint32_t levels,
                                             uint32_t width, uint32_t height) {
  auto commandList = device->requestImmediateCommandList();

  rhi::ImageBarrier barrier;
  barrier.texture = texture;
  barrier.baseLevel = 0;
  barrier.levelCount = levels;
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::TransferWrite;
  barrier.srcLayout = rhi::ImageLayout::Undefined;
  barrier.dstLayout = rhi::ImageLayout::TransferDestinationOptimal;

  commandList.pipelineBarrier(rhi::PipelineStage::Transfer,
                              rhi::PipelineStage::Transfer, {}, {barrier});

  barrier.srcAccess = rhi::Access::TransferWrite;
  barrier.dstAccess = rhi::Access::TransferRead;
  barrier.srcLayout = rhi::ImageLayout::TransferDestinationOptimal;
  barrier.dstLayout = rhi::ImageLayout::TransferSourceOptimal;
  barrier.levelCount = 1;

  uint32_t mipWidth = width;
  uint32_t mipHeight = height;

  for (uint32_t i = 1; i < levels; ++i) {
    barrier.baseLevel = i - 1;
    commandList.pipelineBarrier(rhi::PipelineStage::Transfer,
                                rhi::PipelineStage::Transfer, {}, {barrier});

    rhi::BlitRegion region{};
    region.srcOffsets.at(0) = {0, 0, 0};
    region.srcOffsets.at(1) = {mipWidth, mipHeight, 1};
    region.srcLayerCount = layers;
    region.srcMipLevel = i - 1;
    region.dstOffsets.at(0) = {0, 0, 0};
    region.dstOffsets.at(1) = {mipWidth > 1 ? mipWidth / 2 : 1,
                               mipHeight > 1 ? mipHeight / 2 : 1, 1};
    region.dstLayerCount = layers;
    region.dstMipLevel = i;

    commandList.blitTexture(texture, texture, {region}, rhi::Filter::Nearest);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1)
      mipHeight /= 2;
  }

  barrier.baseLevel = 0;
  barrier.levelCount = levels;
  barrier.srcLayout = rhi::ImageLayout::Undefined;
  barrier.dstLayout = layout;
  barrier.srcAccess = rhi::Access::None;
  barrier.dstAccess = rhi::Access::None;
  commandList.pipelineBarrier(rhi::PipelineStage::Transfer,
                              rhi::PipelineStage::PipeTop, {}, {barrier});

  device->submitImmediate(commandList);
}

size_t TextureUtils::getBufferSizeFromLevels(
    const std::vector<TextureAssetLevel> &levels) {
  size_t size = 0;
  for (auto &level : levels) {
    size += level.size;
  }

  return size;
}

} // namespace liquid
