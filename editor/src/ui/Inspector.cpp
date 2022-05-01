#include "liquid/core/Base.h"
#include "Inspector.h"

#include <imgui.h>

namespace liquidator {

void Inspector::render(std::function<void()> &&fn) {

  if (ImGui::Begin("Inspector", &mOpen)) {
    if (ImGui::BeginTabBar("inspector-tabs")) {
      fn();

      ImGui::EndTabBar();
    }

    ImGui::End();
  }
}

} // namespace liquidator
