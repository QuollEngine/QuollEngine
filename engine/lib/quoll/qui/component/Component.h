#pragma once

#include "View.h"

namespace qui {

class Component {
public:
  virtual ~Component() = default;

  virtual void build() = 0;

  virtual View *getView() = 0;
};

} // namespace qui
