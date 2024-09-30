#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=88.8889045715332,469.8889265060425
AudioSynthWaveform       waveform1;      //xy=162.88888549804688,205
AudioMixer4              mixer1;         //xy=230.88888931274414,473.8889274597168
AudioEffectEnvelope      envelope1;      //xy=324.8888854980469,205
AudioAnalyzePeak         peak1;          //xy=365.8888931274414,311.8888921737671
AudioAnalyzeRMS          rms1;           //xy=367.8888931274414,360.8888931274414
AudioAnalyzeNoteFrequency notefreq1;      //xy=463.8888931274414,435.88892555236816
AudioOutputI2S           i2s1;           //xy=632.8888854980469,223
AudioConnection          patchCord1(i2s2, 0, mixer1, 0);
AudioConnection          patchCord2(i2s2, 1, mixer1, 1);
AudioConnection          patchCord3(waveform1, envelope1);
AudioConnection          patchCord4(mixer1, peak1);
AudioConnection          patchCord5(mixer1, rms1);
AudioConnection          patchCord6(mixer1, notefreq1);
AudioConnection          patchCord7(envelope1, 0, i2s1, 0);
AudioConnection          patchCord8(envelope1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=574.8888854980469,313
// GUItool: end automatically generated code
