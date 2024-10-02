#pragma once

#include "../component/Element.h"
#include "../reactive/Value.h"

namespace qui {

struct PressEvent {
  glm::vec2 point;
};

class Pressable : public Component {
  using PressHandler = std::function<void(const PressEvent &event)>;

public:
  Pressable(Value<Element> child);

  Pressable &onPress(PressHandler &&handler);
  Pressable &onPressDown(PressHandler &&handler);
  Pressable &onPressUp(PressHandler &&handler);
  Pressable &onHoverIn(PressHandler &&handler);
  Pressable &onHoverOut(PressHandler &&handler);

  void build(BuildContext &context) override;

  constexpr View *getView() override { return mChild().getView(); }

public:
  inline auto getChild() const { return mChild(); }
  inline auto getOnPress() const { return mOnPress; }
  inline auto getOnPressDown() const { return mOnPressDown; }
  inline auto getOnPressUp() const { return mOnPressUp; }
  inline auto getOnHoverIn() const { return mOnHoverIn; }
  inline auto getOnHoverOut() const { return mOnHoverOut; }

private:
  Value<Element> mChild;

  PressHandler mOnPress;
  PressHandler mOnPressDown;
  PressHandler mOnPressUp;
  PressHandler mOnHoverIn;
  PressHandler mOnHoverOut;

  bool mHovered = false;

  EventHolder mEventHolder;
};

} // namespace qui
