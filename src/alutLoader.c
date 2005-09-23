#include "alutInternal.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#if HAVE_STAT
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#define structStat struct stat
#elif HAVE__STAT
#define stat(p,b) _stat((p),(b))
#define structStat struct _stat
#else
#error No stat-like function on this platform
#endif

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

struct SampleAttribs
{
  ALsizei length;
  unsigned char *buffer;
  int bps;
  int stereo;
  int rate;                     /* ToDo: This should probably be an ALfloat */
};

struct DataGetter
{
  size_t length;                /* Total length of file */
  FILE *fd;                     /* For files: just the file descriptor */
  const ALvoid *data;           /* For memory: length, data pointer and position in data */
  size_t next;
};

typedef ALvoid *codec (ALvoid *data, unsigned int *length);

static codec linear, uLaw, pcm8s, pcm16, aLaw;

typedef enum
{
  OK,
  NotOK
} Status;

/****************************************************************************/

static int
eof (struct DataGetter *source)
{
  if (source->fd != NULL)
    {
      int c = fgetc (source->fd);
      if (c != EOF)
        {
          ungetc (c, source->fd);
        }
      return c == EOF;
    }

  if (source->data != NULL)
    {
      return (source->length - source->next) == 0;
    }

  _alutSetError (ALUT_ERROR_INVALID_OPERATION);
  return 0;
}

/*
 * Like fread except it reads from a DataGetter and is only concerned
 * about bytes (not records).
 */

static Status
dgread (void *ptr, size_t numBytesToRead, struct DataGetter *source)
{
  if (source->fd != NULL)
    {
      size_t numBytesRead = fread (ptr, 1, numBytesToRead, source->fd);
      if (!(numBytesToRead == numBytesRead))
        {
          _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
          return NotOK;
        }
      return OK;
    }

  if (source->data != NULL)
    {
      size_t numBytesLeft = source->length - source->next;
      if (!(numBytesToRead <= numBytesLeft))
        {
          _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
          return NotOK;
        }
      memcpy (ptr, &(((char *) (source->data))[source->next]),
              numBytesToRead);
      source->next += numBytesToRead;
      return OK;
    }

  _alutSetError (ALUT_ERROR_INVALID_OPERATION);
  return NotOK;
}

static Status
skip (size_t numBytesToSkip, struct DataGetter *source)
{
  Status status;
  char *buf;
  if (numBytesToSkip == 0)
    {
      return OK;
    }
  buf = (char *) malloc (numBytesToSkip);
  if (!(buf != NULL))
    {
      _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
      return NotOK;
    }
  status = dgread (buf, numBytesToSkip, source);
  free (buf);
  return status;
}

/****************************************************************************/

typedef uint16_t UInt16LittleEndian;

static Status
readUInt16LittleEndian (UInt16LittleEndian *value, struct DataGetter *source)
{
  unsigned char buf[2];
  if (!(dgread (buf, sizeof (buf), source) == OK))
    {
      return NotOK;
    }
  *value = ((UInt16LittleEndian) buf[1] << 8) | ((UInt16LittleEndian) buf[0]);
  return OK;
}

/****************************************************************************/

typedef uint32_t UInt32LittleEndian;

static Status
readUInt32LittleEndian (UInt32LittleEndian * value, struct DataGetter *source)
{
  unsigned char buf[4];
  if (!(dgread (buf, sizeof (buf), source) == OK))
    {
      return NotOK;
    }
  *value =
    ((UInt32LittleEndian) buf[3] << 24) |
    ((UInt32LittleEndian) buf[2] << 16) |
    ((UInt32LittleEndian) buf[1] << 8) | ((UInt32LittleEndian) buf[0]);
  return OK;
}

/****************************************************************************/

typedef int32_t Int32BigEndian;

static Status
readInt32BigEndian (Int32BigEndian *value, struct DataGetter *source)
{
  unsigned char buf[4];
  if (!(dgread (buf, sizeof (buf), source) == OK))
    {
      return NotOK;
    }
  *value =
    ((Int32BigEndian) buf[0] << 24) |
    ((Int32BigEndian) buf[1] << 16) |
    ((Int32BigEndian) buf[2] << 8) | ((Int32BigEndian) buf[3]);
  return OK;
}

/****************************************************************************/

static Status _alutLoadWavFile (struct DataGetter *fd,
                                struct SampleAttribs *attr);
static Status _alutLoadAUFile (struct DataGetter *fd,
                               struct SampleAttribs *attr);
static Status _alutLoadRawFile (struct DataGetter *fd,
                                struct SampleAttribs *attr);
static Status _alutLoadFile (const char *fname, struct DataGetter *fd,
                             struct SampleAttribs *attr);

ALuint
alutCreateBufferFromFile (const char *filename)
{
  structStat statBuf;
  FILE *fd;
  struct DataGetter dg;
  struct SampleAttribs attr;
  ALuint buffer;
  ALuint format;

  _alutSanityCheck ();

  if (!(stat (filename, &statBuf) == 0))
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_FOUND);
      return AL_NONE;
    }

  fd = fopen (filename, "rb");
  if (!(fd != NULL))
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_READABLE);
      return AL_NONE;
    }

  dg.fd = fd;
  dg.length = statBuf.st_size;
  dg.data = NULL;
  dg.next = 0;

  attr.buffer = NULL;

  if (!(_alutLoadFile (filename, &dg, &attr) == OK))
    {
      fclose (fd);
      return AL_NONE;
    }
  fclose (fd);

  if (!(attr.bps == 8 || attr.bps == 16))
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      free (attr.buffer);
      return AL_NONE;
    }
  format = (attr.bps == 8) ?
    (attr.stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8) :
    (attr.stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);

  buffer = AL_NONE;
  alGenBuffers (1, &buffer);
  if (!(buffer != AL_NONE))
    {
      free (attr.buffer);
      return AL_NONE;
    }

  alBufferData (buffer, format, attr.buffer, attr.length, attr.rate);
  free (attr.buffer);

  return buffer;
}

ALuint
alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length)
{
  struct DataGetter dg;
  struct SampleAttribs attr;
  ALuint format;
  ALuint buffer;

  _alutSanityCheck ();

  dg.fd = NULL;
  dg.length = length;
  dg.data = data;
  dg.next = 0;

  attr.buffer = NULL;

  if (!(_alutLoadFile (NULL, &dg, &attr) == OK))
    {
      return AL_NONE;
    }

  if (!(attr.bps == 8 || attr.bps == 16))
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      free (attr.buffer);
      return AL_NONE;
    }
  format = (attr.bps == 8) ?
    (attr.stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8) :
    (attr.stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);

  buffer = AL_NONE;
  alGenBuffers (1, &buffer);
  if (!(buffer != AL_NONE))
    {
      free (attr.buffer);
      return AL_NONE;
    }

  alBufferData (buffer, format, attr.buffer, attr.length, attr.rate);
  free (attr.buffer);

  return buffer;
}

static void *
_alutPrivateLoadMemoryFromFile (const char *filename, ALenum *format,
                                ALsizei *size, ALfloat *freq)
{
  structStat statBuf;
  FILE *fd;
  struct DataGetter dg;
  struct SampleAttribs attr;

  if (!(stat (filename, &statBuf) == 0))
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_FOUND);
      return NULL;
    }

  fd = fopen (filename, "rb");
  if (!(fd != NULL))
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_READABLE);
      return NULL;
    }

  dg.fd = fd;
  dg.length = statBuf.st_size;
  dg.data = NULL;
  dg.next = 0;

  attr.buffer = NULL;

  if (!(_alutLoadFile (filename, &dg, &attr) == OK))
    {
      fclose (fd);
      return NULL;
    }
  fclose (fd);

  if (!(attr.bps == 8 || attr.bps == 16))
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      free (attr.buffer);
      return NULL;
    }

  if (size != NULL)
    {
      *size = attr.length;
    }
  if (format != NULL)
    {
      *format = (attr.bps == 8) ?
        (attr.stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8) :
        (attr.stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);
    }
  if (freq != NULL)
    {
      *freq = (ALfloat) attr.rate;
    }

  return attr.buffer;
}

static void *
_alutPrivateLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                                     ALenum *format, ALsizei *size,
                                     ALfloat *freq)
{
  struct DataGetter dg;
  struct SampleAttribs attr;

  dg.fd = NULL;
  dg.length = length;
  dg.data = data;
  dg.next = 0;

  attr.buffer = NULL;

  if (!(_alutLoadFile (NULL, &dg, &attr) == OK))
    {
      return NULL;
    }

  if (!(attr.bps == 8 || attr.bps == 16))
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      free (attr.buffer);
      return NULL;
    }

  if (size != NULL)
    {
      *size = attr.length;
    }
  if (format != NULL)
    {
      *format = (attr.bps == 8) ?
        (attr.stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8) :
        (attr.stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);
    }
  if (freq != NULL)
    {
      *freq = (ALfloat) attr.rate;
    }

  return attr.buffer;
}

const char *
alutEnumerateSupportedFileTypes (void)
{
  _alutSanityCheck ();

  return "*.wav, *.au, *.raw";
}

/*
  Yukky backwards compatibility crap.
*/

void
alutLoadWAVFile (ALbyte *filename, ALenum *format, void **data, ALsizei *size,
                 ALsizei *freq
#if !defined(__APPLE__)
                 , ALboolean *loop
#endif
  )
{
  ALfloat frequency;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data =
    _alutPrivateLoadMemoryFromFile ((const char *) filename, format, size,
                                    &frequency);

  if (freq)
    {
      *freq = (ALsizei) frequency;
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
                   ALsizei *freq
#if !defined(__APPLE__)
                   , ALboolean *loop
#endif
  )
{
  ALfloat frequency;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data = _alutPrivateLoadMemoryFromFileImage ((const ALvoid *) buffer, 0x7FFFFFFF,     /* Eeek! */
                                               format, size, &frequency);
  if (freq)
    {
      *freq = (ALsizei) frequency;
    }
#if !defined(__APPLE__)
  if (loop)
    {
      *loop = AL_FALSE;
    }
#endif
}

void
alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei freq)
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

static Status
_alutLoadFile (const char *fname, struct DataGetter *dg,
               struct SampleAttribs *attr)
{
  char magic[4];

  /* Raw files have no magic number - so use the filename extension */

  if (fname != NULL && hasSuffixIgnoringCase (fname, ".raw"))
    {
      return _alutLoadRawFile (dg, attr);
    }

  /* For other file formats, read the quasi-standard four byte magic number */

  if (!(dgread (magic, sizeof (magic), dg) == OK))
    {
      return NotOK;
    }

  /* Magic number 'RIFF' == Microsoft '.wav' format */

  if (magic[0] == 'R' && magic[1] == 'I' &&
      magic[2] == 'F' && magic[3] == 'F')
    {
      return _alutLoadWavFile (dg, attr);
    }

  /* Magic number '.snd' == Sun & Next's '.au' format */

  if (magic[0] == '.' && magic[1] == 's' &&
      magic[2] == 'n' && magic[3] == 'd')
    {
      return _alutLoadAUFile (dg, attr);
    }

  _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_TYPE);
  return NotOK;
}

static Status
_alutLoadWavFile (struct DataGetter *dg, struct SampleAttribs *attr)
{
  ALboolean found_header = AL_FALSE;
  UInt32LittleEndian leng1;
  char magic[4];
  codec *_codec = linear;

  if (attr->buffer)
    {
      free (attr->buffer);
    }
  attr->buffer = NULL;
  attr->length = 0;

  if (!(readUInt32LittleEndian (&leng1, dg) == OK &&
        dgread (magic, sizeof (magic), dg) == OK))
    {
      return NotOK;
    }

  if (!(magic[0] == 'W' && magic[1] == 'A' &&
        magic[2] == 'V' && magic[3] == 'E'))
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return NotOK;
    }

  while (1)
    {
      UInt16LittleEndian audioFormat;
      UInt16LittleEndian numChannels;
      UInt32LittleEndian sampleRate;
      UInt32LittleEndian byteRate;
      UInt16LittleEndian blockAlign;
      UInt16LittleEndian bitsPerSample;

      if (!(dgread (magic, sizeof (magic), dg) == OK &&
            readUInt32LittleEndian (&leng1, dg) == OK))
        {
          return NotOK;
        }

      if (magic[0] == 'f' && magic[1] == 'm' &&
          magic[2] == 't' && magic[3] == ' ')
        {
          found_header = AL_TRUE;

          if (!(leng1 >= 16))
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
              return NotOK;
            }

          if (!(readUInt16LittleEndian (&audioFormat, dg) == OK &&
                readUInt16LittleEndian (&numChannels, dg) == OK &&
                readUInt32LittleEndian (&sampleRate, dg) == OK &&
                readUInt32LittleEndian (&byteRate, dg) == OK &&
                readUInt16LittleEndian (&blockAlign, dg) == OK &&
                readUInt16LittleEndian (&bitsPerSample, dg) == OK))
            {
              return NotOK;
            }

          if (!(skip (leng1 - 16, dg) == OK))
            {
              return NotOK;
            }

          switch (audioFormat)
            {
            case 1:            /* PCM */
              attr->bps = bitsPerSample;
              _codec = (attr->bps == 8
                        || endianess () == LittleEndian) ? linear : pcm16;
              break;
            case 7:            /* uLaw */
              attr->bps = bitsPerSample * 2;    /* ToDo: ??? */
              _codec = uLaw;
              break;
            default:
              _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
              return NotOK;
            }

          attr->stereo = (numChannels > 1);
          attr->rate = sampleRate;
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
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
              return NotOK;
            }

          attr->length = leng1;
          len = attr->length;
          buf = (char *) malloc (len);
          if (!(buf != NULL))
            {
              _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
              return NotOK;
            }

          if (!(dgread (buf, len, dg) == OK))
            {
              free (buf);
              return NotOK;
            }

          attr->buffer = _codec (buf, &len);
          attr->length = len;
          return OK;
        }
      else
        {
          if (!(skip (leng1, dg) == OK))
            {
              return NotOK;
            }
        }

      if (!((leng1 & 1) == 0 || eof (dg) || skip (1, dg) == OK))
        {
          return NotOK;
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
static Status
_alutLoadAUFile (struct DataGetter *dg, struct SampleAttribs *attr)
{
  Int32BigEndian dataOffset;    /* byte offset to data part, minimum 24 */
  Int32BigEndian dataSize;      /* number of bytes in the data part, -1 = not known */
  Int32BigEndian encoding;      /* encoding of the data part, see AUEncoding */
  Int32BigEndian sampleRate;    /* number of samples per second */
  Int32BigEndian channels;      /* number of interleaved channels */
  codec *_codec;
  char *buf;
  size_t len;
  if (!(readInt32BigEndian (&dataOffset, dg) == OK &&
        readInt32BigEndian (&dataSize, dg) == OK &&
        readInt32BigEndian (&encoding, dg) == OK &&
        readInt32BigEndian (&sampleRate, dg) == OK &&
        readInt32BigEndian (&channels, dg) == OK))
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return NotOK;
    }

  if (dataSize == -1)
    {
      dataSize = dg->length - dataOffset;
    }

#if 0
  fprintf (stderr,
           "data offset %ld, data size %ld, encoding %ld, sample rate %ld, channels %ld\n",
           (long) dataOffset, (long) dataSize, (long) encoding,
           (long) sampleRate, (long) channels);
#endif
  if (!(dataOffset >= 24 && dataSize > 0 && sampleRate >= 1 && channels >= 1))
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return NotOK;
    }

  if (!(skip (dataOffset - 24, dg) == OK))
    {
      return NotOK;
    }

  switch (encoding)
    {
    case AU_ULAW_8:
      attr->bps = 16;
      _codec = uLaw;
      break;
    case AU_PCM_8:
      attr->bps = 8;
      _codec = pcm8s;
      break;
    case AU_PCM_16:
      attr->bps = 16;
      _codec = (endianess () == BigEndian) ? linear : pcm16;
      break;
    case AU_ALAW_8:
      attr->bps = 16;
      _codec = aLaw;
      break;
    default:
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return NotOK;
    }

  attr->length = 0;
  attr->stereo = (channels > 1);
  attr->rate = sampleRate;
  buf = (char *) malloc (dataSize);
  if (!(buf != NULL))
    {
      _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
      return NotOK;
    }
  if (!(dgread (buf, dataSize, dg) == OK))
    {
      free (buf);
      return NotOK;
    }

  if (attr->buffer)
    {
      free (attr->buffer);
    }
  len = dataSize;
  attr->buffer = _codec (buf, &len);
  attr->length = len;
  return OK;
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

static Status
_alutLoadRawFile (struct DataGetter *dg, struct SampleAttribs *attr)
{
  if (attr->buffer)
    {
      free (attr->buffer);
    }
  attr->buffer = NULL;
  attr->length = dg->length;
  if (attr->length > 0)
    {
      attr->buffer = (unsigned char *) malloc (attr->length);
      if (!(dgread (attr->buffer, attr->length, dg) == OK))
        {
          free (attr->buffer);
          return NotOK;
        }
    }

  attr->bps = 8;
  attr->stereo = 0;
  attr->rate = 8000;            /* Guess */
  return OK;
}

ALvoid *
alutLoadMemoryFromFile (const char *filename, ALenum *format,
                        ALsizei *size, ALfloat *freq)
{
  _alutSanityCheck ();
  return _alutPrivateLoadMemoryFromFile (filename, format, size, freq);
}

ALvoid *
alutLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                             ALenum *format, ALsizei *size, ALfloat *freq)
{
  _alutSanityCheck ();
  return _alutPrivateLoadMemoryFromFileImage (data, length, format, size,
                                              freq);
}
