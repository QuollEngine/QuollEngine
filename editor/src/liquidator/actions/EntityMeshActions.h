#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

using EntityCreateMesh = EntityDefaultCreateComponent<Mesh>;

using EntitySetMesh = EntityDefaultUpdateComponent<Mesh>;

using EntityDeleteMesh = EntityDefaultDeleteAction<Mesh>;

using EntityCreateSkinnedMesh = EntityDefaultCreateComponent<SkinnedMesh>;

using EntitySetSkinnedMesh = EntityDefaultUpdateComponent<SkinnedMesh>;

using EntityDeleteSkinnedMesh = EntityDefaultDeleteAction<SkinnedMesh>;

} // namespace liquid::editor
