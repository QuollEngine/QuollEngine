#include "quoll/core/Base.h"
#include "Uuid.h"

#include <stduuid/uuid.h>

namespace quoll {

Uuid Uuid::generate() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);

  uuids::uuid_random_generator gen{generator};
  auto id = gen();

  auto str = uuids::to_string(id);

  std::erase(str, '-');
  return Uuid(str);
}

Uuid::Uuid(String uuid) : mUuid(uuid) {}

void Uuid::updateWithString(String uuid) { mUuid = uuid; }

} // namespace quoll
