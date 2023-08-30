#pragma once

#ifdef LIQUID_DEBUG
#define LiquidDebugOnly(Content)                                               \
  do {                                                                         \
    Content                                                                    \
  } while (false)
#else
#define LiquidDebugOnly(Content)                                               \
  do {                                                                         \
  } while (false)
#endif
