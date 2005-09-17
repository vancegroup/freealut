#if !defined(AL_ALUT_H)
#define AL_ALUT_H

#include <AL/al.h>
#include <AL/alc.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32) && !defined(_XBOX)
 #if defined (ALUT_BUILD_LIBRARY)
  #define ALUT_API __declspec(dllexport)
 #else
  #define ALUT_API __declspec(dllimport)
 #endif
#else
 #define ALUT_API extern
#endif

#if defined(_WIN32)
 #define ALUT_APIENTRY __cdecl
#else
 #define ALUT_APIENTRY
#endif

#if defined(__MWERKS_)
 #pragma export on
#endif

#define ALUT_API_MAJOR_VERSION                 1
#define ALUT_API_MINOR_VERSION                 0

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

#define ALUT_WAVEFORM_SINE                     0x100
#define ALUT_WAVEFORM_SQUARE                   0x101
#define ALUT_WAVEFORM_SAWTOOTH                 0x102
#define ALUT_WAVEFORM_WHITENOISE               0x103
#define ALUT_WAVEFORM_IMPULSE                  0x104

ALUT_API ALboolean ALUT_APIENTRY alutInit (int *argcp, char **argv);
ALUT_API ALboolean ALUT_APIENTRY alutInitWithoutContext (int *argcp, char **argv);
ALUT_API void ALUT_APIENTRY alutExit (void);

ALUT_API ALenum ALUT_APIENTRY alutGetError (void);
ALUT_API const char * ALUT_APIENTRY alutGetErrorString (ALenum error);

ALUT_API ALuint ALUT_APIENTRY alutCreateBufferFromFile (const char *filename);
ALUT_API ALuint ALUT_APIENTRY alutCreateBufferFromFileImage (const ALvoid *data, ALsizei length);
ALUT_API ALvoid * ALUT_APIENTRY alutLoadMemoryFromFile (const char *filename, ALenum *format, ALsizei *size, ALfloat *freq);
ALUT_API ALvoid * ALUT_APIENTRY alutLoadMemoryFromFileImage (const ALvoid *data, ALsizei length, ALenum *format, ALsizei *size, ALfloat *freq);
ALUT_API const char * ALUT_APIENTRY alutEnumerateSupportedFileTypes (void);

ALUT_API ALuint ALUT_APIENTRY alutCreateBufferHelloWorld (void);
ALUT_API ALuint ALUT_APIENTRY alutCreateBufferWaveform (ALenum waveshape, ALfloat frequency, ALfloat phase, ALfloat duration);

ALUT_API ALint ALUT_APIENTRY alutGetMajorVersion (void);
ALUT_API ALint ALUT_APIENTRY alutGetMinorVersion (void);

ALUT_API void ALUT_APIENTRY alutMicroSleep (ALuint microSeconds);

/* Nasty Compatibility stuff, WARNING: THESE FUNCTIONS ARE STRONGLY DEPRECATED */
#if defined(__APPLE__)
ALUT_API void ALUT_APIENTRY alutLoadWAVFile (ALbyte *filename, ALenum *format, void **data, ALsizei *size, ALsizei *freq);
ALUT_API void ALUT_APIENTRY alutLoadWAVMemory (ALbyte *buffer, ALenum *format, void **data, ALsizei *size, ALsizei *freq);
#else
ALUT_API void ALUT_APIENTRY alutLoadWAVFile (ALbyte *filename, ALenum *format, void **data, ALsizei *size, ALsizei *freq, ALboolean *loop);
ALUT_API void ALUT_APIENTRY alutLoadWAVMemory (ALbyte *buffer, ALenum *format, void **data, ALsizei *size, ALsizei *freq, ALboolean *loop);
#endif
ALUT_API void ALUT_APIENTRY alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei freq);

#if defined(__MWERKS_)
 #pragma export off
#endif

#if defined(__cplusplus)
}
#endif

#endif
