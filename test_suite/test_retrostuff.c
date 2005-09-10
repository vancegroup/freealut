#include <stdlib.h>
#include <stdio.h>
#include <AL/alut.h>

/*
  This program loads and plays a file the ALUT 0.x.x way.
*/

int
main (int argc, char **argv)
{
  ALenum format;
  ALsizei size;
  ALsizei freq;
#if !defined(__APPLE__)
  ALboolean loop;
#endif
  ALvoid *data;
  ALuint buffer;
  ALuint handle;
  FILE *fd;
  ALbyte filebuffer[100000];

  alutInit (&argc, argv);

  /*
     WARNING!!
     This is only a test program.
     It's testing a nasty, old fashioned way to load sounds.
     Whatever you do, don't pick this as an example of how to
     write ALUT programs!!
   */

  alutLoadWAVFile ((ALbyte *) "file1.wav", &format, &data, &size, &freq
#if !defined(__APPLE__)
                   , &loop
#endif
    );
  alGenBuffers (1, &buffer);
  alBufferData (buffer, format, data, size, freq);
  free (data);
  alGenSources (1, &handle);
  alSourcei (handle, AL_BUFFER, buffer);
  alSourcePlay (handle);
  alutMicroSleep (2000000);

  fd = fopen ("file1.wav", "rb");
  if (fd == NULL)
    {
      fprintf (stderr, "Error opening .wav file\n");
      exit (EXIT_FAILURE);
    }
  fread (filebuffer, 1, 10000, fd);
  if (ferror (fd))
    {
      fprintf (stderr, "Error reading .wav file\n");
      exit (EXIT_FAILURE);
    }
  fclose (fd);

  alutLoadWAVMemory (filebuffer, &format, &data, &size, &freq
#if !defined(__APPLE__)
                     , &loop
#endif
    );
  alGenBuffers (1, &buffer);
  alBufferData (buffer, format, data, size, freq);
  free (data);
  alGenSources (1, &handle);
  alSourcei (handle, AL_BUFFER, buffer);
  alSourcePlay (handle);
  alutMicroSleep (2000000);

  alutExit ();
  return EXIT_SUCCESS;
}
