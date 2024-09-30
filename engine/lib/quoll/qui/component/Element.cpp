#include "quoll/core/Base.h"
#include "quoll/qui/component/Element.h"

namespace qui {

void Element::build() {
  if (!mBuilt) {
    mComponent->build();
    mBuilt = true;
  }
}

} // namespace qui
