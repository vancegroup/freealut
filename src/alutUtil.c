#include "alutInternal.h"

#if HAVE_NANOSLEEP && HAVE_TIME_H
#include <time.h>
#elif HAVE_USLEEP && HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_SLEEP && HAVE_WINDOWS_H
#include <windows.h>
#else
#error No way to sleep on this platform
#endif

void
alutMicroSleep (ALuint microSeconds)
{
#if HAVE_NANOSLEEP && HAVE_TIME_H
  ALuint seconds = microSeconds / 1000000;
  ALuint microSecs = microSeconds - (seconds * 1000000);
  struct timespec t;
  t.tv_sec = (time_t) seconds;
  t.tv_nsec = ((long) microSecs) * 1000;
  nanosleep (&t, NULL);
#elif HAVE_USLEEP && HAVE_UNISTD_H
  usleep (microSeconds);
#elif HAVE_SLEEP && HAVE_WINDOWS_H
  Sleep (microSeconds / 1000);
#endif
}
