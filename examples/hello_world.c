
#include <stdlib.h>
#include <unistd.h>
#include <AL/alut.h>

/*
  This is the 'Hello World' program from the ALUT
  reference manual.

  Link using '-lalut -lopenal -lpthread'.
*/

int main ( int argc, char **argv )
{
  ALuint helloBuffer, helloSource ;
  alutInit ( & argc, argv ) ;
  helloBuffer = alutCreateBufferHelloWorld () ;
  alGenSources ( 1, &helloSource ) ;
  alSourcei ( helloSource, AL_BUFFER, helloBuffer ) ;
  alSourcePlay ( helloSource ) ;
  sleep ( 2 ) ;
  alutExit () ;
  exit ( -1 ) ;
}


