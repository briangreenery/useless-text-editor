// Assert.h

#ifndef Assert_h
#define Assert_h

#include <cassert>

#ifdef _DEBUG
#   define Assert( condition ) assert( condition )
#else
#   define Assert( condition )
#endif

#endif
