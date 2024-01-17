#pragma once

#ifdef QUOLL_PROFILER
#include <Tracy/Tracy.hpp>

#define QUOLL_PROFILE_EVENT(name) ZoneScopedN(name)
#define QUOLL_PROFILE_FRAME(...) FrameMark
#define QUOLL_PROFILE_TAG(y, x) ZoneText(x, strlen(x))

#else

#define QUOLL_PROFILE_EVENT(...)
#define QUOLL_PROFILE_FRAME(...)
#define QUOLL_PROFILE_TAG(...)

#endif
