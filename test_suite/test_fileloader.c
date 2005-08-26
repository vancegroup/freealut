
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <AL/alut.h>

/*
  This program loads and plays a variety of files.
*/

int main ( int argc, char **argv )
{
  ALuint file1Buffer, file1Source ;
  ALuint file2Buffer, file2Source ;
  ALuint file3Buffer, file3Source ;

  alutInit ( & argc, argv ) ;

  file1Buffer = alutCreateBufferFromFile ( "file1.wav" ) ;
  if ( file1Buffer == 0 )
    fprintf(stderr, "Error loading wav file: '%s'\n",
                    alutGetErrorString ( alutGetError () ) ) ;

  alGenSources ( 1, &file1Source ) ;
  alSourcei ( file1Source, AL_BUFFER, file1Buffer ) ;

  file2Buffer = alutCreateBufferFromFile ( "file2.au" ) ;
  if ( file2Buffer == 0 )
    fprintf(stderr, "Error loading .au file: '%s'\n",
                    alutGetErrorString ( alutGetError () ) ) ;

  alGenSources ( 1, &file2Source ) ;
  alSourcei ( file2Source, AL_BUFFER, file2Buffer ) ;

  file3Buffer = alutCreateBufferFromFile ( "file3.raw" ) ;
  if ( file3Buffer == 0 )
    fprintf(stderr, "Error loading .ub file: '%s'\n",
                    alutGetErrorString ( alutGetError () ) ) ;

  alGenSources ( 1, &file3Source ) ;
  alSourcei ( file3Source, AL_BUFFER, file3Buffer ) ;

  while ( 1 )
  {
    alSourcePlay ( file1Source ) ; sleep ( 1 ) ;
    alSourcePlay ( file2Source ) ; sleep ( 1 ) ;
    alSourcePlay ( file3Source ) ; sleep ( 1 ) ;
  }

  alutExit () ;
  exit ( -1 ) ;
}


