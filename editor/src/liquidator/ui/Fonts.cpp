#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"
#include "Fonts.h"

namespace liquidator {

static constexpr float FONT_SIZE = 16.0f;

void addFonts() {
  liquid::Path fontPath = std::filesystem::current_path() / "assets" / "fonts" /
                          "Roboto-Regular.ttf";
  auto &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), FONT_SIZE);
}

} // namespace liquidator
