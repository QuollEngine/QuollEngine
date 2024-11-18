#pragma once

#include "../common/Traits.h"
#include "Component.h"

namespace qui {

class Element {
public:
  Element() = default;

  template <std::derived_from<Component> Comp>
  constexpr Element(Comp &&component)
      : mComponent(new Comp(std::forward<Comp>(component))){};

  template <std::derived_from<Component> Comp>
  constexpr Element(const Comp &component) : mComponent(new Comp(component)){};

  constexpr View *getView() { return mComponent->getView(); }

  inline bool operator==(const Element &other) const {
    return mComponent == other.mComponent;
  }

  inline operator bool() const { return mComponent != nullptr; }

  inline const Component *getComponent() const { return mComponent.get(); }

  void build(BuildContext &context);

private:
  quoll::SharedPtr<Component> mComponent = nullptr;
  bool mBuilt = false;
};

} // namespace qui
