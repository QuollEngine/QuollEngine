#pragma once

#include "RenderGraphPass.h"

namespace liquid {

/**
 * @brief Render graph texture sync dependency
 */
struct RenderGraphTextureSyncDependency {
  /**
   * Pipeline stage
   */
  rhi::PipelineStage stage;

  /**
   * Access
   */
  rhi::Access access;

  /**
   * Image layout
   */
  rhi::ImageLayout layout;
};

/**
 * @brief Render graph buffer sync dependency
 */
struct RenderGraphBufferSyncDependency {
  /**
   * Pipeline stage
   */
  rhi::PipelineStage stage;

  /**
   * Access
   */
  rhi::Access access;
};

/**
 * @brief Render graph sync dependency utilities
 */
class RenderGraphSyncDependency {
public:
  /**
   * @brief Get sync dependency for texture read operation
   *
   * @param type Render graph pass type
   * @return Render graph texture sync dependency
   */
  static RenderGraphTextureSyncDependency
  getTextureRead(RenderGraphPassType type);

  /**
   * @brief Get sync dependency for texture write operation
   *
   * @param type Render graph pass type
   * @param attachmentType Attachment type
   * @return Render graph texture sync dependency
   */
  static RenderGraphTextureSyncDependency
  getTextureWrite(RenderGraphPassType type, AttachmentType attachmentType);

  /**
   * @brief Get sync dependency for buffer read
   *
   * @param type Render graph pass type
   * @param usage Buffer usage
   * @return Render graph buffer sync dependency
   */
  static RenderGraphBufferSyncDependency getBufferRead(RenderGraphPassType type,
                                                       rhi::BufferUsage usage);

  /**
   * @brief Get sync dependency for buffer write
   *
   * @param type Render graph pass type
   * @return Render graph buffer sync dependency
   */
  static RenderGraphBufferSyncDependency
  getBufferWrite(RenderGraphPassType type);
};

} // namespace liquid
