#include "alutInternal.h"
#include <ctype.h>

/****************************************************************************/

typedef enum
{
  LittleEndian,
  BigEndian,
  UnknwonEndian                 /* has anybody still a PDP11? :-) */
} Endianess;

/* test from Harbison & Steele, "C - A Reference Manual", section 6.1.2 */
static Endianess
endianess (void)
{
  union
  {
    long l;
    char c[sizeof (long)];
  } u;

  u.l = 1;
  return (u.c[0] == 1) ? LittleEndian :
    ((u.c[sizeof (long) - 1] == 1) ? BigEndian : UnknwonEndian);
}

/****************************************************************************/

typedef ALvoid *codec (ALvoid *data, unsigned int *length);

static codec linear, uLaw, pcm8s, pcm16, aLaw;

static ALboolean _alutLoadWavFile (InputStream *stream,
                                   BufferData *bufferData);
static ALboolean _alutLoadAUFile (InputStream *stream,
                                  BufferData *bufferData);
static ALboolean _alutLoadRawFile (InputStream *stream,
                                   BufferData *bufferData);
static ALboolean _alutLoadFile (InputStream *stream, BufferData *bufferData);

ALuint
alutCreateBufferFromFile (const char *fileName)
{
  InputStream *stream;
  BufferData bufferData;
  ALuint buffer;

  if (!_alutSanityCheck ())
    {
      return AL_NONE;
    }

  stream = _alutStreamFromFile (fileName);
  if (stream == NULL)
    {
      return AL_NONE;
    }

  if (!_alutLoadFile (stream, &bufferData))
    {
      _alutStreamDestroy (stream);
      return AL_NONE;
    }
  _alutStreamDestroy (stream);

  buffer = _alutPassBufferData (&bufferData);
  free (bufferData.data);
  return buffer;
}

ALuint
alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length)
{
  InputStream *stream;
  BufferData bufferData;
  ALuint buffer;

  if (!_alutSanityCheck ())
    {
      return AL_NONE;
    }

  stream = _alutStreamFromMemory (data, length);
  if (stream == NULL)
    {
      return AL_NONE;
    }

  if (!_alutLoadFile (stream, &bufferData))
    {
      _alutStreamDestroy (stream);
      return AL_NONE;
    }
  _alutStreamDestroy (stream);

  buffer = _alutPassBufferData (&bufferData);
  free (bufferData.data);
  return buffer;
}

static void *
_alutPrivateLoadMemoryFromFile (const char *fileName, ALenum *format,
                                ALsizei *size, ALfloat *frequency)
{
  InputStream *stream;
  BufferData bufferData;
  ALenum fmt;

  stream = _alutStreamFromFile (fileName);
  if (stream == NULL)
    {
      return NULL;
    }

  if (!_alutLoadFile (stream, &bufferData))
    {
      _alutStreamDestroy (stream);
      return NULL;
    }
  _alutStreamDestroy (stream);

  if (!_alutGetFormat (&bufferData, &fmt))
    {
      free (bufferData.data);
      return NULL;
    }

  if (size != NULL)
    {
      *size = bufferData.length;
    }

  if (format != NULL)
    {
      *format = fmt;
    }

  if (frequency != NULL)
    {
      *frequency = bufferData.sampleFrequency;
    }

  return bufferData.data;
}

static void *
_alutPrivateLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                                     ALenum *format, ALsizei *size,
                                     ALfloat *frequency)
{
  InputStream *stream;
  BufferData bufferData;
  ALenum fmt;

  stream = _alutStreamFromMemory (data, length);
  if (stream == NULL)
    {
      return NULL;
    }

  if (!_alutLoadFile (stream, &bufferData))
    {
      _alutStreamDestroy (stream);
      return NULL;
    }
  _alutStreamDestroy (stream);

  if (!_alutGetFormat (&bufferData, &fmt))
    {
      free (bufferData.data);
      return NULL;
    }

  if (size != NULL)
    {
      *size = bufferData.length;
    }

  if (format != NULL)
    {
      *format = fmt;
    }

  if (frequency != NULL)
    {
      *frequency = bufferData.sampleFrequency;
    }

  return bufferData.data;
}

const char *
alutGetMIMETypes (ALenum loader)
{
  if (!_alutSanityCheck ())
    {
      return NULL;
    }

  /* We do not distinguish the loaders yet... */
  switch (loader)
    {
    case ALUT_LOADER_BUFFER:
      return "audio/basic,audio/x-raw,audio/x-wav";

    case ALUT_LOADER_MEMORY:
      return "audio/basic,audio/x-raw,audio/x-wav";

    default:
      _alutSetError (ALUT_ERROR_INVALID_ENUM);
      return NULL;
    }
}

/*
  Yukky backwards compatibility crap.
*/

void
alutLoadWAVFile (ALbyte *fileName, ALenum *format, void **data, ALsizei *size,
                 ALsizei *frequency
#if !defined(__APPLE__)
                 , ALboolean *loop
#endif
  )
{
  ALfloat freq;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data =
    _alutPrivateLoadMemoryFromFile ((const char *) fileName, format, size,
                                    &freq);

  if (frequency)
    {
      *frequency = (ALsizei) freq;
    }
#if !defined(__APPLE__)
  if (loop)
    {
      *loop = AL_FALSE;
    }
#endif
}

void
alutLoadWAVMemory (ALbyte *buffer, ALenum *format, void **data, ALsizei *size,
                   ALsizei *frequency
#if !defined(__APPLE__)
                   , ALboolean *loop
#endif
  )
{
  ALfloat freq;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data = _alutPrivateLoadMemoryFromFileImage ((const ALvoid *) buffer, 0x7FFFFFFF,     /* Eeek! */
                                               format, size, &freq);
  if (frequency)
    {
      *frequency = (ALsizei) freq;
    }
#if !defined(__APPLE__)
  if (loop)
    {
      *loop = AL_FALSE;
    }
#endif
}

void
alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei frequency)
{
  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  free (data);
}

static int
safeToLower (int c)
{
  return isupper (c) ? tolower (c) : c;
}

static int
hasSuffixIgnoringCase (const char *string, const char *suffix)
{
  const char *stringPointer = string;
  const char *suffixPointer = suffix;

  if (suffix[0] == '\0')
    {
      return 1;
    }

  while (*stringPointer != '\0')
    {
      stringPointer++;
    }

  while (*suffixPointer != '\0')
    {
      suffixPointer++;
    }

  if (stringPointer - string < suffixPointer - suffix)
    {
      return 0;
    }

  while (safeToLower (*--suffixPointer) == safeToLower (*--stringPointer))
    {
      if (suffixPointer == suffix)
        {
          return 1;
        }
    }

  return 0;
}

static ALboolean
_alutLoadFile (InputStream *stream, BufferData *bufferData)
{
  const char *fileName;
  char magic[4];

  /* Raw files have no magic number - so use the fileName extension */

  fileName = _alutStreamGetFileName (stream);
  if (fileName != NULL && hasSuffixIgnoringCase (fileName, ".raw"))
    {
      return _alutLoadRawFile (stream, bufferData);
    }

  /* For other file formats, read the quasi-standard four byte magic number */

  if (!_alutStreamRead (stream, magic, sizeof (magic)))
    {
      return AL_FALSE;
    }

  /* Magic number 'RIFF' == Microsoft '.wav' format */

  if (magic[0] == 'R' && magic[1] == 'I' &&
      magic[2] == 'F' && magic[3] == 'F')
    {
      return _alutLoadWavFile (stream, bufferData);
    }

  /* Magic number '.snd' == Sun & Next's '.au' format */

  if (magic[0] == '.' && magic[1] == 's' &&
      magic[2] == 'n' && magic[3] == 'd')
    {
      return _alutLoadAUFile (stream, bufferData);
    }

  _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_TYPE);
  return AL_FALSE;
}

static ALboolean
_alutLoadWavFile (InputStream *stream, BufferData *bufferData)
{
  ALboolean found_header = AL_FALSE;
  UInt32LittleEndian leng1;
  char magic[4];
  codec *_codec = linear;

  bufferData->data = NULL;
  bufferData->length = 0;

  if (!_alutReadUInt32LittleEndian (stream, &leng1) ||
      !_alutStreamRead (stream, magic, sizeof (magic)))
    {
      return AL_FALSE;
    }

  if (magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'V'
      || magic[3] != 'E')
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return AL_FALSE;
    }

  while (1)
    {
      UInt16LittleEndian audioFormat;
      UInt16LittleEndian numChannels;
      UInt32LittleEndian sampleFrequency;
      UInt32LittleEndian byteRate;
      UInt16LittleEndian blockAlign;
      UInt16LittleEndian bitsPerSample;

      if (!_alutStreamRead (stream, magic, sizeof (magic)) ||
          !_alutReadUInt32LittleEndian (stream, &leng1))
        {
          return AL_FALSE;
        }

      if (magic[0] == 'f' && magic[1] == 'm' &&
          magic[2] == 't' && magic[3] == ' ')
        {
          found_header = AL_TRUE;

          if (leng1 < 16)
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA);
              return AL_FALSE;
            }

          if (!_alutReadUInt16LittleEndian (stream, &audioFormat) ||
              !_alutReadUInt16LittleEndian (stream, &numChannels) ||
              !_alutReadUInt32LittleEndian (stream, &sampleFrequency) ||
              !_alutReadUInt32LittleEndian (stream, &byteRate) ||
              !_alutReadUInt16LittleEndian (stream, &blockAlign) ||
              !_alutReadUInt16LittleEndian (stream, &bitsPerSample))
            {
              return AL_FALSE;
            }

          if (!_alutStreamSkip (stream, leng1 - 16))
            {
              return AL_FALSE;
            }

          switch (audioFormat)
            {
            case 1:            /* PCM */
              bufferData->bitsPerSample = bitsPerSample;
              _codec = (bufferData->bitsPerSample == 8
                        || endianess () == LittleEndian) ? linear : pcm16;
              break;
            case 7:            /* uLaw */
              bufferData->bitsPerSample = bitsPerSample * 2;    /* ToDo: ??? */
              _codec = uLaw;
              break;
            default:
              _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
              return AL_FALSE;
            }

          bufferData->numChannels = numChannels;
          bufferData->sampleFrequency = sampleFrequency;
        }
      else
        if (magic[0] == 'd' && magic[1] == 'a' &&
            magic[2] == 't' && magic[3] == 'a')
        {
          unsigned int len;
          char *buf;
          if (!found_header)
            {
              /* ToDo: A bit wrong to check here, fmt chunk could come later... */
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA);
              return AL_FALSE;
            }

          bufferData->length = leng1;
          len = bufferData->length;
          buf = (char *) malloc (len);
          if (buf == NULL)
            {
              _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
              return AL_FALSE;
            }

          if (!_alutStreamRead (stream, buf, len))
            {
              free (buf);
              return AL_FALSE;
            }

          bufferData->data = _codec (buf, &len);
          bufferData->length = len;
          return AL_TRUE;
        }
      else
        {
          if (!_alutStreamSkip (stream, leng1))
            {
              return AL_FALSE;
            }
        }

      if ((leng1 & 1) && !_alutStreamEOF (stream)
          && !_alutStreamSkip (stream, 1))
        {
          return AL_FALSE;
        }
    }
}

/* see: http://en.wikipedia.org/wiki/Au_file_format, G.72x are missing */

enum AUEncoding
{
  AU_ULAW_8 = 1,                /* 8-bit ISDN u-law */
  AU_PCM_8 = 2,                 /* 8-bit linear PCM (signed) */
  AU_PCM_16 = 3,                /* 16-bit linear PCM (signed, big-endian) */
  AU_PCM_24 = 4,                /* 24-bit linear PCM */
  AU_PCM_32 = 5,                /* 32-bit linear PCM */
  AU_FLOAT_32 = 6,              /* 32-bit IEEE floating point */
  AU_FLOAT_64 = 7,              /* 64-bit IEEE floating point */
  AU_ALAW_8 = 27                /* 8-bit ISDN a-law */
};

static ALboolean
_alutLoadAUFile (InputStream *stream, BufferData *bufferData)
{
  Int32BigEndian dataOffset;    /* byte offset to data part, minimum 24 */
  Int32BigEndian dataSize;      /* number of bytes in the data part, -1 = not known */
  Int32BigEndian encoding;      /* encoding of the data part, see AUEncoding */
  Int32BigEndian sampleFrequency;       /* number of samples per second */
  Int32BigEndian channels;      /* number of interleaved channels */
  codec *_codec;
  char *buf;
  size_t len;
  if (!_alutReadInt32BigEndian (stream, &dataOffset) ||
      !_alutReadInt32BigEndian (stream, &dataSize) ||
      !_alutReadInt32BigEndian (stream, &encoding) ||
      !_alutReadInt32BigEndian (stream, &sampleFrequency) ||
      !_alutReadInt32BigEndian (stream, &channels))
    {
      return AL_FALSE;
    }

  if (dataSize == -1)
    {
      dataSize = _alutStreamGetRemainingLength (stream) - 24 - dataOffset;
    }

#if 0
  fprintf (stderr,
           "data offset %ld, data size %ld, encoding %ld, sample rate %ld, channels %ld\n",
           (long) dataOffset, (long) dataSize, (long) encoding,
           (long) sampleFrequency, (long) channels);
#endif
  if (!
      (dataOffset >= 24 && dataSize > 0 && sampleFrequency >= 1
       && channels >= 1))
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA);
      return AL_FALSE;
    }

  if (!_alutStreamSkip (stream, dataOffset - 24))
    {
      return AL_FALSE;
    }

  switch (encoding)
    {
    case AU_ULAW_8:
      bufferData->bitsPerSample = 16;
      _codec = uLaw;
      break;
    case AU_PCM_8:
      bufferData->bitsPerSample = 8;
      _codec = pcm8s;
      break;
    case AU_PCM_16:
      bufferData->bitsPerSample = 16;
      _codec = (endianess () == BigEndian) ? linear : pcm16;
      break;
    case AU_ALAW_8:
      bufferData->bitsPerSample = 16;
      _codec = aLaw;
      break;
    default:
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return AL_FALSE;
    }

  bufferData->length = 0;
  bufferData->numChannels = channels;
  bufferData->sampleFrequency = sampleFrequency;
  buf = (char *) malloc (dataSize);
  if (buf == NULL)
    {
      _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
      return AL_FALSE;
    }
  if (!_alutStreamRead (stream, buf, dataSize))
    {
      free (buf);
      return AL_FALSE;
    }

  len = dataSize;
  bufferData->data = _codec (buf, &len);
  bufferData->length = len;
  return AL_TRUE;
}

ALvoid *
linear (ALvoid *data, unsigned int *length)
{
  return data;
}

ALvoid *
pcm8s (ALvoid *data, unsigned int *length)
{
  int8_t *d = (int8_t *) data;
  unsigned int l = *length;
  unsigned int i;
  for (i = 0; i < l; i++)
    {
      d[i] += 128;
    }
  return data;
}

ALvoid *
pcm16 (ALvoid *data, unsigned int *length)
{
  int16_t *d = (int16_t *) data;
  unsigned int l = *length / 2;
  unsigned int i;
  for (i = 0; i < l; i++)
    {
      int16_t x = d[i];
      d[i] = ((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF);
    }
  return data;
}

/*
 * From: http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1
 */
static int16_t
mulaw2linear (uint8_t mulawbyte)
{
  const static int16_t exp_lut[8] = {
    0, 132, 396, 924, 1980, 4092, 8316, 16764
  };
  int16_t sign, exponent, mantissa, sample;
  mulawbyte = ~mulawbyte;
  sign = (mulawbyte & 0x80);
  exponent = (mulawbyte >> 4) & 0x07;
  mantissa = mulawbyte & 0x0F;
  sample = exp_lut[exponent] + (mantissa << (exponent + 3));
  if (sign != 0)
    sample = -sample;
  return (sample);
}

ALvoid *
uLaw (ALvoid *data, unsigned int *length)
{
  uint8_t *d = (uint8_t *) data;
  unsigned int l = *length;
  unsigned int i;
  int16_t *buf = (int16_t *) malloc (l * 2);
  for (i = 0; i < l; i++)
    buf[i] = mulaw2linear (d[i]);
  free (data);
  *length *= 2;
  return buf;
}

/*
 * From: http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1
 */
#define SIGN_BIT (0x80)         /* Sign bit for a A-law byte. */
#define QUANT_MASK (0xf)        /* Quantization field mask. */
#define SEG_SHIFT (4)           /* Left shift for segment number. */
#define SEG_MASK (0x70)         /* Segment field mask. */
static int16_t
alaw2linear (uint8_t a_val)
{
  int16_t t, seg;
  a_val ^= 0x55;
  t = (a_val & QUANT_MASK) << 4;
  seg = ((int16_t) a_val & SEG_MASK) >> SEG_SHIFT;
  switch (seg)
    {
    case 0:
      t += 8;
      break;
    case 1:
      t += 0x108;
      break;
    default:
      t += 0x108;
      t <<= seg - 1;
    }
  return ((a_val & SIGN_BIT) ? t : -t);
}

ALvoid *
aLaw (ALvoid *data, unsigned int *length)
{
  uint8_t *d = (uint8_t *) data;
  unsigned int l = *length;
  unsigned int i;
  int16_t *buf = (int16_t *) malloc (l * 2);
  for (i = 0; i < l; i++)
    buf[i] = alaw2linear (d[i]);
  free (data);
  data = NULL;
  *length *= 2;
  return buf;
}

static ALboolean
_alutLoadRawFile (InputStream *stream, BufferData *bufferData)
{
  bufferData->data = NULL;
  bufferData->length = _alutStreamGetRemainingLength (stream);
  if (bufferData->length > 0)
    {
      bufferData->data = (unsigned char *) malloc (bufferData->length);
      if (!_alutStreamRead (stream, bufferData->data, bufferData->length))
        {
          free (bufferData->data);
          return AL_FALSE;
        }
    }

  bufferData->bitsPerSample = 8;
  bufferData->numChannels = 1;
  bufferData->sampleFrequency = 8000;   /* Guess */
  return AL_TRUE;
}

ALvoid *
alutLoadMemoryFromFile (const char *fileName, ALenum *format,
                        ALsizei *size, ALfloat *frequency)
{
  if (!_alutSanityCheck ())
    {
      return NULL;
    }

  return _alutPrivateLoadMemoryFromFile (fileName, format, size, frequency);
}

ALvoid *
alutLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                             ALenum *format, ALsizei *size,
                             ALfloat *frequency)
{
  if (!_alutSanityCheck ())
    {
      return NULL;
    }

  return _alutPrivateLoadMemoryFromFileImage (data, length, format, size,
                                              frequency);
}
