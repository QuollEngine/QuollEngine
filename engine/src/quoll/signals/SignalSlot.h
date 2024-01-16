#pragma once

namespace quoll {

class SignalSlot {
public:
  SignalSlot() = default;

  SignalSlot(std::function<void()> &&disconnect);

  inline void disconnect() { return mDisconnect(); }

private:
  std::function<void()> mDisconnect;
};

} // namespace quoll
