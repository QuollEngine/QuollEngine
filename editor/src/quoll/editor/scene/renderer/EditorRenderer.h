#pragma once

#include "quoll/renderer/RenderGraph.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/renderer/SceneRenderer.h"

#include "EditorRendererFrameData.h"

namespace quoll::editor {

class EditorRenderer {
  struct CollidableShapeDraw {
    rhi::Buffer buffer;

    u32 vertexCount = 0;
  };

public:
  EditorRenderer(RenderStorage &renderStorage, rhi::RenderDevice *device);

  void attach(RenderGraph &graph, const SceneRenderPassData &scenePassData,
              const RendererOptions &options);

  void updateFrameData(EntityDatabase &entityDatabase, Entity camera,
                       WorkspaceState &state, AssetRegistry &assetRegistry,
                       u32 frameIndex);

private:
  void renderSpriteOutlines(rhi::RenderCommandList &commandList,
                            EditorRendererFrameData &frameData,
                            rhi::PipelineHandle pipeline, u32 instanceStart,
                            u32 instanceEnd, glm::vec4 color, f32 scale);

  void renderTextOutlines(rhi::RenderCommandList &commandList,
                          EditorRendererFrameData &frameData,
                          rhi::PipelineHandle pipeline, u32 instanceStart,
                          u32 instanceEnd, glm::vec4 color, f32 scale);

  void renderMeshOutlines(rhi::RenderCommandList &commandList,
                          EditorRendererFrameData &frameData,
                          rhi::PipelineHandle pipeline, u32 instanceStart,
                          u32 instanceEnd, glm::vec4 color, f32 scale);

  void createCollidableShapes();

private:
  rhi::RenderDevice *mDevice;
  Entity mSelectedEntity = Entity::Null;

  CollidableShapeDraw mCollidableCube;
  CollidableShapeDraw mCollidableSphere;
  CollidableShapeDraw mCollidableCapsule;

  RenderStorage &mRenderStorage;
  std::array<EditorRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;

  rhi::SamplerHandle mTextOutlineSampler = rhi::SamplerHandle::Null;
};

} // namespace quoll::editor
