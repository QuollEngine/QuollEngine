#include "quoll/core/Base.h"
#include "quoll/animation/KeyframeInterpolator.h"

#include "quoll-tests/Testing.h"

using SequenceTarget = quoll::KeyframeSequenceAssetTarget;
using SequenceInterpolation = quoll::KeyframeSequenceAssetInterpolation;

using Interpolator = quoll::KeyframeInterpolator;

class KeyframeInterpolatorTest : public ::testing::Test {
public:
  glm::quat randomQuat(std::mt19937 mt) {
    std::uniform_real_distribution<f32> dist(-1.0f, 1.0f);
    f32 x = 0.0f, y = 0.0f, z = 0.0f, u = 0.0f, v = 0.0f, w = 0.0f, s = 0.0f;
    do {
      x = dist(mt);
      y = dist(mt);
      z = x * x + y * y;
    } while (z > 1);
    do {
      u = dist(mt);
      v = dist(mt);
      w = u * u + v * v;
    } while (w > 1);
    s = sqrt((1 - z) / w);

    return glm::normalize(glm::quat(s * v, x, y, s * u));
  };

  glm::vec4 quatToVec4(glm::quat q) { return glm::vec4(q.x, q.y, q.z, q.w); };
};

using KeyframeInterpolatorDeathTest = KeyframeInterpolatorTest;

TEST_F(KeyframeInterpolatorTest,
       GetLastKeyframeValueIfTimeIsBiggerThanLastKeyframe) {
  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Step;
  sequence.keyframeTimes.push_back(1.0f);
  sequence.keyframeValues.push_back(glm::vec4(0.5f));

  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 5.0f), glm::vec3(0.5f));
}

TEST_F(KeyframeInterpolatorTest, GetPreviousKeyframeValueOnStepInterpolation) {
  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Step;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(1.0f)};

  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 0.0f), glm::vec3(0.0f));
  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 0.2f), glm::vec3(0.0f));
  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 0.6f), glm::vec3(0.5f));
  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 0.8f), glm::vec3(0.5f));
  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 1.0f), glm::vec3(1.0f));
}

TEST_F(KeyframeInterpolatorTest,
       GetInterpolatedKeyFrameValueBetweenTwoKeysOnLinearInterpolationOnVec3) {
  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(1.0f)};

  for (f32 t = 0; t < 0.5f; t += 0.05f) {
    // 0 + (t - 0.0) * vec4(0.5 - 0.0) / (0.5 - 0.0) = t
    EXPECT_EQ(Interpolator::interpolateVec3(sequence, t), glm::vec3(t));
  }

  for (f32 t = 0.5f; t <= 1.0f; t += 0.05f) {
    // 0.5 + (t - 0.5) * vec4(1.0 - 0.0) / (1.0 - 0.0) = 0.5 + (1 - 0.5)
    EXPECT_EQ(Interpolator::interpolateVec3(sequence, t),
              glm::vec3(0.5f + (t - 0.5f)));
  }
}

TEST_F(KeyframeInterpolatorTest,
       GetLastItemValueIfTimeIsLargerThanLastTimeOnLinearInterpolationOnVec3) {
  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
  sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f), glm::vec4(3.0f)};

  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 1.0f), glm::vec3(3.0f));
  EXPECT_EQ(Interpolator::interpolateVec3(sequence, 2.0f), glm::vec3(3.0f));
}

TEST_F(KeyframeInterpolatorTest,
       GetInterpolatedKeyFrameValueBetweenTwoKeysOnLinearInterpolationOnQuat) {
  std::random_device device;
  std::mt19937 mt(device());

  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};

  auto q1 = randomQuat(mt);
  auto q2 = randomQuat(mt);
  auto q3 = randomQuat(mt);

  sequence.keyframeValues = {quatToVec4(q1), quatToVec4(q2), quatToVec4(q3)};

  for (f32 t = 0; t < 0.5f; t += 0.05f) {
    f32 k = t / 0.5f;

    EXPECT_EQ(Interpolator::interpolateQuat(sequence, t),
              glm::normalize(glm::slerp(q1, q2, k)));
  }

  for (f32 t = 0.5f; t <= 1.0f; t += 0.05f) {
    f32 k = (t - 0.5f) / 0.5f;

    EXPECT_EQ(Interpolator::interpolateQuat(sequence, t),
              glm::normalize(glm::slerp(q2, q3, k)));
  }
}

TEST_F(KeyframeInterpolatorTest,
       GetLastItemValueIfTimeIsLargerThanLastTimeOnLinearInterpolationOnQuat) {
  std::random_device device;
  std::mt19937 mt(device());

  quoll::KeyframeSequenceAsset sequence;
  sequence.target = SequenceTarget::Position;
  sequence.interpolation = SequenceInterpolation::Linear;
  sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};

  auto q1 = randomQuat(mt);
  auto q2 = randomQuat(mt);
  auto q3 = randomQuat(mt);

  sequence.keyframeValues = {quatToVec4(q1), quatToVec4(q2), quatToVec4(q3)};

  EXPECT_EQ(Interpolator::interpolateQuat(sequence, 1.0f), q3);
  EXPECT_EQ(Interpolator::interpolateQuat(sequence, 2.0f), q3);
}
