#pragma once

#include "../common/Traits.h"
#include "Component.h"

namespace qui {

class Element {
public:
  Element() = default;

  template <std::derived_from<Component> Comp>
  Element(Comp &&component)
      : mComponent(new Comp(std::forward<Comp>(component))){};

  template <std::derived_from<Component> Comp>
  Element(const Comp &component) : mComponent(new Comp(component)){};

  inline Component *getComponent() { return mComponent.get(); }

  inline bool operator==(const Element &other) const {
    return mComponent == other.mComponent;
  }

private:
  quoll::SharedPtr<Component> mComponent = nullptr;
};

} // namespace qui
