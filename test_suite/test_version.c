
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <AL/alut.h>

/*
  This program checks that the version of OpenAL in the
  library agrees with the header file we're compiled against.
*/

int main ( int argc, char **argv )
{
  alutInit ( & argc, argv ) ;

#ifdef ALUT_API_MAJOR_VERSION
  if ( alutGetMajorVersion () != ALUT_API_MAJOR_VERSION ||
       alutGetMinorVersion () != ALUT_API_MINOR_VERSION )
    fprintf ( stderr, "WARNING: The ALUT library is version %d.%d.x but the alut.h says it's %d.%d.x!!\n", 
       alutGetMajorVersion (), alutGetMinorVersion (),
       ALUT_API_MAJOR_VERSION, ALUT_API_MINOR_VERSION ) ;
  else
    fprintf ( stderr, "The ALUT library is at version %d.%d.x.\n", 
       alutGetMajorVersion (), alutGetMinorVersion () ) ;
#else
  fprintf ( stderr, "WARNING: Your copy of AL/alut.h is pre-1.0.0\n" ) ;
  fprintf ( stderr, "But you are running the ALUT test suite from ALUT\n" ) ;
  fprintf ( stderr, "version 1.0.0 or later.\n" ) ;
#endif

  alutExit () ;
  exit ( -1 ) ;
}


