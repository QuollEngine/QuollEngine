#include "quoll/core/Base.h"
#include "Custom.h"

namespace qui {

void Custom::build(BuildContext &context) { mResult.build(context); }

View *Custom::getView() { return mResult.getView(); }

const Element &Custom::getResult() const { return mResult; }

} // namespace qui
