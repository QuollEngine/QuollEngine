#include "liquid/core/Base.h"
#include "liquid/profiler/FPSCounter.h"

#include "liquid-tests/Testing.h"

class FPSCounterTest : public ::testing::Test {
public:
  liquid::FPSCounter counter;
};

TEST_F(FPSCounterTest, CollectsFPS) {
  counter.collectFPS(60);
  EXPECT_EQ(counter.getFPS(), 60);
}
