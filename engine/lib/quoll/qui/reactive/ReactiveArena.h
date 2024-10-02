#pragma once

namespace qui::rgraph {

class ReactiveNode;

class ReactiveArena {
public:
  template <std::derived_from<ReactiveNode> TNode, typename... Args>
  inline TNode *allocate(Args &&...args) {
    auto *node = new TNode(std::forward<Args>(args)...);
    mNodes.insert_or_assign(node, std::unique_ptr<ReactiveNode>(node));
    return node;
  }

private:
  std::unordered_map<ReactiveNode *, std::unique_ptr<ReactiveNode>> mNodes;
};

} // namespace qui::rgraph
