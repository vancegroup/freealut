#include <stdlib.h>
#include <AL/alut.h>

#if defined(_WIN32)
#define sleep(x) Sleep(1000*(x))
#else
#include <unistd.h>
#endif

/*
  This is the 'Hello World' program from the ALUT
  reference manual.

  Link using '-lalut -lopenal -lpthread'.
*/

int
main (int argc, char **argv)
{
  ALuint helloBuffer, helloSource;
  alutInit (&argc, argv);
  helloBuffer = alutCreateBufferHelloWorld ();
  alGenSources (1, &helloSource);
  alSourcei (helloSource, AL_BUFFER, helloBuffer);
  alSourcePlay (helloSource);
  sleep (3);
  alutExit ();
  return EXIT_SUCCESS;
}
