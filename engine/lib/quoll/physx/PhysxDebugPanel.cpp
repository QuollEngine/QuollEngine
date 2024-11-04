#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "PhysxDebugPanel.h"

namespace quoll::debug {

void PhysxDebugPanel::create(physx::PxFoundation *foundation) {
  mPvd = physx::PxCreatePvd(*foundation);
}

void PhysxDebugPanel::release() {
  if (mPvd) {
    mPvd->release();
  }
}

void PhysxDebugPanel::onRenderMenu() {
  ImGui::MenuItem("PhysX", nullptr, &mOpen);
}

void PhysxDebugPanel::onRender() {
  if (!mOpen) {
    return;
  }

  if (ImGui::Begin("PhysX", &mOpen, ImGuiWindowFlags_NoDocking)) {
    ImGui::Text("Physx Version: %d.%d.%d", PX_PHYSICS_VERSION_MAJOR,
                PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX);

    ImGui::Separator();
    ImGui::Text("Physx Visual Debugger");
    imgui::inputText("PVD Host", mPvdHost);
    ImGui::InputScalar("PVD Port", ImGuiDataType_S32, &mPvdPort);

    if (mPvd->isConnected()) {
      if (ImGui::Button("Disconnect")) {
        mPvd->disconnect();
      }
    } else {
      if (ImGui::Button("Connect")) {
        static constexpr u32 PvdTimeoutInMs = 2000;
        physx::PxPvdTransport *transport =
            physx::PxDefaultPvdSocketTransportCreate(mPvdHost.c_str(), mPvdPort,
                                                     PvdTimeoutInMs);
        mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
      }
      ImGui::Text("Make sure PhysX Visual Debugger app is open");
    }

    ImGui::End();
  }
}

} // namespace quoll::debug
