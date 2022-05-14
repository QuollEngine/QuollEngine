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
static void topologicalSort(size_t index, std::vector<bool> &visited,
                            const std::vector<std::list<size_t>> &adjacencyList,
                            std::vector<size_t> &output) {
  visited.at(index) = true;

  for (size_t x : adjacencyList.at(index)) {
    if (!visited.at(x)) {
      topologicalSort(x, visited, adjacencyList, output);
    }
  }

  output.push_back(index);
}

std::vector<size_t> RenderGraph::compile() {
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
      LOG_DEBUG("Pass is ignored during compilation because it has not inputs, "
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
      passReads[resourceId].push_back(i);
    }
  }

  // Create adjacency list from inputs and outputs
  // to determine the edges of the graph
  std::vector<std::list<size_t>> adjacencyList;
  adjacencyList.resize(passIndices.size());

  for (size_t i = 0; i < passIndices.size(); ++i) {
    auto &pass = mPasses.at(passIndices.at(i));
    for (auto resourceId : pass.getOutputs()) {
      if (passReads.find(resourceId) != passReads.end()) {
        for (auto read : passReads.at(resourceId)) {
          adjacencyList.at(i).push_back(read);
        }
      }
    }
  }

  // topological sort based on DFS
  std::vector<size_t> sortedPasses;
  sortedPasses.reserve(passIndices.size());
  std::vector<bool> visited(passIndices.size(), false);

  for (size_t i = passIndices.size(); i-- > 0;) {
    if (!visited.at(i)) {
      topologicalSort(i, visited, adjacencyList, sortedPasses);
    }
  }

  std::reverse(sortedPasses.begin(), sortedPasses.end());

  std::unordered_map<rhi::TextureHandle, bool> visitedOutputs;
  for (auto index : sortedPasses) {
    auto &pass = mPasses.at(index);
    for (auto output : pass.getOutputs()) {
      visitedOutputs.insert({output, false});
    }
  }

  // Determine attachment load and store ops
  for (auto index : sortedPasses) {
    auto &pass = mPasses.at(index);
    for (size_t i = 0; i < pass.getOutputs().size(); ++i) {
      auto output = pass.getOutputs().at(i);
      auto &attachment = pass.mAttachments.at(i);

      if (!visitedOutputs.at(output)) {
        attachment.loadOp = AttachmentLoadOp::Clear;
        visitedOutputs.at(output) = true;
      } else {
        attachment.loadOp = AttachmentLoadOp::Load;
      }

      attachment.storeOp = AttachmentStoreOp::Store;
    }
  }
  return sortedPasses;
}

} // namespace liquid::rhi
