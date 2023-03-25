#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/rhi/RenderCommandList.h"

#include "RenderGraph.h"

namespace liquid {

EnableBitwiseEnum(GraphDirty);

RenderGraph::RenderGraph(StringView name) : mName(name) {
  LOG_DEBUG("Render graph initialized: " << name);
}

RenderGraphPass &RenderGraph::addGraphicsPass(StringView name) {
  mDirty |= GraphDirty::PassChanges;

  mPasses.push_back({name, RenderGraphPassType::Graphics});
  return mPasses.back();
}

RenderGraphPass &RenderGraph::addComputePass(StringView name) {
  mDirty |= GraphDirty::PassChanges;

  mPasses.push_back({name, RenderGraphPassType::Compute});
  return mPasses.back();
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

void RenderGraph::compile(rhi::RenderDevice *device) {
  if (!BitwiseEnumContains(mDirty, GraphDirty::PassChanges))
    return;

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

  static constexpr rhi::PipelineStage StageFragmentTest =
      rhi::PipelineStage::EarlyFragmentTests |
      rhi::PipelineStage::LateFragmentTests;
  static constexpr auto StageColor = rhi::PipelineStage::ColorAttachmentOutput;
  static constexpr auto StageFragmentShader =
      rhi::PipelineStage::FragmentShader;

  // Determine attachments, image layouts, and barriers
  std::unordered_map<rhi::TextureHandle, rhi::ImageLayout> visitedOutputs;
  std::unordered_map<rhi::BufferHandle, rhi::PipelineStage> visitedBuffers;

  for (auto &pass : mCompiledPasses) {
    pass.mPreBarrier = RenderGraphPassBarrier{};
    pass.mPostBarrier = RenderGraphPassBarrier{};

    for (auto &input : pass.mTextureInputs) {
      LIQUID_ASSERT(visitedOutputs.find(input.texture) != visitedOutputs.end(),
                    "Pass is reading from an empty texture");

      input.srcLayout = visitedOutputs.at(input.texture);
      input.dstLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;

      rhi::PipelineStage otherStage{rhi::PipelineStage::None};
      rhi::Access otherAccess{rhi::Access::None};

      if (input.srcLayout == rhi::ImageLayout::DepthStencilAttachmentOptimal) {
        otherStage = StageFragmentTest;
        otherAccess = rhi::Access::DepthStencilAttachmentWrite;
      } else if (input.srcLayout == rhi::ImageLayout::ColorAttachmentOptimal) {
        otherStage = StageColor;
        otherAccess = rhi::Access::ColorAttachmentWrite;
      }

      rhi::ImageBarrier preImageBarrier{};
      preImageBarrier.srcLayout = input.srcLayout;
      preImageBarrier.dstLayout = input.dstLayout;
      preImageBarrier.texture = input.texture;
      preImageBarrier.srcAccess = otherAccess;
      preImageBarrier.dstAccess = rhi::Access::ShaderRead;

      rhi::ImageBarrier postImageBarrier{};
      postImageBarrier.srcLayout = input.dstLayout;
      postImageBarrier.dstLayout = input.srcLayout;
      postImageBarrier.texture = input.texture;
      postImageBarrier.srcAccess = rhi::Access::ShaderRead;
      postImageBarrier.dstAccess = otherAccess;

      pass.mPreBarrier.enabled = true;
      pass.mPreBarrier.srcStage |= otherStage;
      pass.mPreBarrier.dstStage |= StageFragmentShader;
      pass.mPreBarrier.imageBarriers.push_back(preImageBarrier);

      pass.mPostBarrier.enabled = true;
      pass.mPostBarrier.srcStage |= StageFragmentShader;
      pass.mPostBarrier.dstStage |= otherStage;
      pass.mPostBarrier.imageBarriers.push_back(postImageBarrier);
    }

    for (size_t i = 0; i < pass.mTextureOutputs.size(); ++i) {
      auto &output = pass.mTextureOutputs.at(i);
      auto &attachment = pass.mAttachments.at(i);
      if (visitedOutputs.find(output.texture) == visitedOutputs.end()) {
        output.srcLayout = rhi::ImageLayout::Undefined;
        attachment.loadOp = rhi::AttachmentLoadOp::Clear;
      } else {
        output.srcLayout = visitedOutputs.at(output.texture);
        attachment.loadOp = rhi::AttachmentLoadOp::Load;
      }

      attachment.storeOp = rhi::AttachmentStoreOp::Store;

      rhi::PipelineStage stage{rhi::PipelineStage::None};
      rhi::Access srcAccess{rhi::Access::None};
      rhi::Access dstAccess{rhi::Access::None};

      if (attachment.type == AttachmentType::Color ||
          attachment.type == AttachmentType::Resolve) {
        output.dstLayout = rhi::ImageLayout::ColorAttachmentOptimal;
        stage = StageColor;
        srcAccess = rhi::Access::ColorAttachmentWrite;
        dstAccess = srcAccess | rhi::Access::ColorAttachmentRead;
      } else if (attachment.type == AttachmentType::Depth) {
        output.dstLayout = rhi::ImageLayout::DepthStencilAttachmentOptimal;
        stage = StageFragmentTest;
        srcAccess = rhi::Access::DepthStencilAttachmentWrite;
        dstAccess = srcAccess | rhi::Access::DepthStencilAttachmentRead;
      }

      rhi::MemoryBarrier memoryBarrier{};
      memoryBarrier.srcAccess = srcAccess;
      memoryBarrier.dstAccess = dstAccess;

      pass.mPostBarrier.enabled = true;
      pass.mPostBarrier.srcStage |= stage;
      pass.mPostBarrier.dstStage |= stage;
      pass.mPostBarrier.memoryBarriers.push_back(memoryBarrier);

      visitedOutputs.insert_or_assign(output.texture, output.dstLayout);
    }

    for (auto &input : pass.mBufferInputs) {
      LIQUID_ASSERT(visitedBuffers.find(input.buffer) != visitedBuffers.end(),
                    "Pass is reading from an empty buffer");

      rhi::PipelineStage stage{rhi::PipelineStage::None};
      rhi::Access srcAccess{rhi::Access::None};
      rhi::Access dstAccess{rhi::Access::None};

      if ((input.usage & rhi::BufferUsage::Vertex) ==
          rhi::BufferUsage::Vertex) {
        stage |= rhi::PipelineStage::VertexInput;
        srcAccess = rhi::Access::ShaderWrite;
        dstAccess |= rhi::Access::VertexAttributeRead;
      }

      if ((input.usage & rhi::BufferUsage::Index) == rhi::BufferUsage::Index) {
        stage |= rhi::PipelineStage::VertexInput;
        srcAccess |= rhi::Access::ShaderWrite;
        dstAccess |= rhi::Access::IndexRead;
      }

      if ((input.usage & rhi::BufferUsage::Indirect) ==
          rhi::BufferUsage::Indirect) {
        stage |= rhi::PipelineStage::DrawIndirect;
        srcAccess |= rhi::Access::ShaderWrite;
        dstAccess |= rhi::Access::IndirectCommandRead;
      }

      if (((input.usage & rhi::BufferUsage::Storage) ==
           rhi::BufferUsage::Storage) ||
          ((input.usage & rhi::BufferUsage::Uniform) ==
           rhi::BufferUsage::Uniform)) {
        if (pass.getType() == RenderGraphPassType::Compute) {
          stage |= rhi::PipelineStage::ComputeShader;
        } else {
          stage |= rhi::PipelineStage::FragmentShader;
        }
        srcAccess |= rhi::Access::ShaderWrite;
        dstAccess |= rhi::Access::ShaderRead;
      }

      rhi::MemoryBarrier memoryBarrier{};
      memoryBarrier.srcAccess = srcAccess;
      memoryBarrier.dstAccess = dstAccess;

      pass.mPreBarrier.enabled = true;
      pass.mPreBarrier.srcStage |= visitedBuffers.at(input.buffer);
      pass.mPreBarrier.dstStage |= stage;
      pass.mPreBarrier.memoryBarriers.push_back(memoryBarrier);
    }

    for (auto &output : pass.mBufferOutputs) {
      rhi::PipelineStage stage{rhi::PipelineStage::None};
      rhi::Access srcAccess{rhi::Access::None};
      rhi::Access dstAccess{rhi::Access::None};

      if (((output.usage & rhi::BufferUsage::Storage) ==
           rhi::BufferUsage::Storage) ||
          ((output.usage & rhi::BufferUsage::Uniform) ==
           rhi::BufferUsage::Uniform)) {
        if (pass.getType() == RenderGraphPassType::Compute) {
          stage |= rhi::PipelineStage::ComputeShader;
        } else {
          stage |= rhi::PipelineStage::FragmentShader;
        }
        srcAccess |= rhi::Access::ShaderWrite;
        dstAccess |= rhi::Access::ShaderRead;
      }

      visitedBuffers.insert_or_assign(output.buffer, stage);

      rhi::MemoryBarrier memoryBarrier{};
      memoryBarrier.srcAccess = srcAccess;
      memoryBarrier.dstAccess = dstAccess;

      pass.mPostBarrier.enabled = true;
      pass.mPostBarrier.srcStage |= stage;
      pass.mPostBarrier.dstStage |= stage;
      pass.mPostBarrier.memoryBarriers.push_back(memoryBarrier);
    }
  }

  LOG_DEBUG("Render graph compiled: " << mName);
}

void RenderGraph::setFramebufferExtent(glm::uvec2 framebufferExtent) {
  mFramebufferExtent = framebufferExtent;
  mDirty |= GraphDirty::SizeUpdate;
}

void RenderGraph::updateDirtyFlag() { mDirty = GraphDirty::None; }

} // namespace liquid
