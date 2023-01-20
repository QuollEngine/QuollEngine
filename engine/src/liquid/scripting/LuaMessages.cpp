#include "liquid/core/Base.h"
#include "LuaMessages.h"

namespace liquid {

String LuaMessages::noEntityTable(const String &interfaceName,
                                  const String &functionName) {
  String correctName = interfaceName + ":" + functionName;
  String invalidName = interfaceName + "." + functionName;

  return "Entity does not exist. Make sure you are using "
         "` " +
         correctName +
         "` instead "
         "of `" +
         invalidName + "`";
}

} // namespace liquid
