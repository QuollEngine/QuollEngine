#pragma once

#include <imgui.h>

#include "liquid/window/Window.h"

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

/**
 * @brief Imgui renderer
 */
class ImguiRenderer {
  /**
   * @brief Imgui frame data
   */
  struct FrameData {
    /**
     * Vertex buffer
     */
    rhi::BufferHandle vertexBuffer = rhi::BufferHandle::Invalid;

    /**
     * Vertex buffer size
     */
    size_t vertexBufferSize = 0;

    /**
     * Vertex buffer data
     */
    void *vertexBufferData = nullptr;

    /**
     * Index buffer
     */
    rhi::BufferHandle indexBuffer = rhi::BufferHandle::Invalid;

    /**
     * Index buffer size
     */
    size_t indexBufferSize = 0;

    /**
     * Index buffer data
     */
    void *indexBufferData = nullptr;
  };

public:
  /**
   * @brief Create imgui renderer
   *
   * @param window Window
   * @param registry Resource registry
   */
  ImguiRenderer(Window &window, rhi::ResourceRegistry &registry);

  /**
   * @brief Destroy imgui renderer
   */
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  /**
   * @brief Begin imgui rendering
   */
  void beginRendering();

  /**
   * @brief End imgui rendering
   *
   * Prepares all the buffers for drawing
   */
  void endRendering();

  /**
   * @brief Send imgui data to command list
   *
   * @param commandList Command list
   * @param pipeline Pipeline
   */
  void draw(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline);

  /**
   * @brief Use config path for saving settings
   *
   * @param path Config path
   */
  void useConfigPath(const String &path);

private:
  /**
   * @brief Load fonts
   */
  void loadFonts();

  /**
   * @brief Setup remder states
   *
   * @param data Imgui data
   * @param commandList Command list
   * @param fbWidth Framebuffer width
   * @param fbHeight Framebuffer height
   * @param pipeline Pipeline
   */
  void setupRenderStates(ImDrawData *data, rhi::RenderCommandList &commandList,
                         int fbWidth, int fbHeight,
                         rhi::PipelineHandle pipeline);

private:
  rhi::ResourceRegistry &mRegistry;
  rhi::TextureHandle mFontTexture = rhi::TextureHandle::Invalid;
  std::vector<FrameData> mFrameData;
  uint32_t mCurrentFrame = 0;
};

} // namespace liquid
