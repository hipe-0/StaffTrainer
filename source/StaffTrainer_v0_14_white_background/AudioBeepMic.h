#include "AudioControl.h"
#include "iferror.h"
#include "AudioImport.h"

#define NOTE_PLAUSABILITY_THRESHOLD 0.6
#define RMS_THRESHOLD 0.02
#define RATIO_PITCH_BETWEEN_TWO_CONSEQUTIVE_NOTES 0.1
#define MIN_FREQUENCY 70

class AudioBeepMic
{
private:
  float oldFreq_;

 float computeFrequency_(int note){
    return 440.0 * pow(2.0, (note + 3) / 12.0);
  }

  float computeNote_(float freq){
    return 12.0 * log2(freq / 440.0) - 3;
  }


public:
  void init() {
    AudioMemory(60);
    sgtl5000_1.enable();
    //sgtl5000_1.volume(0.01);
    sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
    //sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    //sgtl5000_1.inputLevel(1.0);
    sgtl5000_1.micGain(36);
    //sgtl5000_1.lineInLevel(10.0);


    //envelope1.delay  (0);
    envelope1.attack (10);
    envelope1.decay  (50);
    envelope1.sustain(0.1);
    envelope1.release(10);

    waveform1.begin( WAVEFORM_SAWTOOTH );
    waveform1.amplitude( 0.1 );
    

    mixer1.gain(0, 1.0);
    mixer1.gain(1, 1.0);

    notefreq1.begin(NOTE_PLAUSABILITY_THRESHOLD);
  }

  void playBeep(int note, long durationMs) {

    long frequencyHz = computeFrequency_(note);
    DEBUGVAL("frequencyHz",frequencyHz)

    waveform1.frequency(frequencyHz);
    envelope1.noteOn();

    delay(durationMs);

    envelope1.noteOff();
    //delay(100);


    DEBUGTXT("playBeep stopped")
  }

  int getMicHearingNote()
  {
    float freq = -1;
    float rms = getMicRMS();
    float note = -100;

    if(rms > 0)
    {
      if(notefreq1.available() )
      {
        float ftemp = notefreq1.read();
        if ( rms > RMS_THRESHOLD && ftemp > MIN_FREQUENCY)
        {
          float ratio = ftemp / oldFreq_;

          if (abs(1-ratio)< RATIO_PITCH_BETWEEN_TWO_CONSEQUTIVE_NOTES )
          {
            freq = ftemp;
            note = computeNote_(freq);

            Serial.print(freq);            
            Serial.print(",");
            Serial.println(note);
          }
        }

        if(freq == -1 && oldFreq_ > 0) 
          Serial.println(0);            

        oldFreq_ = ftemp;
     }
    }

    return round(note);
  }

  float getMicRMS()
  {
    if(rms1.available())
      return rms1.read();
    else
      return -1;
  }

};