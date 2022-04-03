#include "liquid/core/Base.h"
#include "liquid/animation/KeyframeInterpolator.h"

#include <gtest/gtest.h>

using SequenceTarget = liquid::KeyframeSequenceAssetTarget;
using SequenceInterpolation = liquid::KeyframeSequenceAssetInterpolation;

class KeyframeInterpolatorTest : public ::testing::Test {
public:
  liquid::KeyframeInterpolator interpolator;
};

using KeyframeInterpolatorDeathTest = KeyframeInterpolatorTest;

TEST_F(KeyframeInterpolatorTest,
       GetLastKeyframeValueIfTimeIsBiggerThanLastKeyframe) {
  liquid::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Step;
  sequence.keyframeTimes.push_back(1.0f);
  sequence.keyframeValues.push_back(glm::vec4(0.5f));

  EXPECT_EQ(interpolator.interpolate(sequence, 5.0f), glm::vec4(0.5f));
}

TEST_F(KeyframeInterpolatorTest, GetPreviousKeyframeValueOnStepInterpolation) {
  liquid::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Step;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(1.0f)};

  EXPECT_EQ(interpolator.interpolate(sequence, 0.0f), glm::vec4(0.0f));
  EXPECT_EQ(interpolator.interpolate(sequence, 0.2f), glm::vec4(0.0f));
  EXPECT_EQ(interpolator.interpolate(sequence, 0.6f), glm::vec4(0.5f));
  EXPECT_EQ(interpolator.interpolate(sequence, 0.8f), glm::vec4(0.5f));
  EXPECT_EQ(interpolator.interpolate(sequence, 1.0f), glm::vec4(1.0f));
}

TEST_F(KeyframeInterpolatorTest,
       GetInterpolatedKeyFrameValueBetweenTwoKeysOnLinearInterpolation) {
  liquid::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(1.0f)};

  for (float t = 0; t < 0.5f; t += 0.05f) {
    // 0 + (t - 0.0) * vec4(0.5 - 0.0) / (0.5 - 0.0) = t
    EXPECT_EQ(interpolator.interpolate(sequence, t), glm::vec4(t));
  }

  for (float t = 0.5f; t <= 1.0f; t += 0.05f) {
    // 0.5 + (t - 0.5) * vec4(1.0 - 0.0) / (1.0 - 0.0) = 0.5 + (1 - 0.5)
    EXPECT_EQ(interpolator.interpolate(sequence, t),
              glm::vec4(0.5f + (t - 0.5f)));
  }
}

TEST_F(KeyframeInterpolatorTest,
       GetLastItemValueIfTimeIsLargerThanLastTimeOnLinearInterpolation) {
  liquid::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(3.0f)};

  EXPECT_EQ(interpolator.interpolate(sequence, 1.0f), glm::vec4(3.0f));
  EXPECT_EQ(interpolator.interpolate(sequence, 2.0f), glm::vec4(3.0f));
}
