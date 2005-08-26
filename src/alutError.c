#include "alut.h"
#include <stdio.h>

#ifndef NULL
#define NULL 0L
#endif

/* Enable the following line to have all ALUT errors reported as they happen. */
/* #define ALUT_DEBUG_ENABLED 1 */

ALint lastError = ALUT_ERROR_NO_ERROR ;

void _alutSetError ( ALint err )
{
#ifdef ALUT_DEBUG_ENABLED
  fprintf ( stderr, "DEBUG: ALUT ERROR - %s\n",
                           alutGetErrorString ( err ) ) ;
#endif

  if ( lastError == ALUT_ERROR_NO_ERROR )
    lastError = err ;
}


ALint alutGetError ()
{
  _alutSanityCheck () ;
  ALint ret = lastError ;
  lastError = ALUT_ERROR_NO_ERROR ;
  return ret ;
}


const char *alutGetErrorString ( ALint error )
{
  _alutSanityCheck () ;
  switch ( error )
  {
    case ALUT_ERROR_NO_ERROR :
      return "No ALUT error found" ;

    case ALUT_ERROR_OUT_OF_MEMORY :
      return "ALUT ran out of memory" ;

    case ALUT_ERROR_INVALID_ENUM :
      return "ALUT was given an invalid enumeration" ;

    case ALUT_ERROR_INVALID_VALUE :
      return "ALUT was given an invalid value" ;

    case ALUT_ERROR_INVALID_OPERATION :
      return "ALUT was given an invalid operation" ;

    case ALUT_ERROR_NOT_INITIALISED :
      return "alutInit was not called" ;

    case ALUT_ERROR_NO_DEVICE_AVAILABLE :
      return "ALUT could not find a valid OpenAL device" ;

    case ALUT_ERROR_NO_CONTEXT_AVAILABLE :
      return "ALUT could not create a valid OpenAL context" ;

    case ALUT_ERROR_FILE_NOT_FOUND :
      return "File not found" ;

    case ALUT_ERROR_FILE_NOT_READABLE :
      return "File found but not readable" ;

    case ALUT_ERROR_UNSUPPORTED_FILE_TYPE :
      return "Unsupported file type" ;

    case ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE :
      return "Unsupported mode within an otherwise usable file type" ;

    case ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE :
      return "File was corrupt or truncated" ;

    default :
      return "An impossible ALUT error condition was reported?!?" ;
  }
}


