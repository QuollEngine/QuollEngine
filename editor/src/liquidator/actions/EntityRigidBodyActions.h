#pragma once

#include "Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

using EntityCreateRigidBody = EntityDefaultCreateComponent<RigidBody>;

using EntitySetRigidBody = EntityDefaultUpdateComponent<RigidBody>;

using EntityDeleteRigidBody = EntityDefaultDeleteAction<RigidBody>;

} // namespace liquid::editor
