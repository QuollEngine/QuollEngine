#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/logger/NoopLogTransport.h"

#include "Testing.h"

int main(int argc, char **argv) {
  quoll::Engine::getLogger().setTransport(quoll::NoopLogTransport);
  quoll::Engine::getUserLogger().setTransport(quoll::NoopLogTransport);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
