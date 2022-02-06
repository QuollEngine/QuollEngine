#pragma once

#ifdef LIQUID_PROFILER
#include <optick.h>

// Undefining this value since it is
// defined by Vulkan as well
#undef VKAPI_PTR

#define LIQUID_PROFILE_EVENT OPTICK_EVENT
#define LIQUID_PROFILE_FRAME OPTICK_FRAME
#define LIQUID_PROFILE_CATEGORY OPTICK_CATEGORY
#define LIQUID_PROFILE_TAG OPTICK_CATEGORY
#else

#define LIQUID_PROFILE_EVENT
#define LIQUID_PROFILE_FRAME
#define LIQUID_PROFILE_CATEGORY
#define LIQUID_PROFILE_TAG

#endif
