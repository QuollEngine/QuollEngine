#pragma once

#include "Yoga.h"

namespace quoll {

static YGFlexDirection
getYogaDirectionFromString(String direction, YGFlexDirection defaultValue) {
  if (direction == "row") {
    return YGFlexDirectionRow;
  }

  if (direction == "row-reverse") {
    return YGFlexDirectionRowReverse;
  }

  if (direction == "column-reverse") {
    return YGFlexDirectionColumnReverse;
  }

  if (direction == "column") {
    return YGFlexDirectionColumn;
  }

  // Throw warning if invalid direction specified
  return defaultValue;
}

static YGJustify getYogaJustifyFromString(String justify,
                                          YGJustify defaultValue) {
  if (justify == "start") {
    return YGJustifyFlexStart;
  }

  if (justify == "end") {
    return YGJustifyFlexEnd;
  }

  if (justify == "center") {
    return YGJustifyCenter;
  }

  if (justify == "space-around") {
    return YGJustifySpaceAround;
  }

  if (justify == "space-between") {
    return YGJustifySpaceBetween;
  }

  if (justify == "space-evenly") {
    return YGJustifySpaceEvenly;
  }

  // Throw warning if invalid justify is specified

  return defaultValue;
}

static YGAlign getYogaAlignFromString(String align, YGAlign defaultValue) {
  if (align == "start") {
    return YGAlignFlexStart;
  }

  if (align == "end") {
    return YGAlignFlexEnd;
  }

  if (align == "center") {
    return YGAlignCenter;
  }

  if (align == "baseline") {
    return YGAlignBaseline;
  }

  if (align == "stretch") {
    return YGAlignStretch;
  }

  if (align == "space-between") {
    return YGAlignSpaceBetween;
  }

  if (align == "space-around") {
    return YGAlignSpaceAround;
  }

  // Throw warning if invalid align is specified

  return defaultValue;
}
} // namespace quoll