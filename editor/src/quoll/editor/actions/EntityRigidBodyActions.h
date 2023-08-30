#pragma once

#include "Action.h"
#include "quoll/editor/actions/EntityDefaultDeleteAction.h"

namespace quoll::editor {

using EntityCreateRigidBody = EntityDefaultCreateComponent<RigidBody>;

using EntitySetRigidBody = EntityDefaultUpdateComponent<RigidBody>;

using EntityDeleteRigidBody = EntityDefaultDeleteAction<RigidBody>;

} // namespace quoll::editor
