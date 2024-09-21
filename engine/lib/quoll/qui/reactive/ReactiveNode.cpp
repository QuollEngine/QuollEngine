#include "quoll/core/Base.h"
#include "ReactiveNode.h"

namespace qui::rgraph {

void ReactiveNode::addDependent(ReactiveNode *node) {
  mDependents.insert_or_assign(node, true);
}

void ReactiveNode::removeDependent(ReactiveNode *node) {
  mDependents.erase(node);
}

void ReactiveNode::notify() {
  for (auto &[node, _] : mDependents) {
    node->update();
  }

  for (auto &observer : mObservers) {
    observer();
  }
}

} // namespace qui::rgraph
