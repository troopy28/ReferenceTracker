#pragma once

#ifdef _MSC_VER

#include <vcruntime.h>

#else

#ifndef _NODISCARD
#define _NODISCARd [[nodiscard]]
#endif // _NODISCARD

#endif