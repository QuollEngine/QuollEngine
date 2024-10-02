#pragma once

#include "BuildContext.h"
#include "View.h"

namespace qui {

class Component {
public:
  virtual ~Component() = default;

  virtual void build(BuildContext &context) = 0;

  virtual View *getView() = 0;
};

} // namespace qui
