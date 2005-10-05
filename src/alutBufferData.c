#include "alutInternal.h"

struct BufferData_struct
{
  ALvoid *data;
  ALsizei length;
  ALint numChannels;
  ALint bitsPerSample;
  ALfloat sampleFrequency;
};

BufferData *
_alutBufferDataConstruct (ALvoid *data, ALsizei length, ALint numChannels,
                          ALint bitsPerSample, ALfloat sampleFrequency)
{
  BufferData *bufferData = (BufferData *) _alutMalloc (sizeof (BufferData));
  if (bufferData == NULL)
    {
      return NULL;
    }

  bufferData->data = data;
  bufferData->length = length;
  bufferData->numChannels = numChannels;
  bufferData->bitsPerSample = bitsPerSample;
  bufferData->sampleFrequency = sampleFrequency;

  return bufferData;
}

ALboolean
_alutBufferDataDestroy (BufferData *bufferData)
{
  if (bufferData->data != NULL)
    {
      free (bufferData->data);
    }
  free (bufferData);
  return AL_TRUE;
}

ALvoid *
_alutBufferDataGetData (const BufferData *bufferData)
{
  return bufferData->data;
}

void
_alutBufferDataDetachData (BufferData *bufferData)
{
  bufferData->data = NULL;
}

ALsizei
_alutBufferDataGetLength (const BufferData *bufferData)
{
  return bufferData->length;
}

static ALint
getNumChannels (const BufferData *bufferData)
{
  return bufferData->numChannels;
}

static ALint
getBitsPerSample (const BufferData *bufferData)
{
  return bufferData->bitsPerSample;
}

ALfloat
_alutBufferDataGetSampleFrequency (const BufferData *bufferData)
{
  return bufferData->sampleFrequency;
}

/****************************************************************************
 * The utility functions below do not know the internal BufferData
 * representation.
 ****************************************************************************/

ALboolean
_alutGetFormat (const BufferData *bufferData, ALenum *format)
{
  switch (getNumChannels (bufferData))
    {
    case 1:
      switch (getBitsPerSample (bufferData))
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
      switch (getBitsPerSample (bufferData))
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

static ALboolean
passBufferData (BufferData *bufferData, ALuint bid)
{
  ALenum format;
  if (!_alutGetFormat (bufferData, &format))
    {
      return AL_FALSE;
    }
  alBufferData (bid, format, _alutBufferDataGetData (bufferData),
                _alutBufferDataGetLength (bufferData),
                (ALsizei) _alutBufferDataGetSampleFrequency (bufferData));
  if (alGetError () != AL_NO_ERROR)
    {
      _alutSetError (ALUT_ERROR_BUFFER_DATA);
      return AL_FALSE;
    }
  return AL_TRUE;
}

ALuint
_alutPassBufferData (BufferData *bufferData)
{
  ALuint buffer = generateBuffer ();
  if (buffer == AL_NONE)
    {
      return AL_NONE;
    }

  if (!passBufferData (bufferData, buffer))
    {
      return AL_NONE;
    }

  return buffer;
}
