#pragma once

#ifdef QUOLL_DEBUG
#define QuollDebugOnly(Content)                                                \
  do {                                                                         \
    Content                                                                    \
  } while (false)
#else
#define QuollDebugOnly(Content)                                                \
  do {                                                                         \
  } while (false)
#endif
