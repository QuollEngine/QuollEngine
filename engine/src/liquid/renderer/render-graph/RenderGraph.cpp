#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "RenderGraph.h"
#include "RenderGraphPassBase.h"

namespace liquid {

RenderGraph::RenderGraph(RenderGraph &&rhs) {
  mPasses = rhs.mPasses;
  mTextures = std::move(rhs.mTextures);
  mResourceMap = std::move(rhs.mResourceMap);
  mRegistry = std::move(rhs.mRegistry);
  mPipelines = std::move(rhs.mPipelines);
  mLastId = rhs.mLastId;

  rhs.mPasses.clear();
}

RenderGraph::~RenderGraph() {
  for (auto &x : mPasses) {
    delete x;
  }
}

void RenderGraph::addPassInternal(RenderGraphPassBase *pass) {
  mPasses.push_back(pass);
}

std::vector<RenderGraphPassBase *> RenderGraph::compile() {
  LIQUID_PROFILE_EVENT("RenderGraph::compile");
  std::vector<RenderGraphPassBase *> tempPasses = mPasses;

  // Validate pass names
  std::set<String> uniquePasses;
  for (auto &x : tempPasses) {
    uniquePasses.insert(x->getName());
  }
  LIQUID_ASSERT(
      uniquePasses.size() == tempPasses.size(),
      "Some of the names in the render graph are used in more than one pass");

  if (uniquePasses.size() != tempPasses.size()) {
    engineLogger.log(Logger::Warning) << "Some of the names in the render "
                                         "graph are used in more than one pass";
  }

  // Create all resources
  for (auto &x : tempPasses) {
    x->build(RenderGraphBuilder(*this, x));
  }

  // Delete lonely nodes
  for (auto it = tempPasses.begin(); it != tempPasses.end(); ++it) {
    if ((*it)->getInputs().size() == 0 && (*it)->getOutputs().size() == 0) {
      LOG_DEBUG("Lonely pass removed from compilation: " << (*it)->getName());
      tempPasses.erase(it);
      it = tempPasses.begin();
    }
  }

  // Cache reads so we can easily access them
  // for creating the adjacency lsit
  std::unordered_map<rhi::TextureHandle, std::vector<size_t>> passReads;
  for (size_t i = 0; i < tempPasses.size(); ++i) {
    for (auto &resourceId : tempPasses.at(i)->getInputs()) {
      passReads[resourceId].push_back(i);
    }
  }

  // Create adjacency list from inputs and outputs
  // to determine the edges of the graph
  std::vector<std::list<size_t>> adjacencyList;
  adjacencyList.resize(tempPasses.size());

  for (size_t i = 0; i < tempPasses.size(); ++i) {
    for (const auto &[resourceId, _] : tempPasses.at(i)->getOutputs()) {
      if (passReads.find(resourceId) != passReads.end()) {
        for (auto &read : passReads.at(resourceId)) {
          adjacencyList.at(i).push_back(read);
        }
      }
    }
  }

  // topological sort based on DFS
  std::vector<RenderGraphPassBase *> sortedPasses;
  sortedPasses.reserve(tempPasses.size());
  std::vector<bool> visited(tempPasses.size(), false);

  for (size_t i = tempPasses.size(); i-- > 0;) {
    if (!visited.at(i)) {
      topologicalSort(i, visited, adjacencyList, sortedPasses);
    }
  }

  std::reverse(sortedPasses.begin(), sortedPasses.end());

  // TODO: Test this
  std::unordered_map<rhi::TextureHandle, bool> visitedOutputs;
  for (const auto &pass : sortedPasses) {
    for (const auto &[output, _] : pass->getOutputs()) {
      visitedOutputs.insert({output, false});
    }
  }

  for (auto &pass : sortedPasses) {
    for (auto &[output, attachment] : pass->getOutputs()) {
      if (!visitedOutputs.at(output)) {
        attachment.loadOp = AttachmentLoadOp::Clear;
        visitedOutputs.at(output) = true;
      } else {
        attachment.loadOp = AttachmentLoadOp::Load;
      }

      attachment.storeOp = AttachmentStoreOp::Store;

      if (isSwapchain(output)) {
        attachment.clearValue = getSwapchainColor();
      } else {
        attachment.clearValue = mTextures.at(output);
      }
    }
  }

  return sortedPasses;
}

void RenderGraph::topologicalSort(
    size_t index, std::vector<bool> &visited,
    const std::vector<std::list<size_t>> &adjacencyList,
    std::vector<RenderGraphPassBase *> &output) {
  visited.at(index) = true;

  for (size_t x : adjacencyList.at(index)) {
    if (!visited.at(x)) {
      topologicalSort(x, visited, adjacencyList, output);
    }
  }

  output.push_back(mPasses.at(index));
}

GraphResourceId RenderGraph::generateNewId() { return mLastId++; }

void RenderGraph::import(
    const String &name, rhi::TextureHandle handle,
    const std::variant<glm::vec4, DepthStencilClear> &clearValue) {
  mTextures.insert_or_assign(handle, clearValue);
  mResourceMap.insert_or_assign(name, handle);
}

void RenderGraph::setSwapchainColor(const glm::vec4 &color) {
  mSwapchainColor = color;
}

GraphResourceId
RenderGraph::addPipeline(const RenderGraphPipelineDescription &descriptor) {
  auto id = generateNewId();
  mPipelines.insert({id, descriptor});
  return id;
}

rhi::TextureHandle RenderGraph::getResourceId(const String &name) {
  LIQUID_ASSERT(hasResourceId(name), "Resource does not exist");
  return mResourceMap.at(name);
}

} // namespace liquid
