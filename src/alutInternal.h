/*
 * This file should be #included as the first header in all *.c files.
 */

#ifndef ALUT_INTERNAL_H
#define ALUT_INTERNAL_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <AL/alut.h>

#if HAVE___ATTRIBUTE__
#define UNUSED(x) x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

/* in alutError.c */
extern void _alutSetError (ALenum err);

/* in alutInit.c */
extern void _alutSanityCheck (void);

#endif /* not ALUT_INTERNAL_H */
