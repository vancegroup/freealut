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

typedef ALvoid *Codec (ALvoid *data, ALsizei length, ALint numChannels,
                       ALint bitsPerSample, ALfloat sampleFrequency);

static Codec linear, uLaw, pcm8s, pcm16, aLaw;

static BufferData *_alutLoadWavFile (InputStream *stream);
static BufferData *_alutLoadAUFile (InputStream *stream);
static BufferData *_alutLoadRawFile (InputStream *stream);
static BufferData *_alutLoadFile (InputStream *stream);

static ALuint
createBuffer (InputStream *stream)
{
  BufferData *bufferData;
  ALuint buffer;

  if (stream == NULL)
    {
      return AL_NONE;
    }

  bufferData = _alutLoadFile (stream);
  _alutStreamDestroy (stream);
  if (bufferData == NULL)
    {
      return AL_NONE;
    }

  buffer = _alutPassBufferData (bufferData);
  _alutBufferDataDestroy (bufferData);

  return buffer;
}

ALuint
alutCreateBufferFromFile (const char *fileName)
{
  if (!_alutSanityCheck ())
    {
      return AL_NONE;
    }
  return createBuffer (_alutStreamConstructFromFile (fileName));
}

ALuint
alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length)
{
  if (!_alutSanityCheck ())
    {
      return AL_NONE;
    }
  return createBuffer (_alutStreamConstructFromMemory (data, length));
}

static void *
loadMemory (InputStream *stream, ALenum *format, ALsizei *size,
            ALfloat *frequency)
{
  BufferData *bufferData;
  ALenum fmt;
  void *data;

  if (stream == NULL)
    {
      return NULL;
    }

  bufferData = _alutLoadFile (stream);
  if (bufferData == NULL)
    {
      _alutStreamDestroy (stream);
      return NULL;
    }
  _alutStreamDestroy (stream);

  if (!_alutGetFormat (bufferData, &fmt))
    {
      _alutBufferDataDestroy (bufferData);
      return NULL;
    }

  if (size != NULL)
    {
      *size = _alutBufferDataGetLength (bufferData);
    }

  if (format != NULL)
    {
      *format = fmt;
    }

  if (frequency != NULL)
    {
      *frequency = _alutBufferDataGetSampleFrequency (bufferData);
    }

  data = _alutBufferDataGetData (bufferData);
  _alutBufferDataDetachData (bufferData);
  _alutBufferDataDestroy (bufferData);
  return data;
}

ALvoid *
alutLoadMemoryFromFile (const char *fileName, ALenum *format,
                        ALsizei *size, ALfloat *frequency)
{
  if (!_alutSanityCheck ())
    {
      return NULL;
    }
  return loadMemory (_alutStreamConstructFromFile (fileName), format, size,
                     frequency);
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

  return loadMemory (_alutStreamConstructFromMemory (data, length), format,
                     size, frequency);
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
    loadMemory (_alutStreamConstructFromFile (fileName), format, size, &freq);
  if (*data == NULL)
    {
      return;
    }

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

  /* ToDo: Can we do something less insane than passing 0x7FFFFFFF? */
  loadMemory (_alutStreamConstructFromMemory (buffer, 0x7FFFFFFF), format,
              size, &freq);
  if (*data == NULL)
    {
      return;
    }

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

static BufferData *
_alutLoadFile (InputStream *stream)
{
  const char *fileName;
  char magic[4];

  /* Raw files have no magic number - so use the fileName extension */

  fileName = _alutStreamGetFileName (stream);
  if (fileName != NULL && hasSuffixIgnoringCase (fileName, ".raw"))
    {
      return _alutLoadRawFile (stream);
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
      return _alutLoadWavFile (stream);
    }

  /* Magic number '.snd' == Sun & Next's '.au' format */

  if (magic[0] == '.' && magic[1] == 's' &&
      magic[2] == 'n' && magic[3] == 'd')
    {
      return _alutLoadAUFile (stream);
    }

  _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_TYPE);
  return AL_FALSE;
}

static BufferData *
_alutLoadWavFile (InputStream *stream)
{
  ALboolean found_header = AL_FALSE;
  UInt32LittleEndian chunkLength;
  char magic[4];
  UInt16LittleEndian audioFormat;
  UInt16LittleEndian numChannels;
  UInt32LittleEndian sampleFrequency;
  UInt32LittleEndian byteRate;
  UInt16LittleEndian blockAlign;
  UInt16LittleEndian bitsPerSample;
  Codec *codec = linear;

  if (!_alutStreamReadUInt32LE (stream, &chunkLength) ||
      !_alutStreamRead (stream, magic, sizeof (magic)))
    {
      return NULL;
    }

  if (magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'V'
      || magic[3] != 'E')
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return NULL;
    }

  while (1)
    {
      if (!_alutStreamRead (stream, magic, sizeof (magic)) ||
          !_alutStreamReadUInt32LE (stream, &chunkLength))
        {
          return NULL;
        }

      if (magic[0] == 'f' && magic[1] == 'm' &&
          magic[2] == 't' && magic[3] == ' ')
        {
          found_header = AL_TRUE;

          if (chunkLength < 16)
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA);
              return NULL;
            }

          if (!_alutStreamReadUInt16LE (stream, &audioFormat) ||
              !_alutStreamReadUInt16LE (stream, &numChannels) ||
              !_alutStreamReadUInt32LE (stream, &sampleFrequency) ||
              !_alutStreamReadUInt32LE (stream, &byteRate) ||
              !_alutStreamReadUInt16LE (stream, &blockAlign) ||
              !_alutStreamReadUInt16LE (stream, &bitsPerSample))
            {
              return NULL;
            }

          if (!_alutStreamSkip (stream, chunkLength - 16))
            {
              return NULL;
            }

          switch (audioFormat)
            {
            case 1:            /* PCM */
              codec = (bitsPerSample == 8
                       || endianess () == LittleEndian) ? linear : pcm16;
              break;
            case 7:            /* uLaw */
              bitsPerSample *= 2;       /* ToDo: ??? */
              codec = uLaw;
              break;
            default:
              _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
              return NULL;
            }
        }
      else
        if (magic[0] == 'd' && magic[1] == 'a' &&
            magic[2] == 't' && magic[3] == 'a')
        {
          ALvoid *data;
          if (!found_header)
            {
              /* ToDo: A bit wrong to check here, fmt chunk could come later... */
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA);
              return NULL;
            }

          data = (char *) _alutMalloc (chunkLength);
          if (data == NULL)
            {
              return NULL;
            }

          if (!_alutStreamRead (stream, data, chunkLength))
            {
              free (data);
              return NULL;
            }

          return codec (data, chunkLength, numChannels, bitsPerSample,
                        sampleFrequency);
        }
      else
        {
          if (!_alutStreamSkip (stream, chunkLength))
            {
              return NULL;
            }
        }

      if ((chunkLength & 1) && !_alutStreamEOF (stream)
          && !_alutStreamSkip (stream, 1))
        {
          return NULL;
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

static BufferData *
_alutLoadAUFile (InputStream *stream)
{
  Int32BigEndian dataOffset;    /* byte offset to data part, minimum 24 */
  Int32BigEndian length;        /* number of bytes in the data part, -1 = not known */
  Int32BigEndian encoding;      /* encoding of the data part, see AUEncoding */
  Int32BigEndian sampleFrequency;       /* number of samples per second */
  Int32BigEndian numChannels;   /* number of interleaved channels */
  Codec *codec;
  char *data;
  ALint bitsPerSample;

  if (!_alutStreamReadInt32BE (stream, &dataOffset) ||
      !_alutStreamReadInt32BE (stream, &length) ||
      !_alutStreamReadInt32BE (stream, &encoding) ||
      !_alutStreamReadInt32BE (stream, &sampleFrequency) ||
      !_alutStreamReadInt32BE (stream, &numChannels))
    {
      return AL_FALSE;
    }

  if (length == -1)
    {
      length = _alutStreamGetRemainingLength (stream) - 24 - dataOffset;
    }

  if (!
      (dataOffset >= 24 && length > 0 && sampleFrequency >= 1
       && numChannels >= 1))
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
      bitsPerSample = 16;
      codec = uLaw;
      break;
    case AU_PCM_8:
      bitsPerSample = 8;
      codec = pcm8s;
      break;
    case AU_PCM_16:
      bitsPerSample = 16;
      codec = (endianess () == BigEndian) ? linear : pcm16;
      break;
    case AU_ALAW_8:
      bitsPerSample = 16;
      codec = aLaw;
      break;
    default:
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return AL_FALSE;
    }

  data = (char *) _alutMalloc (length);
  if (data == NULL)
    {
      return NULL;
    }

  if (!_alutStreamRead (stream, data, length))
    {
      free (data);
      return NULL;
    }

  return codec (data, length, numChannels, bitsPerSample, sampleFrequency);
}

static BufferData *
_alutLoadRawFile (InputStream *stream)
{
  size_t length = _alutStreamGetRemainingLength (stream);
  ALvoid *data = (char *) _alutMalloc (length);
  if (data == NULL)
    {
      return NULL;
    }

  if (!_alutStreamRead (stream, data, length))
    {
      free (data);
      return NULL;
    }

  /* Guesses */
  return linear (data, length, 1, 8, 8000);
}

static ALvoid *
linear (ALvoid *data, ALsizei length, ALint numChannels,
        ALint bitsPerSample, ALfloat sampleFrequency)
{
  return _alutBufferDataConstruct (data, length, numChannels, bitsPerSample,
                                   sampleFrequency);
}

ALvoid *
pcm8s (ALvoid *data, ALsizei length, ALint numChannels,
       ALint bitsPerSample, ALfloat sampleFrequency)
{
  int8_t *d = (int8_t *) data;
  ALsizei i;
  for (i = 0; i < length; i++)
    {
      d[i] += 128;
    }
  return _alutBufferDataConstruct (data, length, numChannels, bitsPerSample,
                                   sampleFrequency);
}

ALvoid *
pcm16 (ALvoid *data, ALsizei length, ALint numChannels,
       ALint bitsPerSample, ALfloat sampleFrequency)
{
  int16_t *d = (int16_t *) data;
  ALsizei i, l = length / 2;
  for (i = 0; i < l; i++)
    {
      int16_t x = d[i];
      d[i] = ((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF);
    }
  return _alutBufferDataConstruct (data, length, numChannels, bitsPerSample,
                                   sampleFrequency);
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
uLaw (ALvoid *data, ALsizei length, ALint numChannels, ALint bitsPerSample,
      ALfloat sampleFrequency)
{
  uint8_t *d = (uint8_t *) data;
  int16_t *buf = (int16_t *) _alutMalloc (length * 2);
  ALsizei i;
  if (buf == NULL)
    {
      return NULL;
    }
  for (i = 0; i < length; i++)
    {
      buf[i] = mulaw2linear (d[i]);
    }
  free (data);
  return _alutBufferDataConstruct (buf, length * 2, numChannels,
                                   bitsPerSample, sampleFrequency);
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
aLaw (ALvoid *data, ALsizei length, ALint numChannels, ALint bitsPerSample,
      ALfloat sampleFrequency)
{
  uint8_t *d = (uint8_t *) data;
  int16_t *buf = (int16_t *) _alutMalloc (length * 2);
  ALsizei i;
  if (buf == NULL)
    {
      return NULL;
    }
  for (i = 0; i < length; i++)
    {
      buf[i] = alaw2linear (d[i]);
    }
  free (data);
  return _alutBufferDataConstruct (buf, length * 2, numChannels,
                                   bitsPerSample, sampleFrequency);
}
