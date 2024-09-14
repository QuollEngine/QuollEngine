#pragma once

namespace quoll {

class EntityDatabase;

class EntityQuery {
public:
  EntityQuery(EntityDatabase &entityDatabase);

  Entity getFirstEntityByName(StringView name);

private:
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll
