#pragma once

#include "liquid/window/Window.h"

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/RenderGraph.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/renderer/ShaderLibrary.h"

#include "liquid/imgui/Imgui.h"

namespace liquid {

/**
 * @brief Imgui render pass data
 */
struct ImguiRenderPassData {
  /**
   * Imgui pass
   */
  rhi::RenderGraphPass &pass;

  /**
   * Imgui texture
   */
  rhi::TextureHandle imguiColor;
};

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
    rhi::Buffer vertexBuffer;

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
    rhi::Buffer indexBuffer;

    /**
     * Index buffer size
     */
    size_t indexBufferSize = 0;

    /**
     * Index buffer data
     */
    void *indexBufferData = nullptr;
  };

  static constexpr const glm::vec4 DefaultClearColor{0.0f, 0.0f, 0.0f, 1.0f};

public:
  /**
   * @brief Create imgui renderer
   *
   * @param window Window
   * @param device Render device
   * @param shaderLibrary Shader library
   */
  ImguiRenderer(Window &window, ShaderLibrary &shaderLibrary,
                rhi::RenderDevice *device);

  /**
   * @brief Destroy imgui renderer
   */
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  /**
   * @brief Attach render passes to render graph
   *
   * @param graph Render graph
   * @return Imgui render pass data
   */
  ImguiRenderPassData attach(rhi::RenderGraph &graph);

  /**
   * @brief Set clear color
   *
   * @param clearColor Clear color
   */
  void setClearColor(const glm::vec4 &clearColor);

  /**
   * @brief Build font atlas
   */
  void buildFonts();

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
   * @brief Update frame data
   *
   * @param frameIndex
   */
  void updateFrameData(uint32_t frameIndex);

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
  ShaderLibrary &mShaderLibrary;
  rhi::TextureHandle mFontTexture = rhi::TextureHandle::Invalid;
  std::vector<FrameData> mFrameData;
  uint32_t mCurrentFrame = 0;

  glm::vec4 mClearColor{DefaultClearColor};

  bool mReady = false;

  rhi::RenderDevice *mDevice;
};

} // namespace liquid
