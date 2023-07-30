#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/rhi/RenderCommandList.h"

#include "RenderGraphSyncDependency.h"
#include "RenderGraph.h"

namespace liquid {

EnableBitwiseEnum(GraphDirty);

RenderGraph::RenderGraph(StringView name) : mName(name) {
  LOG_DEBUG("Render graph initialized: " << name);
}

RenderGraphPass &RenderGraph::addGraphicsPass(StringView name) {
  mPasses.push_back({name, RenderGraphPassType::Graphics});
  return mPasses.back();
}

RenderGraphPass &RenderGraph::addComputePass(StringView name) {
  mPasses.push_back({name, RenderGraphPassType::Compute});
  return mPasses.back();
}

RenderGraph::RGTexture
RenderGraph::create(const rhi::TextureDescription &description) {
  auto textureIndex = mRegistry.allocate<rhi::TextureHandle>(description);
  return RGTexture(mRegistry, textureIndex);
}

RenderGraph::RGTexture RenderGraph::createView(RGTexture texture,
                                               uint32_t baseMipLevel,
                                               uint32_t mipLevelCount,
                                               uint32_t baseLayer,
                                               uint32_t layerCount) {
  RGTextureViewDescription description{};
  description.baseMipLevel = baseMipLevel;
  description.mipLevelCount = mipLevelCount;
  description.baseLayer = baseLayer;
  description.layerCount = layerCount;
  description.textureIndex = texture.getIndex();

  auto textureIndex = mRegistry.allocate<rhi::TextureHandle>(description);

  return RGTexture(mRegistry, textureIndex);
}

RenderGraph::RGTexture RenderGraph::import(rhi::TextureHandle handle) {
  auto textureIndex = mRegistry.allocate(handle);

  return RGTexture(mRegistry, textureIndex);
}

/**
 * @brief Topologically sort a graph
 *
 * @param index Index of current item
 * @param visited Visited nodes
 * @param adjacencyList Adjacency list
 * @param output Output array
 */
static void topologicalSort(const std::vector<RenderGraphPass> &inputs,
                            size_t index, std::vector<bool> &visited,
                            const std::vector<std::set<size_t>> &adjacencyList,
                            std::vector<RenderGraphPass> &output) {
  visited.at(index) = true;

  for (size_t x : adjacencyList.at(index)) {
    if (!visited.at(x)) {
      topologicalSort(inputs, x, visited, adjacencyList, output);
    }
  }

  output.push_back(inputs.at(index));
}

void RenderGraph::buildResources(RenderStorage &storage) {
  // Create all real handles for render graph if they do not exist
  const auto &textures = mRegistry.getRealResources<rhi::TextureHandle>();
  for (size_t i = 0; i < textures.size(); ++i) {
    if (textures.at(i) != rhi::TextureHandle::Null) {
      continue;
    }

    mRegistry.set(i, storage.getNewTextureHandle());
  }

  auto *device = storage.getDevice();

  for (size_t i = 0; i < textures.size(); ++i) {
    auto handle = mRegistry.get<rhi::TextureHandle>(i);
    const auto &desc = mRegistry.getDescription<rhi::TextureHandle>(i);

    if (const auto *textureDesc = std::get_if<rhi::TextureDescription>(&desc)) {
      device->createTexture(*textureDesc, handle);
    } else if (const auto *viewDesc =
                   std::get_if<RGTextureViewDescription>(&desc)) {
      rhi::TextureViewDescription description{};
      description.baseMipLevel = viewDesc->baseMipLevel;
      description.mipLevelCount = viewDesc->mipLevelCount;
      description.baseLayer = viewDesc->baseLayer;
      description.layerCount = viewDesc->layerCount;
      description.texture =
          mRegistry.get<rhi::TextureHandle>(viewDesc->textureIndex);
      device->createTextureView(description, handle);
    }

    mRegistry.callResourceReady<rhi::TextureHandle>(i, storage);
  }
}

void RenderGraph::compile() {
  LIQUID_PROFILE_EVENT("RenderGraph::compile");
  std::vector<size_t> passIndices;
  passIndices.reserve(mPasses.size());

  // Validate pass names
  std::set<String> uniquePasses;
  for (auto &x : mPasses) {
    uniquePasses.insert(x.getName());
  }
  LIQUID_ASSERT(
      uniquePasses.size() == mPasses.size(),
      "Some of the names in the render graph are used in more than one pass");

  if (uniquePasses.size() != mPasses.size()) {
    Engine::getLogger().error()
        << "Some of the names in the render "
           "graph are used in more than one pass (Graph: "
        << mName << ")";

    return;
  }

  // Delete lonely nodes
  for (auto i = 0; i < mPasses.size(); ++i) {
    auto &pass = mPasses.at(i);
    if (pass.getTextureInputs().empty() && pass.getTextureOutputs().empty() &&
        pass.getBufferInputs().empty() && pass.getBufferOutputs().empty()) {
      LOG_DEBUG("Pass is ignored during compilation because it has no inputs, "
                "nor outputs: "
                << pass.getName() << " (Graph: " << mName << ")");
    } else {
      passIndices.push_back(i);
    }
  }

  // Cache reads so we can easily access them
  // for creating the adjacency lsit
  std::unordered_map<rhi::TextureHandle, std::vector<size_t>> passTextureReads;
  std::unordered_map<rhi::BufferHandle, std::vector<size_t>> passBufferReads;
  for (size_t i = 0; i < passIndices.size(); ++i) {
    auto &pass = mPasses.at(passIndices.at(i));
    for (auto &resourceId : pass.getTextureInputs()) {
      passTextureReads[resourceId.texture].push_back(i);
    }

    for (auto &resourceId : pass.getBufferInputs()) {
      passBufferReads[resourceId.buffer].push_back(i);
    }
  }

  // Create adjacency list from inputs and outputs
  // to determine the edges of the graph
  std::vector<std::set<size_t>> adjacencyList;
  adjacencyList.resize(passIndices.size());

  for (size_t i = 0; i < passIndices.size(); ++i) {
    auto &pass = mPasses.at(passIndices.at(i));
    for (auto resourceId : pass.getTextureOutputs()) {
      if (passTextureReads.find(resourceId.texture) != passTextureReads.end()) {
        for (auto read : passTextureReads.at(resourceId.texture)) {
          adjacencyList.at(i).insert(read);
        }
      }
    }

    for (auto resourceId : pass.getBufferOutputs()) {
      if (passBufferReads.find(resourceId.buffer) != passBufferReads.end()) {
        for (auto read : passBufferReads.at(resourceId.buffer)) {
          adjacencyList.at(i).insert(read);
        }
      }
    }
  }

  // Topological sort based on DFS
  std::vector<RenderGraphPass> compiledPasses;
  compiledPasses.reserve(passIndices.size());
  std::vector<bool> visited(passIndices.size(), false);

  for (size_t i = passIndices.size(); i-- > 0;) {
    if (!visited.at(i)) {
      topologicalSort(mPasses, i, visited, adjacencyList, compiledPasses);
    }
  }

  std::reverse(compiledPasses.begin(), compiledPasses.end());
  mCompiledPasses = std::move(compiledPasses);
}

void RenderGraph::buildBarriers() {
  LIQUID_PROFILE_EVENT("RenderGraph::buildBarriers");

  std::unordered_map<rhi::TextureHandle, rhi::ImageLayout>
      textureAttachmentLayouts;
  std::unordered_map<rhi::TextureHandle, RenderGraphTextureSyncDependency>
      textureDependencies;
  std::unordered_map<rhi::BufferHandle, RenderGraphBufferSyncDependency>
      bufferDependencies;

  for (auto &pass : mCompiledPasses) {
    std::vector<rhi::ImageBarrier> imageBarriers{};
    std::vector<rhi::BufferBarrier> bufferBarriers{};

    for (size_t index = 0; index < pass.getTextureOutputs().size(); ++index) {
      auto &output = pass.getTextureOutputs().at(index);
      auto handle = output.texture.getHandle();

      auto newDependency = RenderGraphSyncDependency::getTextureWrite(
          pass.getType(), pass.getAttachments().at(index).type);

      const auto &description = mRegistry.getDescription<rhi::TextureHandle>(
          output.texture.getIndex());

      uint32_t baseMipLevel = 0;
      uint32_t mipLevelCount = 1;

      if (const auto *textureDescription =
              std::get_if<rhi::TextureDescription>(&description)) {
        mipLevelCount = textureDescription->mipLevelCount;
      } else if (const auto *viewDescription =
                     std::get_if<RGTextureViewDescription>(&description)) {
        mipLevelCount = viewDescription->mipLevelCount;
        baseMipLevel = viewDescription->baseMipLevel;
      }

      rhi::ImageBarrier imageBarrier{};
      imageBarrier.texture = handle;
      imageBarrier.dstAccess = newDependency.access;
      imageBarrier.dstLayout = newDependency.layout;
      imageBarrier.baseLevel = baseMipLevel;
      imageBarrier.levelCount = mipLevelCount;
      imageBarrier.dstStage = newDependency.stage;

      auto it = textureDependencies.find(handle);
      if (it == textureDependencies.end()) {
        imageBarrier.srcStage = rhi::PipelineStage::None;
        imageBarrier.srcAccess = rhi::Access::None;
        imageBarrier.srcLayout = rhi::ImageLayout::Undefined;
      } else {
        auto oldDependency = it->second;

        imageBarrier.srcStage = oldDependency.stage;
        imageBarrier.srcAccess = oldDependency.access;
        imageBarrier.srcLayout = oldDependency.layout;
      }

      imageBarriers.push_back(imageBarrier);
      textureDependencies.insert_or_assign(handle, newDependency);
    }

    for (size_t index = 0; index < pass.getTextureInputs().size(); ++index) {
      auto &input = pass.getTextureInputs().at(index);
      auto handle = input.texture.getHandle();

      auto newDependency =
          RenderGraphSyncDependency::getTextureRead(pass.getType());

      LIQUID_ASSERT(textureDependencies.find(handle) !=
                        textureDependencies.end(),
                    "Cannot read from unwritten texture");

      auto oldDependency = textureDependencies.at(handle);

      rhi::ImageBarrier imageBarrier{};
      imageBarrier.texture = handle;
      imageBarrier.srcAccess = oldDependency.access;
      imageBarrier.dstAccess = newDependency.access;
      imageBarrier.srcLayout = oldDependency.layout;
      imageBarrier.dstLayout = newDependency.layout;
      imageBarrier.srcStage = oldDependency.stage;
      imageBarrier.dstStage = newDependency.stage;
      imageBarriers.push_back(imageBarrier);

      textureDependencies.insert_or_assign(handle, newDependency);
    }

    for (auto &output : pass.getBufferOutputs()) {
      auto handle = output.buffer;

      auto newDependency =
          RenderGraphSyncDependency::getBufferWrite(pass.getType());

      rhi::BufferBarrier bufferBarrier{};
      bufferBarrier.buffer = handle;
      bufferBarrier.dstAccess = newDependency.access;
      bufferBarrier.dstStage = newDependency.stage;

      auto it = bufferDependencies.find(handle);
      if (it == bufferDependencies.end()) {
        bufferBarrier.srcStage = rhi::PipelineStage::None;
        bufferBarrier.srcAccess = rhi::Access::None;
      } else {
        auto oldDependency = it->second;

        bufferBarrier.srcStage = oldDependency.stage;
        bufferBarrier.srcAccess = oldDependency.access;
      }

      bufferBarriers.push_back(bufferBarrier);
      bufferDependencies.insert_or_assign(handle, newDependency);
    }

    for (auto &input : pass.getBufferInputs()) {
      auto handle = input.buffer;

      auto newDependency =
          RenderGraphSyncDependency::getBufferRead(pass.getType(), input.usage);

      auto oldDependency = bufferDependencies.at(handle);

      rhi::BufferBarrier bufferBarrier{};
      bufferBarrier.buffer = handle;
      bufferBarrier.srcAccess = oldDependency.access;
      bufferBarrier.dstAccess = newDependency.access;
      bufferBarrier.srcStage = oldDependency.stage;
      bufferBarrier.dstStage = newDependency.stage;
      bufferBarriers.push_back(bufferBarrier);

      bufferDependencies.insert_or_assign(handle, newDependency);
    }

    pass.mDependencies.imageBarriers = imageBarriers;
    pass.mDependencies.bufferBarriers = bufferBarriers;

    // Attachments
    for (size_t i = 0; i < pass.mTextureOutputs.size(); ++i) {
      auto &output = pass.mTextureOutputs.at(i);
      auto &attachment = pass.mAttachments.at(i);
      if (textureAttachmentLayouts.find(output.texture) ==
          textureAttachmentLayouts.end()) {
        output.srcLayout = rhi::ImageLayout::Undefined;
        attachment.loadOp = rhi::AttachmentLoadOp::Clear;
      } else {
        output.srcLayout = textureAttachmentLayouts.at(output.texture);
        attachment.loadOp = rhi::AttachmentLoadOp::Load;
      }

      attachment.storeOp = rhi::AttachmentStoreOp::Store;

      if (attachment.type == AttachmentType::Color ||
          attachment.type == AttachmentType::Resolve) {
        output.dstLayout = rhi::ImageLayout::ColorAttachmentOptimal;
      } else if (attachment.type == AttachmentType::Depth) {
        output.dstLayout = rhi::ImageLayout::DepthStencilAttachmentOptimal;
      }

      textureAttachmentLayouts.insert_or_assign(output.texture,
                                                output.dstLayout);
    }
  }
}

void RenderGraph::buildPasses(RenderStorage &storage) {
  LIQUID_PROFILE_EVENT("RenderGraph::buildPasses");

  for (auto &pass : mCompiledPasses) {
    if (pass.getType() == RenderGraphPassType::Compute) {
      buildComputePass(pass, storage);
    } else {
      buildGraphicsPass(pass, storage);
    }
  }
}

void RenderGraph::buildGraphicsPass(RenderGraphPass &pass,
                                    RenderStorage &storage) {
  LIQUID_PROFILE_EVENT("RenderGraph::buildGraphicsPass");
  auto *device = storage.getDevice();

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layerCount = 0;
  uint32_t sampleCount = 0;

  std::vector<rhi::TextureHandle> framebufferAttachments;

  rhi::RenderPassDescription renderPassDesc{};
  renderPassDesc.bindPoint = rhi::PipelineBindPoint::Graphics;
  renderPassDesc.debugName = pass.getName();

  for (size_t i = 0; i < pass.getTextureOutputs().size(); ++i) {
    auto &output = pass.mTextureOutputs.at(i);
    sampleCount = std::max(
        device->getTextureDescription(output.texture).samples, sampleCount);

    const auto &attachment = pass.getAttachments().at(i);

    const auto &desc = device->getTextureDescription(output.texture);

    framebufferAttachments.push_back(output.texture);

    rhi::RenderPassAttachmentDescription rpAttachmentDesc{};
    rpAttachmentDesc.texture = output.texture;
    rpAttachmentDesc.clearValue = attachment.clearValue;
    rpAttachmentDesc.initialLayout = output.srcLayout;
    rpAttachmentDesc.layout = output.dstLayout;

    rpAttachmentDesc.loadOp = attachment.loadOp;
    rpAttachmentDesc.storeOp = attachment.storeOp;
    if (BitwiseEnumContains(desc.usage, rhi::TextureUsage::Stencil)) {
      rpAttachmentDesc.stencilLoadOp = attachment.loadOp;
      rpAttachmentDesc.stencilStoreOp = attachment.storeOp;
    }

    if (attachment.type == AttachmentType::Resolve) {
      renderPassDesc.resolveAttachment.emplace(rpAttachmentDesc);
    } else if (attachment.type == AttachmentType::Depth) {
      renderPassDesc.depthAttachment.emplace(rpAttachmentDesc);
    } else {
      renderPassDesc.colorAttachments.push_back(rpAttachmentDesc);
    }

    width = desc.width;
    height = desc.height;
    layerCount = desc.layerCount;
  }

  bool renderPassExists = isHandleValid(pass.mRenderPass);

  if (!rhi::isHandleValid(pass.mRenderPass)) {
    pass.mRenderPass = storage.getNewRenderPassHandle();
  }

  device->createRenderPass(renderPassDesc, pass.mRenderPass);

  rhi::FramebufferDescription framebufferDesc;
  framebufferDesc.width = width;
  framebufferDesc.height = height;
  framebufferDesc.layers = layerCount;
  framebufferDesc.attachments = framebufferAttachments;
  framebufferDesc.renderPass = pass.mRenderPass;
  framebufferDesc.debugName = pass.getName();

  if (!rhi::isHandleValid(pass.mFramebuffer)) {
    pass.mFramebuffer = storage.getNewFramebufferHandle();
  }
  device->createFramebuffer(framebufferDesc, pass.mFramebuffer);

  pass.mDimensions.x = width;
  pass.mDimensions.y = height;
  pass.mDimensions.z = layerCount;

  for (auto handle : pass.mPipelines) {
    auto &description = storage.getGraphicsPipelineDescription(handle);
    description.renderPass = pass.mRenderPass;
    description.multisample.sampleCount = sampleCount;

    if (device->hasPipeline(handle)) {
      device->destroyPipeline(handle);
    }

    device->createPipeline(description, handle);
  }
}

void RenderGraph::buildComputePass(RenderGraphPass &pass,
                                   RenderStorage &storage) {
  LIQUID_PROFILE_EVENT("buildComputePass");
  auto *device = storage.getDevice();

  for (auto handle : pass.mPipelines) {
    auto &description = storage.getComputePipelineDescription(handle);

    if (device->hasPipeline(handle)) {
      device->destroyPipeline(handle);
    }

    device->createPipeline(description, handle);
  }
}

void RenderGraph::execute(rhi::RenderCommandList &commandList,
                          uint32_t frameIndex) {
  LIQUID_PROFILE_EVENT("RenderGraph::execute");

  for (auto &pass : mCompiledPasses) {
    commandList.pipelineBarrier(pass.mDependencies.memoryBarriers,
                                pass.mDependencies.imageBarriers,
                                pass.mDependencies.bufferBarriers);

    if (pass.getType() == RenderGraphPassType::Compute) {
      pass.execute(commandList, frameIndex);
    } else {
      commandList.beginRenderPass(pass.mRenderPass, pass.getFramebuffer(),
                                  {0, 0}, glm::uvec2(pass.getDimensions()));
      commandList.setViewport({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()),
                              {0.0f, 1.0f});
      commandList.setScissor({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()));
      pass.execute(commandList, frameIndex);
      commandList.endRenderPass();
    }
  }
}

void RenderGraph::build(RenderStorage &storage) {
  buildResources(storage);

  compile();
  buildBarriers();
  buildPasses(storage);

  LOG_DEBUG("Render graph built: " << mName);
}

void RenderGraph::destroy(RenderStorage &storage) {
  for (auto &pass : mCompiledPasses) {
    for (auto pipeline : pass.getPipelines()) {
      storage.getDevice()->destroyPipeline(pipeline);
    }

    if (rhi::isHandleValid(pass.getFramebuffer())) {
      storage.getDevice()->destroyFramebuffer(pass.getFramebuffer());
    }

    if (rhi::isHandleValid(pass.getRenderPass())) {
      storage.getDevice()->destroyRenderPass(pass.getRenderPass());
    }
  }

  for (size_t index = 0;
       index < mRegistry.getRealResources<rhi::TextureHandle>().size();
       ++index) {
    if (mRegistry.getResourceState<rhi::TextureHandle>(index) ==
        RGResourceState::Transient) {
      storage.destroyTexture(mRegistry.get<rhi::TextureHandle>(index));
    }
  }
}

} // namespace liquid
