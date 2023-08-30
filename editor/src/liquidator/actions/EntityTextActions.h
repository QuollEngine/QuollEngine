#pragma once

#include "Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace quoll::editor {

using EntitySetText = EntityDefaultUpdateComponent<Text>;

using EntityCreateText = EntityDefaultCreateComponent<Text>;

using EntityDeleteText = EntityDefaultDeleteAction<Text>;

} // namespace quoll::editor
