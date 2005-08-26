
#include <stdlib.h>
#include <unistd.h>
#include <AL/alut.h>

/*
  This program plays a 440Hz tone using a variety of waveforms.
*/

int main ( int argc, char **argv )
{
  ALuint beep1Buffer, beep1Source ;
  ALuint beep2Buffer, beep2Source ;
  ALuint beep3Buffer, beep3Source ;
  ALuint beep4Buffer, beep4Source ;
  ALuint beep5Buffer, beep5Source ;

  alutInit ( & argc, argv ) ;

  beep1Buffer = alutCreateBufferWaveform ( ALUT_WAVEFORM_SINE,
                                          440.0f, 0.0f, 1.0f ) ;
  alGenSources ( 1, &beep1Source ) ;
  alSourcei ( beep1Source, AL_BUFFER, beep1Buffer ) ;

  beep2Buffer = alutCreateBufferWaveform ( ALUT_WAVEFORM_SQUARE,
                                          440.0f, 0.0f, 1.0f ) ;
  alGenSources ( 1, &beep2Source ) ;
  alSourcei ( beep2Source, AL_BUFFER, beep2Buffer ) ;

  beep3Buffer = alutCreateBufferWaveform ( ALUT_WAVEFORM_SAWTOOTH,
                                          440.0f, 0.0f, 1.0f ) ;
  alGenSources ( 1, &beep3Source ) ;
  alSourcei ( beep3Source, AL_BUFFER, beep3Buffer ) ;

  beep4Buffer = alutCreateBufferWaveform ( ALUT_WAVEFORM_WHITENOISE,
                                          440.0f, 0.0f, 1.0f ) ;
  alGenSources ( 1, &beep4Source ) ;
  alSourcei ( beep4Source, AL_BUFFER, beep4Buffer ) ;

  beep5Buffer = alutCreateBufferWaveform ( ALUT_WAVEFORM_IMPULSE,
                                          440.0f, 0.0f, 1.0f ) ;
  alGenSources ( 1, &beep5Source ) ;
  alSourcei ( beep5Source, AL_BUFFER, beep5Buffer ) ;

  while ( 1 )
  {
    alSourcePlay ( beep1Source ) ; sleep ( 1 ) ;
    alSourcePlay ( beep2Source ) ; sleep ( 1 ) ;
    alSourcePlay ( beep3Source ) ; sleep ( 1 ) ;
    alSourcePlay ( beep4Source ) ; sleep ( 1 ) ;
    alSourcePlay ( beep5Source ) ; sleep ( 1 ) ;
  }

  alutExit () ;
  exit ( -1 ) ;
}


