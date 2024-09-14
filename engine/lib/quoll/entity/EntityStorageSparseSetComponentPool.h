#pragma once

namespace quoll {

struct EntityStorageSparseSetComponentPool {
  std::vector<usize> entityIndices;

  std::vector<Entity> entities;

  std::vector<std::any> components;
};

} // namespace quoll
