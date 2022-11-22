#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/logger/NoopLogTransport.h"

#include "Testing.h"

int main(int argc, char **argv) {
  liquid::Engine::getLogger().setTransport(liquid::NoopLogTransport);
  liquid::Engine::getUserLogger().setTransport(liquid::NoopLogTransport);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
