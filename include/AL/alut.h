#ifndef _AL_ALUT_H
#define _AL_ALUT_H

#include <AL/al.h>
#include <AL/alc.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32) && !defined(_XBOX)
 #if defined (_OPENAL32LIB)
  #define ALUTAPI __declspec(dllexport)
 #else
  #define ALUTAPI __declspec(dllimport)
 #endif
#else
 #define ALUTAPI extern
#endif

#if defined(_WIN32)
 #define ALUTAPIENTRY __cdecl
#else
 #define ALUTAPIENTRY
#endif

#if defined(__MWERKS_)
 #pragma export on
#endif

#define ALUT_API_MAJOR_VERSION                 1
#define ALUT_API_MINOR_VERSION                 0

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

ALUTAPI ALboolean ALUTAPIENTRY alutInit (int *argcp, char **argv);
ALUTAPI ALboolean ALUTAPIENTRY alutInitWithoutContext (int *argcp, char **argv);
ALUTAPI void ALUTAPIENTRY alutExit ();

ALUTAPI ALenum ALUTAPIENTRY alutGetError ();
ALUTAPI const char * ALUTAPIENTRY alutGetErrorString (ALenum error);

ALUTAPI ALuint ALUTAPIENTRY alutCreateBufferFromFile (const char *filename);
ALUTAPI ALuint ALUTAPIENTRY alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length);
ALUTAPI void * ALUTAPIENTRY alutLoadMemoryFromFile (const char *filename, ALenum *format, ALsizei *size, ALuint *freq);
ALUTAPI void * ALUTAPIENTRY alutLoadMemoryFromFileImage (const ALvoid *data, ALsizei length, ALenum *format, ALsizei *size, ALuint *freq);
ALUTAPI const char * ALUTAPIENTRY alutEnumerateSupportedFileTypes ();

ALUTAPI ALuint ALUTAPIENTRY alutCreateBufferHelloWorld ();
ALUTAPI ALuint ALUTAPIENTRY alutCreateBufferWaveform (ALenum waveshape, float frequency, float phase, float duration);

ALUTAPI ALint ALUTAPIENTRY alutGetMajorVersion ();
ALUTAPI ALint ALUTAPIENTRY alutGetMinorVersion ();

/* Private ALUT functions - not for general use */

ALUTAPI void ALUTAPIENTRY _alutSetError (ALint errorcode);
ALUTAPI void ALUTAPIENTRY _alutSanityCheck ();

#if !defined(_WIN32)
/* Nasty Compatibility stuff, WARNING: THESE FUNCTIONS ARE STRONGLY DEPRECATED */

ALUTAPI void ALUTAPIENTRY alutLoadWAVFile (ALbyte *filename, ALenum *format, void **data, ALsizei *size, ALsizei *freq, ALboolean *loop);
ALUTAPI void ALUTAPIENTRY alutLoadWAVMemory (ALbyte *buffer, ALenum *format, void **data, ALsizei *size, ALsizei *freq, ALboolean *loop);
ALUTAPI void ALUTAPIENTRY alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei freq);

#endif /* not _WIN32 */

#if defined(__MWERKS_)
 #pragma export off
#endif

#if defined(__cplusplus)
}
#endif

#endif
