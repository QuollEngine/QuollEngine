#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "RenderGraph.h"
#include "RenderGraphPassBase.h"

namespace liquid {

RenderGraph::RenderGraph(RenderGraph &&rhs) {
  passes = rhs.passes;
  textures = rhs.textures;
  resourceMap = rhs.resourceMap;
  registry = rhs.registry;

  rhs.passes.clear();
}

RenderGraph::~RenderGraph() {
  for (auto &x : passes) {
    delete x;
  }
}

void RenderGraph::addPassInternal(RenderGraphPassBase *pass) {
  passes.push_back(pass);
}

std::vector<RenderGraphPassBase *> RenderGraph::compile() {
  std::vector<RenderGraphPassBase *> tempPasses = passes;

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
  std::unordered_map<GraphResourceId, std::vector<size_t>> passReads;
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

  for (size_t i = 0; i < tempPasses.size(); ++i) {
    if (!visited.at(i)) {
      topologicalSort(i, visited, adjacencyList, sortedPasses);
    }
  }

  std::reverse(sortedPasses.begin(), sortedPasses.end());

  // TODO: Test this
  std::unordered_map<GraphResourceId, bool> visitedOutputs;
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
      } else if (hasTexture(output)) {
        attachment.clearValue = textures.at(output).clearValue;
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

  output.push_back(passes.at(index));
}

GraphResourceId RenderGraph::generateNewId() { return lastId++; }

GraphResourceId RenderGraph::create(const String &name,
                                    const AttachmentData &data) {
  auto id = getResourceId(name);
  textures.insert({id, data});
  return id;
}

void RenderGraph::setSwapchainColor(const glm::vec4 &color) {
  swapchainColor = color;
}

GraphResourceId RenderGraph::addPipeline(const PipelineDescriptor &descriptor) {
  auto id = generateNewId();
  pipelines.insert({id, descriptor});
  return id;
}

const GraphResourceId RenderGraph::getResourceId(const String &name) {
  if (!hasResourceId(name)) {
    resourceMap[name] = generateNewId();
  }
  return resourceMap.at(name);
}

} // namespace liquid
