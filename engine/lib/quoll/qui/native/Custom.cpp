#include "quoll/core/Base.h"
#include "Custom.h"

namespace qui {

void Custom::build() { mResult.getComponent()->build(); }

View *Custom::getView() { return mResult.getComponent()->getView(); }

Element &Custom::getResult() { return mResult; }

} // namespace qui
