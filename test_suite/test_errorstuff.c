#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <AL/alut.h>

/*
  This is a minimal test of error handling.
*/

int
main (int argc, char **argv)
{
  ALuint file1Buffer;

  alutInit (&argc, argv);

  file1Buffer = alutCreateBufferFromFile ("no_such_file_in_existance.wav");

  if (file1Buffer == 0)
    {
      fprintf (stderr, "Error loading wav file: '%s'\n",
	       alutGetErrorString (alutGetError ()));
      exit (EXIT_FAILURE);
    }

  alutExit ();
  return EXIT_SUCCESS;
}
