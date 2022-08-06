#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "RenderCommandList.h"
#include "RenderGraph.h"

namespace liquid::rhi {

RenderGraphPass &RenderGraph::addPass(StringView name) {
  mPasses.push_back(name);
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
                            const std::vector<std::list<size_t>> &adjacencyList,
                            std::vector<RenderGraphPass> &output) {
  visited.at(index) = true;

  for (size_t x : adjacencyList.at(index)) {
    if (!visited.at(x)) {
      topologicalSort(inputs, x, visited, adjacencyList, output);
    }
  }

  output.push_back(inputs.at(index));
}

void RenderGraph::compile(ResourceRegistry &resourceRegistry) {
  if (!mDirty)
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
    engineLogger.log(Logger::Warning) << "Some of the names in the render "
                                         "graph are used in more than one pass";
  }

  // Delete lonely nodes
  for (auto i = 0; i < mPasses.size(); ++i) {
    auto &pass = mPasses.at(i);
    if (pass.getInputs().size() == 0 && pass.getOutputs().size() == 0) {
      LOG_DEBUG("Pass is ignored during compilation because it has no inputs, "
                "nor outputs: "
                << pass.getName());
    } else {
      passIndices.push_back(i);
    }
  }

  // Cache reads so we can easily access them
  // for creating the adjacency lsit
  std::unordered_map<rhi::TextureHandle, std::vector<size_t>> passReads;
  for (size_t i = 0; i < passIndices.size(); ++i) {
    auto &pass = mPasses.at(passIndices.at(i));
    for (auto &resourceId : pass.getInputs()) {
      passReads[resourceId.texture].push_back(i);
    }
  }

  // Create adjacency list from inputs and outputs
  // to determine the edges of the graph
  std::vector<std::list<size_t>> adjacencyList;
  adjacencyList.resize(passIndices.size());

  for (size_t i = 0; i < passIndices.size(); ++i) {
    auto &pass = mPasses.at(passIndices.at(i));
    for (auto resourceId : pass.getOutputs()) {
      if (passReads.find(resourceId.texture) != passReads.end()) {
        for (auto read : passReads.at(resourceId.texture)) {
          adjacencyList.at(i).push_back(read);
        }
      }
    }
  }

  // Topological sort based on DFS
  mCompiledPasses.reserve(passIndices.size());
  std::vector<bool> visited(passIndices.size(), false);

  for (size_t i = passIndices.size(); i-- > 0;) {
    if (!visited.at(i)) {
      topologicalSort(mPasses, i, visited, adjacencyList, mCompiledPasses);
    }
  }

  std::reverse(mCompiledPasses.begin(), mCompiledPasses.end());

  static constexpr VkPipelineStageFlags StageFragmentTest =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
      VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  static constexpr VkPipelineStageFlags StageColor =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  static constexpr VkPipelineStageFlags StageFragmentShader =
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

  // Determine attachments, image layouts, and barriers
  std::unordered_map<rhi::TextureHandle, VkImageLayout> visitedOutputs;
  for (auto &pass : mCompiledPasses) {
    pass.mPreBarrier = RenderGraphPassBarrier{};
    pass.mPostBarrier = RenderGraphPassBarrier{};

    for (auto &input : pass.mInputs) {
      LIQUID_ASSERT(visitedOutputs.find(input.texture) != visitedOutputs.end(),
                    "Pass is reading from an empty texture");

      input.srcLayout = visitedOutputs.at(input.texture);
      input.dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      VkPipelineStageFlags otherStage = 0;
      VkAccessFlags otherAccess = 0;

      if (input.srcLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        otherStage = StageFragmentTest;
        otherAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      } else if (input.srcLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        otherStage = StageColor;
        otherAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      }

      ImageBarrier preImageBarrier{};
      preImageBarrier.srcLayout = input.srcLayout;
      preImageBarrier.dstLayout = input.dstLayout;
      preImageBarrier.texture = input.texture;
      preImageBarrier.srcAccess = otherAccess;
      preImageBarrier.dstAccess = VK_ACCESS_SHADER_READ_BIT;

      ImageBarrier postImageBarrier{};
      postImageBarrier.srcLayout = input.dstLayout;
      postImageBarrier.dstLayout = input.srcLayout;
      postImageBarrier.texture = input.texture;
      postImageBarrier.srcAccess = VK_ACCESS_SHADER_READ_BIT;
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

    for (size_t i = 0; i < pass.mOutputs.size(); ++i) {
      auto &output = pass.mOutputs.at(i);
      auto &attachment = pass.mAttachments.at(i);
      if (visitedOutputs.find(output.texture) == visitedOutputs.end()) {
        output.srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.loadOp = AttachmentLoadOp::Clear;
      } else {
        output.srcLayout = visitedOutputs.at(output.texture);
        attachment.loadOp = AttachmentLoadOp::Load;
      }

      attachment.storeOp = AttachmentStoreOp::Store;

      VkPipelineStageFlags stage = 0;
      VkAccessFlags srcAccess = 0;
      VkAccessFlags dstAccess = 0;

      auto &texture =
          resourceRegistry.getTextureMap().getDescription(output.texture);
      if ((texture.usage & TextureUsage::Color) == TextureUsage::Color) {
        output.dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        stage = StageColor;
        srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dstAccess = srcAccess | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
      } else if ((texture.usage & TextureUsage::Depth) == TextureUsage::Depth) {
        output.dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        stage = StageFragmentTest;
        srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dstAccess = srcAccess | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      }

      MemoryBarrier memoryBarrier{};
      memoryBarrier.srcAccess = srcAccess;
      memoryBarrier.dstAccess = dstAccess;

      pass.mPostBarrier.enabled = true;
      pass.mPostBarrier.srcStage |= stage;
      pass.mPostBarrier.dstStage |= stage;
      pass.mPostBarrier.memoryBarriers.push_back(memoryBarrier);

      visitedOutputs.insert_or_assign(output.texture, output.dstLayout);
    }
  }
}

void RenderGraph::setFramebufferExtent(glm::uvec2 framebufferExtent) {
  mFramebufferExtent = framebufferExtent;
  mDirty = true;
}

void RenderGraph::updateDirtyFlag() { mDirty = false; }

} // namespace liquid::rhi
