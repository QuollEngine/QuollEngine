#include "liquid/core/Base.h"
#include "liquid/animation/Animation.h"

#include <gtest/gtest.h>

TEST(AnimationTest, CreatesAnimationWithNameAndTime) {
  liquid::Animation animation("animation_0", 2.0f);

  EXPECT_EQ(animation.getName(), "animation_0");
  EXPECT_EQ(animation.getTime(), 2.0f);
}

TEST(AnimationTest, AddsKeyframeSequence) {
  liquid::Animation animation("animation_0", 0.5f);

  animation.addKeyframeSequence(
      liquid::KeyframeSequence{liquid::KeyframeSequenceTarget::Rotation,
                               liquid::KeyframeSequenceInterpolation::Step});

  animation.addKeyframeSequence(
      liquid::KeyframeSequence{liquid::KeyframeSequenceTarget::Scale,
                               liquid::KeyframeSequenceInterpolation::Step});

  EXPECT_EQ(animation.getKeyframeSequences().at(0).getTarget(),
            liquid::KeyframeSequenceTarget::Rotation);
  EXPECT_EQ(animation.getKeyframeSequences().at(1).getTarget(),
            liquid::KeyframeSequenceTarget::Scale);
}
