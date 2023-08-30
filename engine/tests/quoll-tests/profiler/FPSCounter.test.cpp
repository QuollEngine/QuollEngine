#include "quoll/core/Base.h"
#include "quoll/profiler/FPSCounter.h"

#include "quoll-tests/Testing.h"

class FPSCounterTest : public ::testing::Test {
public:
  quoll::FPSCounter counter;
};

TEST_F(FPSCounterTest, CollectsFPS) {
  counter.collectFPS(60);
  EXPECT_EQ(counter.getFPS(), 60);
}
