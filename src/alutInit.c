
#include "alut.h"
#include <stdio.h>
#include <assert.h>

#ifndef NULL
#define NULL 0L
#endif

static ALboolean   alut_is_initialised = AL_FALSE ;
static ALCdevice  *device  = NULL ;
static ALCcontext *context = NULL ;


void _alutSanityCheck ()
{
  if ( ! alut_is_initialised )
  {
    fprintf ( stderr, "FATAL: ALUT functions were called without alutInit\n" ) ;
    fprintf ( stderr, "alutInitWithoutContext having been called.\n" ) ;
    assert ( 0 ) ;
  }

  if ( ! alcGetCurrentContext () )
  {
    fprintf ( stderr, "FATAL: ALUT functions were called without a valid\n" ) ;
    fprintf ( stderr, "OpenAL rendering context having been created.\n" ) ;
    assert ( 0 ) ;
  }
}


ALboolean alutInit ( ALint *argcp, char **argv )
{
  if ( alut_is_initialised || device != NULL || context != NULL )
  {
    _alutSetError ( ALUT_ERROR_INVALID_OPERATION ) ;
    return AL_FALSE ;
  }

  alut_is_initialised = AL_TRUE  ;

  device = alcOpenDevice ( NULL ) ; /* Use the default device */

  if ( device == NULL )
  {
    _alutSetError ( ALUT_ERROR_NO_DEVICE_AVAILABLE ) ;
    context = NULL ;
    return AL_FALSE ;
  }

  context = alcCreateContext ( device, NULL ) ;

  if ( context == NULL )
  {
    alcCloseDevice ( device ) ;

    _alutSetError ( ALUT_ERROR_NO_CONTEXT_AVAILABLE ) ;
    device = NULL ;
    return AL_FALSE ;
  }

  alcMakeContextCurrent ( context ) ;
  return AL_TRUE ;
}


ALboolean alutInitWithoutContext ( ALint *argcp, char **argv )
{
  if ( alut_is_initialised || device != NULL || context != NULL )
  {
    _alutSetError ( ALUT_ERROR_INVALID_OPERATION ) ;
    return AL_FALSE ;
  }

  alut_is_initialised = AL_TRUE  ;
  device  = NULL ;
  context = NULL ;
  return AL_TRUE ;
}


void alutExit ()
{
  if ( ! alut_is_initialised )
  {
    _alutSetError ( ALUT_ERROR_INVALID_OPERATION ) ;
    return ;
  }

  if ( context != NULL )
  {
    alcMakeContextCurrent ( NULL ) ;
    alcDestroyContext ( context ) ;
  }

  if ( device != NULL )
      alcCloseDevice ( device ) ;

  alut_is_initialised = AL_FALSE ;
  device  = NULL ;
  context = NULL ;
}


