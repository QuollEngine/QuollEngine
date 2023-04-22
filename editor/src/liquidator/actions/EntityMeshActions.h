#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

using EntityDeleteMesh = EntityDefaultDeleteAction<Mesh>;

using EntityDeleteSkinnedMesh = EntityDefaultDeleteAction<SkinnedMesh>;

} // namespace liquid::editor
