/*
 * This file should be #included as the first header in all *.c files.
 */

#if !defined(ALUT_INTERNAL_H)
#define ALUT_INTERNAL_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_BASETSD_H
#include <basetsd.h>
typedef INT8 int8_t;
typedef UINT8 uint8_t;
typedef INT16 int16_t;
typedef UINT16 uint16_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;
#else
#error Do not know sized types on this platform
#endif

#if HAVE___ATTRIBUTE__
#define UNUSED(x) x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#include <AL/alut.h>

/* in alutError.c */
extern void _alutSetError (ALenum err);

/* in alutInit.c */
extern ALboolean _alutSanityCheck (void);

#endif /* not ALUT_INTERNAL_H */
