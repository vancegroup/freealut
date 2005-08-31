
#include <stdlib.h>
#include <AL/alut.h>

#ifndef _WIN32
#include <unistd.h> // ***** GH needed now?
#endif

/*
  This is the 'Hello World' program from the ALUT
  reference manual.

  Link using '-lalut -lopenal -lpthread'.
*/

int main ( int argc, char **argv )
{
  ALuint helloBuffer, helloSource, state ;
  alutInit ( & argc, argv ) ;
  helloBuffer = alutCreateBufferHelloWorld () ;
  alGenSources ( 1, &helloSource ) ;
  alSourcei ( helloSource, AL_BUFFER, helloBuffer ) ;
  alSourcePlay ( helloSource ) ;
  do {
	alGetSourcei( helloSource, AL_SOURCE_STATE, &state );
  } while (state == AL_PLAYING);
  alutExit () ;
  exit ( -1 ) ;
}


