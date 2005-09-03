#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(_WIN32)
#define stat(p,b) _stat((p),(b))
#else
#include <unistd.h>
#endif
#include <AL/alut.h>

#include "alutError.h"
#include "alutInit.h"

struct SampleAttribs
{
  ALsizei length;
  unsigned char *buffer;
  int bps;
  int stereo;
  int rate;                     /* ToDo: This should probably be an ALuint */
  char *comment;
};

struct DataGetter
{
  size_t length;                /* Total length of file */
  FILE *fd;                     /* For files: just the file descriptor */
  const ALvoid *data;           /* For memory: length, data pointer and position in data */
  size_t next;
};

/* Like fread except it reads from a DataGetter */

static size_t
dgread (void *ptr, size_t size, size_t nmemb, struct DataGetter *source)
{
  size_t len = size * nmemb;

  if (source->fd != NULL)
    {
      return fread (ptr, size, nmemb, source->fd);
    }

  if (source->data != NULL)
    {
      /* Attempt to read past end of block? */

      if (source->next + len >= source->length)
        {
          nmemb = (source->length - source->next) / size;
          len = size * nmemb;
        }

      memcpy (ptr, &(((char *) (source->data))[source->next]), len);
      source->next += len;

      return nmemb;
    }

  _alutSetError (ALUT_ERROR_INVALID_OPERATION);
  return 0;
}

static ALboolean _alutLoadWavFile (struct DataGetter *fd,
                                   struct SampleAttribs *attr);
static ALboolean _alutLoadAUFile (struct DataGetter *fd,
                                  struct SampleAttribs *attr);
static ALboolean _alutLoadRawFile (struct DataGetter *fd,
                                   struct SampleAttribs *attr);
static ALboolean _alutLoadFile (const char *fname, struct DataGetter *fd,
                                struct SampleAttribs *attr);

ALuint
alutCreateBufferFromFile (const char *filename)
{
  ALuint albuffer;
  struct SampleAttribs attr;
  struct DataGetter dg;
  struct stat stat_buf;
  FILE *fd;

  _alutSanityCheck ();

  attr.buffer = NULL;
  attr.comment = NULL;

  if (stat (filename, &stat_buf) != 0)
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_FOUND);
      return 0;
    }

  fd = fopen (filename, "rb");

  if (fd == NULL)
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_READABLE);
      return 0;
    }

  dg.fd = fd;
  dg.length = stat_buf.st_size;
  dg.data = NULL;
  dg.next = 0;

  if (!_alutLoadFile (filename, &dg, &attr))
    {
      return 0;
    }

  alGenBuffers (1, &albuffer);
  alBufferData (albuffer, attr.bps == 8 ? AL_FORMAT_MONO8 :
                AL_FORMAT_MONO16, attr.buffer, attr.length, attr.rate);
  free (attr.buffer);
  fclose (fd);
  return albuffer;
}

ALuint
alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length)
{
  ALuint albuffer;
  struct SampleAttribs attr;
  struct DataGetter dg;

  _alutSanityCheck ();

  attr.buffer = NULL;
  attr.comment = NULL;

  dg.fd = NULL;
  dg.length = length;
  dg.data = data;
  dg.next = 0;

  if (!_alutLoadFile ("", &dg, &attr))
    {
      return 0;
    }

  alGenBuffers (1, &albuffer);
  alBufferData (albuffer, attr.bps == 8 ? AL_FORMAT_MONO8 :
                AL_FORMAT_MONO16, attr.buffer, attr.length, attr.rate);
  free (attr.buffer);
  return albuffer;
}

static void *
_alutPrivateLoadMemoryFromFile (const char *filename, ALenum *format,
                                ALsizei *size, ALuint *freq)
{
  struct SampleAttribs attr;
  struct DataGetter dg;
  struct stat stat_buf;
  FILE *fd;

  attr.buffer = NULL;
  attr.comment = NULL;

  if (stat (filename, &stat_buf) != 0)
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_FOUND);
      return NULL;
    }

  fd = fopen (filename, "rb");

  if (fd == NULL)
    {
      _alutSetError (ALUT_ERROR_FILE_NOT_READABLE);
      return NULL;
    }

  dg.fd = fd;
  dg.length = stat_buf.st_size;
  dg.data = NULL;
  dg.next = 0;

  if (!_alutLoadFile (filename, &dg, &attr))
    {
      return NULL;
    }

  if (size != NULL)
    {
      *size = attr.length;
    }
  if (format != NULL)
    {
      *format = (attr.bps == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
  if (freq != NULL)
    {
      *freq = (ALuint) attr.rate;
    }

  return attr.buffer;
}

static void *
_alutPrivateLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                                     ALenum *format, ALsizei *size,
                                     ALuint *freq)
{
  struct SampleAttribs attr;
  struct DataGetter dg;

  attr.buffer = NULL;
  attr.comment = NULL;

  dg.fd = NULL;
  dg.length = length;
  dg.data = data;
  dg.next = 0;

  if (!_alutLoadFile ("", &dg, &attr))
    {
      return NULL;
    }

  if (size != NULL)
    {
      *size = attr.length;
    }
  if (format != NULL)
    {
      *format = (attr.bps == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
  if (freq != NULL)
    {
      *freq = (ALuint) attr.rate;
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
                 ALsizei *freq, ALboolean *loop)
{
  ALuint frequency;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data = _alutPrivateLoadMemoryFromFile (filename, format, size, &frequency);

  if (freq)
    {
      *freq = (ALsizei) frequency;
    }
  if (loop)
    {
      *loop = AL_FALSE;
    }
}

void
alutLoadWAVMemory (ALbyte *buffer, ALenum *format, void **data, ALsizei *size,
                   ALsizei *freq, ALboolean *loop)
{
  ALuint frequency;

  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  *data = _alutPrivateLoadMemoryFromFileImage ((const ALvoid *) buffer, 0x7FFFFFFF,     /* Eeek! */
                                               format, size, &frequency);
  if (freq)
    {
      *freq = (ALsizei) frequency;
    }
  if (loop)
    {
      *loop = AL_FALSE;
    }
}

void
alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei freq)
{
  /* Don't do an _alutSanityCheck () because it's not required in ALUT 0.x.x */

  free (data);
}

static void
swap_Ushort (unsigned short *i)
{
  *i = ((*i << 8) & 0xFF00) + ((*i >> 8) & 0x00FF);
}

static void
swap_int (int *i)
{
  *i = ((*i << 24) & 0xFF000000) +
    ((*i << 8) & 0x00FF0000) +
    ((*i >> 8) & 0x0000FF00) + ((*i >> 24) & 0x000000FF);
}

static ALboolean
_alutStrEqual (const char *a, const char *b)
{
#ifdef _WIN32
  return stricmp (a, b) == 0;
#else
  return strcasecmp (a, b) == 0;
#endif
}

static ALboolean
_alutLoadFile (const char *fname, struct DataGetter *dg,
               struct SampleAttribs *attr)
{
  char magic[4];

  /* Raw files have no magic number - so use the filename extension */

  if (_alutStrEqual (&fname[strlen (fname) - 4], ".raw"))
    {
      return _alutLoadRawFile (dg, attr);
    }

  /* For other file formats, read the quasi-standard four byte magic number */

  if (dgread (magic, 4, 1, dg) == 0)
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return AL_FALSE;
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
  return AL_FALSE;
}

static ALboolean
_alutLoadWavFile (struct DataGetter *dg, struct SampleAttribs *attr)
{
  ALboolean found_header = AL_FALSE;
  ALboolean needs_swabbing = AL_FALSE;
  int leng1;
  char magic[4];

  if (attr->buffer)
    {
      free (attr->buffer);
    }
  attr->buffer = NULL;
  attr->length = 0;

  if (dgread (&leng1, sizeof (int), 1, dg) == 0)
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return AL_FALSE;
    }

  dgread (magic, 4, 1, dg);

  if (magic[0] != 'W' || magic[1] != 'A' ||
      magic[2] != 'V' || magic[3] != 'E')
    {
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return AL_FALSE;
    }

  while (1)
    {
      unsigned short header[8];
      int junk;
      int len;

      len = dgread (magic, 4, 1, dg);

      if (len <= 0)
        {
          _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
          return AL_FALSE;
        }

      if (magic[0] == 'f' && magic[1] == 'm' &&
          magic[2] == 't' && magic[3] == ' ')
        {
          found_header = AL_TRUE;

          if (dgread (&leng1, sizeof (int), 1, dg) == 0)
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
              return AL_FALSE;
            }

          if (leng1 > 65536)
            {
              needs_swabbing = AL_TRUE;
              swap_int (&leng1);
            }

          if (leng1 != sizeof (header))
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
            }

          dgread (&header, sizeof (header), 1, dg);

          for (junk = sizeof (header); junk < leng1; junk++)
            {
              char throw_away;
              dgread (&throw_away, 1, 1, dg);
            }

          if (needs_swabbing)
            {
              swap_Ushort (&header[0]);
              swap_Ushort (&header[1]);
              swap_int ((int *) &header[2]);
              swap_int ((int *) &header[4]);
              swap_Ushort (&header[6]);
              swap_Ushort (&header[7]);
            }

          if (header[0] != 0x0001)
            {
              _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
              return AL_FALSE;
            }

          attr->stereo = (header[1] > 1);
          attr->rate = (*((int *) (&header[2])));
          attr->bps = (header[7]);
        }
      else
        if (magic[0] == 'd' && magic[1] == 'a' &&
            magic[2] == 't' && magic[3] == 'a')
        {
          if (!found_header)
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
              return AL_FALSE;
            }

          if (dgread (&attr->length, sizeof (int), 1, dg) == 0)
            {
              _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
              return AL_FALSE;
            }

          if (needs_swabbing)
            {
              swap_int (&attr->length);
            }

          attr->buffer = (unsigned char *) malloc (attr->length);

          dgread (attr->buffer, 1, attr->length, dg);
          return AL_TRUE;
        }
    }
}

typedef ALvoid *codec (ALvoid *data, unsigned int *length);

static codec uLaw, pcm8, pcm16, aLaw;

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
_alutLoadAUFile (struct DataGetter *dg, struct SampleAttribs *attr)
{
  int hdr_length;
  int dat_length;
  int encoding;
  int irate;
  int nchans;
  codec *_codec;

  if (attr->buffer)
    {
      free (attr->buffer);
    }
  attr->buffer = NULL;
  attr->length = 0;

  if (dgread (&hdr_length, sizeof (int), 1, dg) == 0 ||
      dgread (&dat_length, sizeof (int), 1, dg) == 0 ||
      dgread (&encoding, sizeof (int), 1, dg) == 0 ||
      dgread (&irate, sizeof (int), 1, dg) == 0 ||
      dgread (&nchans, sizeof (int), 1, dg) == 0)
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return AL_FALSE;
    }

  if (hdr_length > 65536)
    {
      swap_int (&hdr_length);
      swap_int (&dat_length);
      swap_int (&encoding);
      swap_int (&irate);
      swap_int (&nchans);
    }

  switch (encoding)
    {
    case AU_ULAW_8:
      attr->bps = 16;
      _codec = uLaw;
      break;

    case AU_PCM_8:
      attr->bps = 8;
      _codec = pcm8;
      break;

    case AU_PCM_16:
      attr->bps = 16;
      _codec = pcm16;
      break;

    case AU_ALAW_8:
      attr->bps = 16;
      _codec = aLaw;
      break;

    default:
      _alutSetError (ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE);
      return AL_FALSE;
    }

  attr->stereo = (nchans > 1);
  attr->rate = irate;

#if 0
  fprintf (stderr,
           "hdr_length %d, dat_length %d, encoding %d, bps %d, stereo %d, rate %d\n",
           hdr_length, dat_length, encoding, attr->bps, attr->stereo,
           attr->rate);
#endif

  if (hdr_length > 512 || hdr_length < 24 ||
      irate > 65526 || irate <= 1000 || nchans < 1 || nchans > 2)
    {
      _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
      return AL_FALSE;
    }

  if (hdr_length > 24)
    {
      if (attr->comment)
        free (attr->comment);
      attr->comment = NULL;
      attr->comment = (char *) malloc (hdr_length - 24 + 1);

      dgread (attr->comment, 1, hdr_length - 24, dg);
      (attr->comment)[hdr_length - 24] = '\0';
    }

  if (dat_length > 0)
    {
      char *buf = (char *) malloc (dat_length);
      unsigned int len = dgread (buf, 1, dat_length, dg);

      if (len != dat_length)
        {
          _alutSetError (ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE);
          return AL_FALSE;
        }

      attr->buffer = _codec (buf, &len);
      attr->length = len;
    }

  return AL_TRUE;
}

ALvoid *
pcm8 (ALvoid *data, unsigned int *length)
{
  int8_t *d = (int8_t *) data;
  unsigned int l = *length;
  unsigned int i;
  for (i = 0; i < l; i++)
    {
      d[i] = d[i] + 128;
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
      swap_Ushort ((unsigned short *) &d[i]);
    }
  return data;
}

/*
 * From: http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1
 */
static int16_t
mulaw2linear (uint8_t mulawbyte)
{
  const static int16_t exp_lut[8] =
    { 0, 132, 396, 924, 1980, 4092, 8316, 16764 };
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
  a_val = 0x55;
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
      attr->length = dgread (attr->buffer, 1, attr->length, dg);
    }

  attr->bps = 8;
  attr->stereo = 0;
  attr->rate = 8000;            /* Guess */

  return AL_TRUE;
}

ALvoid *
alutLoadMemoryFromFile (const char *filename, ALenum *format, ALsizei *size,
                        ALuint *freq)
{
  _alutSanityCheck ();
  return _alutPrivateLoadMemoryFromFile (filename, format, size, freq);
}

ALvoid *
alutLoadMemoryFromFileImage (const ALvoid *data, ALsizei length,
                             ALenum *format, ALsizei *size, ALuint *freq)
{
  _alutSanityCheck ();
  return _alutPrivateLoadMemoryFromFileImage (data, length,
                                              format, size, freq);
}
