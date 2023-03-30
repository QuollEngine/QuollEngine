#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/renderer/RenderGraphEvaluator.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

class Window;

/**
 * @brief Default graph resources
 */
struct DefaultGraphResources {
  /**
   * Scene color texture
   */
  rhi::TextureHandle sceneColor;

  /**
   * Depth buffer texture
   */
  rhi::TextureHandle depthBuffer;

  /**
   * Imgui texture
   */
  rhi::TextureHandle imguiColor;

  /**
   * Default color
   */
  glm::vec4 defaultColor;
};

/**
 * @brief Renderer
 *
 * Executes the graph in the render device
 */
class Renderer {
public:
  /**
   * @brief Create renderer
   *
   * @param window Window
   * @param device Render device
   */
  Renderer(Window &window, rhi::RenderDevice *device);

  ~Renderer() = default;
  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  /**
   * @brief Get shader library
   *
   * @return Shader library
   */
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }

  /**
   * @brief Get render device
   *
   * @return Render device
   */
  inline rhi::RenderDevice *getRenderDevice() { return mDevice; }

  /**
   * @brief Render
   *
   * @param graph Render graph
   * @param commandList Render command list
   * @param frameIndex Frame index
   */
  void render(RenderGraph &graph, rhi::RenderCommandList &commandList,
              uint32_t frameIndex);

  /**
   * @brief Wait for device
   */
  inline void wait() { mDevice->waitForIdle(); }

  /**
   * @brief Get render storage
   *
   * @return Render storage
   */
  inline RenderStorage &getRenderStorage() { return mRenderStorage; }

private:
  ShaderLibrary mShaderLibrary;
  rhi::RenderDevice *mDevice;
  RenderStorage mRenderStorage;

  RenderGraphEvaluator mGraphEvaluator;
};

} // namespace liquid
