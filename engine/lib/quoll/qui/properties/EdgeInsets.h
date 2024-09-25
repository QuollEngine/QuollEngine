#pragma once

namespace qui {

struct EdgeInsetAxis {
  f32 start = 0.0f;
  f32 end = 0.0f;
};

class EdgeInsets {
public:
  EdgeInsetAxis vertical{0.0f, 0.0f};
  EdgeInsetAxis horizontal{0.0f, 0.0f};

public:
  constexpr EdgeInsets() = default;
  constexpr EdgeInsets(f32 verticalStart, f32 horizontalStart, f32 verticalEnd,
                       f32 horizontalEnd)
      : vertical{verticalStart, verticalEnd},
        horizontal{horizontalStart, horizontalEnd} {}
  constexpr EdgeInsets(f32 vertical, f32 horizontal)
      : EdgeInsets(vertical, horizontal, vertical, horizontal) {}
  constexpr EdgeInsets(f32 allSides) : EdgeInsets(allSides, allSides) {}

  constexpr bool operator==(const EdgeInsets &rhs) const {
    return vertical.start == rhs.vertical.start &&
           vertical.end == rhs.vertical.end &&
           horizontal.start == rhs.horizontal.start &&
           horizontal.end == rhs.horizontal.end;
  }

  constexpr bool operator!=(const EdgeInsets &rhs) const {
    return !(*this == rhs);
  }
};

} // namespace qui
