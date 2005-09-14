#include "alutInternal.h"

static ALboolean alutInitialised = AL_FALSE;
static ALCdevice *device = NULL;
static ALCcontext *context = NULL;

void
_alutSanityCheck (void)
{
  if (!(alutInitialised == AL_TRUE))
    {
      _alutSetError (ALUT_ERROR_NOT_INITIALISED);
    }
  else if (!(alcGetCurrentContext () != NULL))
    {
      _alutSetError (ALUT_ERROR_NO_CONTEXT_AVAILABLE);
    }
}

ALboolean
alutInit (int *argcp, char **argv)
{
  if (!(alutInitialised == AL_FALSE && device == NULL && context == NULL))
    {
      _alutSetError (ALUT_ERROR_INVALID_OPERATION);
      return AL_FALSE;
    }

  device = alcOpenDevice (NULL);
  if (device == NULL)
    {
      _alutSetError (ALUT_ERROR_NO_DEVICE_AVAILABLE);
      return AL_FALSE;
    }

  context = alcCreateContext (device, NULL);
  if (context == NULL)
    {
      alcCloseDevice (device);
      device = NULL;
      _alutSetError (ALUT_ERROR_NO_CONTEXT_AVAILABLE);
      return AL_FALSE;
    }
  alcMakeContextCurrent (context);

  alutInitialised = AL_TRUE;
  return AL_TRUE;
}

ALboolean
alutInitWithoutContext (int *argcp, char **argv)
{
  if (!(alutInitialised == AL_FALSE && device == NULL && context == NULL))
    {
      _alutSetError (ALUT_ERROR_INVALID_OPERATION);
      return AL_FALSE;
    }

  alutInitialised = AL_TRUE;
  return AL_TRUE;
}

void
alutExit (void)
{
  if (!(alutInitialised == AL_TRUE))
    {
      _alutSetError (ALUT_ERROR_INVALID_OPERATION);
      return;
    }

  if (context != NULL)
    {
      alcMakeContextCurrent (NULL);
      alcDestroyContext (context);
      context = NULL;
    }

  if (device != NULL)
    {
      alcCloseDevice (device);
      device = NULL;
    }

  alutInitialised = AL_FALSE;
}
