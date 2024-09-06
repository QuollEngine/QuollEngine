#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/logger/NoopLogTransport.h"
#include "quoll-tests/Testing.h"

int main(int argc, char **argv) {
  quoll::Engine::getLogger().setTransport(quoll::NoopLogTransport);
  quoll::Engine::getUserLogger().setTransport(quoll::NoopLogTransport);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
