/*
 * This file should be #included as the first header in all *.c files.
 */

#if !defined(ALUT_INTERNAL_H)
#define ALUT_INTERNAL_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_BASETSD_H
#include <basetsd.h>
typedef INT8 int8_t;
typedef UINT8 uint8_t;
typedef INT16 int16_t;
typedef UINT16 uint16_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;
#else
#error Do not know sized types on this platform
#endif

typedef uint16_t UInt16LittleEndian;
typedef int32_t Int32BigEndian;
typedef uint32_t UInt32LittleEndian;

#if HAVE___ATTRIBUTE__
#define UNUSED(x) x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#include <AL/alut.h>

/* in alutError.c */
extern void _alutSetError (ALenum err);

/* in alutInit.c */
extern ALboolean _alutSanityCheck (void);

/* in alutInputStream.c */
typedef struct InputStream_struct InputStream;
extern InputStream *_alutStreamConstructFromFile (const char *fileName);
extern InputStream *_alutStreamConstructFromMemory (const ALvoid *data,
                                                    ALsizei length);
extern const char *_alutStreamGetFileName (const InputStream *stream);
extern size_t _alutStreamGetRemainingLength (const InputStream *stream);
extern ALboolean _alutStreamDestroy (InputStream *stream);
extern ALboolean _alutStreamEOF (InputStream *stream);
extern ALboolean _alutStreamRead (InputStream *stream, void *ptr,
                                  size_t numBytesToRead);
extern ALboolean _alutStreamSkip (InputStream *stream, size_t numBytesToSkip);
extern ALboolean _alutStreamReadUInt16LE (InputStream *stream,
                                          UInt16LittleEndian *value);
extern ALboolean _alutStreamReadInt32BE (InputStream *stream,
                                         Int32BigEndian *value);
extern ALboolean _alutStreamReadUInt32LE (InputStream *stream,
                                          UInt32LittleEndian *value);

/* in alutUtil.c */
extern ALvoid *_alutMalloc (size_t size);

/* in alutWaveform.c */
typedef struct BufferData_struct BufferData;
extern BufferData *_alutBufferDataConstruct (ALvoid *data, ALsizei length,
                                             ALint numChannels,
                                             ALint bitsPerSample,
                                             ALfloat sampleFrequency);
extern ALboolean _alutBufferDataDestroy (BufferData *bufferData);
extern void _alutBufferDataDetachData (BufferData *bufferData);
extern ALvoid *_alutBufferDataGetData (const BufferData *bufferData);
extern ALsizei _alutBufferDataGetLength (const BufferData *bufferData);
extern ALfloat _alutBufferDataGetSampleFrequency (const BufferData
                                                  *bufferData);
extern ALboolean _alutGetFormat (const BufferData *bufferData,
                                 ALenum *format);
extern ALuint _alutPassBufferData (BufferData *bufferData);

#endif /* not ALUT_INTERNAL_H */
