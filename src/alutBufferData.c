#include "alutInternal.h"

static ALuint
generateBuffer (void)
{
  ALuint buffer;
  alGenBuffers (1, &buffer);
  if (alGetError () != AL_NO_ERROR)
    {
      _alutSetError (ALUT_ERROR_GEN_BUFFERS);
      return AL_NONE;
    }
  return buffer;
}


ALboolean
_alutGetFormat (const BufferData *bufferData, ALenum *format)
{
  switch (bufferData->numChannels)
    {
    case 1:
      switch (bufferData->bitsPerSample)
        {
        case 8:
          *format = AL_FORMAT_MONO8;
          return AL_TRUE;
        case 16:
          *format = AL_FORMAT_MONO16;
          return AL_TRUE;
        }
      break;
    case 2:
      switch (bufferData->bitsPerSample)
        {
        case 8:
          *format = AL_FORMAT_STEREO8;
          return AL_TRUE;
        case 16:
          *format = AL_FORMAT_STEREO16;
          return AL_TRUE;
        }
      break;
    }
  _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
  return AL_FALSE;
}


static ALboolean
passBufferData (ALuint bid, const BufferData *bufferData)
{
  ALenum format;
  if (!_alutGetFormat (bufferData, &format))
    {
      return AL_FALSE;
    }
  alBufferData (bid, format, bufferData->data, bufferData->length,
                (ALsizei) bufferData->sampleFrequency);
  if (alGetError () != AL_NO_ERROR)
    {
      _alutSetError (ALUT_ERROR_BUFFER_DATA);
      return AL_FALSE;
    }
  return AL_TRUE;
}


ALuint
_alutPassBufferData (const BufferData *bufferData)
{
  ALuint buffer = generateBuffer ();
  if (buffer == AL_NONE)
    {
      return AL_NONE;
    }

  if (!passBufferData (buffer, bufferData))
    {
      return AL_NONE;
    }

  return buffer;
}
