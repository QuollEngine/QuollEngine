#pragma once

#include "quoll/rhi/RenderCommandList.h"

#include "WorkspaceMatchParams.h"

namespace quoll::editor {

/**
 * @brief Workspace
 */
class Workspace {
public:
  /**
   * @brief Default constructor
   */
  Workspace() = default;

  /**
   * @brief Default destructor
   */
  virtual ~Workspace() = default;

  Workspace(const Workspace &) = delete;
  Workspace &operator=(const Workspace &) = delete;
  Workspace(Workspace &&) = delete;
  Workspace &operator=(Workspace &&) = delete;

  /**
   * @brief Data preparation step
   */
  virtual void prepare() = 0;

  /**
   * @brief Fixed update
   *
   * @param dt Delta time
   */
  virtual void fixedUpdate(f32 dt) = 0;

  /**
   * @brief Update
   *
   * @param dt Delta time
   */
  virtual void update(f32 dt) = 0;

  /**
   * @brief Render
   */
  virtual void render() = 0;

  /**
   * @brief Process shortcuts
   *
   * @param key Key
   * @param mods Modifiers
   */
  virtual void processShortcuts(int key, int mods) = 0;

  /**
   * @brief Update frame data
   *
   * @param commandList Render command list
   * @param frameIndex Frame index
   */
  virtual void updateFrameData(rhi::RenderCommandList &commandList,
                               u32 frameIndex) = 0;

  /**
   * @brief Get match params
   *
   * @return Match params
   */
  virtual WorkspaceMatchParams getMatchParams() const = 0;

  /**
   * Reload workspace
   */
  virtual void reload() = 0;
};

} // namespace quoll::editor
