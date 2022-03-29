#pragma once

#ifdef _MSC_VER

#include <vcruntime.h>

#else

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif // _NODISCARD

#endif
