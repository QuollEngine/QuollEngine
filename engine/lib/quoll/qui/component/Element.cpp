#include "quoll/core/Base.h"
#include "quoll/qui/component/Element.h"

namespace qui {

void Element::build(BuildContext &context) {
  if (!mBuilt) {
    mComponent->build(context);
    mBuilt = true;
  }
}

} // namespace qui
