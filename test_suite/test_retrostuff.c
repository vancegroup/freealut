#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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
  ALboolean loop;
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

  alutLoadWAVFile ((ALbyte *) "file1.wav", &format, &data, &size, &freq,
		   &loop);
  alGenBuffers (1, &buffer);
  alBufferData (buffer, format, data, size, freq);
  free (data);
  alGenSources (1, &handle);
  alSourcei (handle, AL_BUFFER, buffer);
  alSourcePlay (handle);
  sleep (2);

  fd = fopen ("file1.wav", "rb");
  fread (filebuffer, 1, 10000, fd);
  fclose (fd);

  alutLoadWAVMemory (filebuffer, &format, &data, &size, &freq, &loop);
  alGenBuffers (1, &buffer);
  alBufferData (buffer, format, data, size, freq);
  free (data);
  alGenSources (1, &handle);
  alSourcei (handle, AL_BUFFER, buffer);
  alSourcePlay (handle);
  sleep (2);

  alutExit ();
  return EXIT_SUCCESS;
}
