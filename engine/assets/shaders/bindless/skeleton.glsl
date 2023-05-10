/**
 * @brief Single skeleton joints
 */
struct SkeletonItem {
  /**
   * Joints for skeleton
   */
  mat4 joints[32];
};

Buffer(64) SkeletonsArray { SkeletonItem items[]; };

#define getSkeleton(index) uDrawParams.skeletons.items[index]
