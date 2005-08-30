#ifndef __ALUT_H__
#define __ALUT_H__  1

#include <AL/al.h>
#include <AL/alc.h>

#define ALUT_API_MAJOR_VERSION 1
#define ALUT_API_MINOR_VERSION 0

#define ALUT_WAVEFORM_SINE                     0x100
#define ALUT_WAVEFORM_SQUARE                   0x101
#define ALUT_WAVEFORM_SAWTOOTH                 0x102
#define ALUT_WAVEFORM_WHITENOISE               0x103
#define ALUT_WAVEFORM_IMPULSE                  0x104

#define ALUT_ERROR_NO_ERROR                    0
#define ALUT_ERROR_OUT_OF_MEMORY               0x201
#define ALUT_ERROR_INVALID_ENUM                0x202
#define ALUT_ERROR_INVALID_VALUE               0x203
#define ALUT_ERROR_INVALID_OPERATION           0x204
#define ALUT_ERROR_NOT_INITIALISED             0x205
#define ALUT_ERROR_NO_DEVICE_AVAILABLE         0x206
#define ALUT_ERROR_NO_CONTEXT_AVAILABLE        0x207
#define ALUT_ERROR_FILE_NOT_FOUND              0x208
#define ALUT_ERROR_FILE_NOT_READABLE           0x209
#define ALUT_ERROR_UNSUPPORTED_FILE_TYPE       0x20A
#define ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE    0x20B
#define ALUT_ERROR_CORRUPT_OR_TRUNCATED_FILE   0x20C

#ifdef __cplusplus
extern "C" {
#endif

extern ALboolean   alutInit                        ( ALint      *argcp,
                                                     char      **argv ) ;
extern ALboolean   alutInitWithoutContext          ( ALint      *argcp,
                                                     char      **argv ) ;
extern void        alutExit                        () ;

extern ALint       alutGetError                    () ;
extern const char *alutGetErrorString              ( ALint       error ) ;

extern ALuint      alutCreateBufferFromFile        ( const char *filename ) ;
extern ALuint      alutCreateBufferFromFileImage   ( const unsigned char *data,
                                                     ALsizei     length ) ;
extern void       *alutLoadMemoryFromFile          ( const char *filename,
                                                     ALenum     *format,
                                                     ALsizei    *size,
                                                     float      *freq ) ;
extern void       *alutLoadMemoryFromFileImage     ( const unsigned char *data,
                                                     ALsizei     length,
                                                     ALenum     *format,
                                                     ALsizei    *size,
                                                     float      *freq ) ;
extern const char *alutEnumerateSupportedFileTypes () ;
extern ALuint      alutCreateBufferHelloWorld      () ;
extern ALuint      alutCreateBufferWaveform        ( ALenum      waveshape,
                                                     float       frequency,
                                                     float       phase,
                                                     float       duration ) ;
extern ALint       alutGetMajorVersion             () ;
extern ALint       alutGetMinorVersion             () ;

/* Private ALUT functions - not for general use */

extern void  _alutSetError ( ALint errorcode ) ;
extern void  _alutSanityCheck () ;

/* Nasty Compatibility stuff */

/* WARNING: THESE FUNCTIONS ARE STRONGLY DEPRECATED */

extern void alutLoadWAVFile ( const char *filename,
                              ALenum *format,
                              void **data,
                              ALsizei *size,
                              ALsizei *freq,
                              ALboolean *loop ) ;

extern void alutLoadWAVMemory ( const char *buffer,
                                ALenum *format,
                                void **data,
                                ALsizei *size,
                                ALsizei *freq,
                                ALboolean *loop ) ;

extern void alutUnloadWAV ( ALenum format,
                            ALvoid *data,
                            ALsizei size,
                            ALsizei freq ) ;

#ifdef __cplusplus
} ;
#endif

#endif

