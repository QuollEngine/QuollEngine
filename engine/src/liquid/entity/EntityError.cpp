#include "liquid/core/Base.h"
#include "EntityError.h"

namespace liquid {

EntityError::EntityError(const String &what)
    : std::runtime_error("[Entity] " + what){};

} // namespace liquid
