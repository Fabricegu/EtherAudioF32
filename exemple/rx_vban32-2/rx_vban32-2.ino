#include <Audio.h>

#ifdef HAVE_AUDIO_BOARD
  #include <Wire.h>
  #include <SPI.h>
  #include <SD.h>
  #include <SerialFlash.h>
#endif

#include "control_ethernet.h"
#include "input_net.h"
#include "OpenAudio_ArduinoLibrary.h"


const float sample_rate_Hz = 48000.0f;
const int   audio_block_samples = 128;  // Always 128, which is AUDIO_BLOCK_SAMPLES from AudioStream.h
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

AudioOutputI2S_F32 audioOutput(audio_settings);

AudioControlEthernet   ether1;
AudioInputNet         in1(1);

//create audio connections
AudioConnection_F32      patchCord1(in1, 0, audioOutput, 0);
AudioConnection_F32      patchCord2(in1, 0, audioOutput, 1);
 AudioControlSGTL5000  sgtl;


#include "utils.h" // diagnostic print functions - assumes "AudioControlEthernet  ether1;""
void setup() 
{
  AudioMemory_F32(50); // more than required

  Serial.begin(115200);
  while (!Serial && millis() < 5000) 
  {
    delay(10);
  }
  Serial.println("\n\nStarting Multi Audio Stream Test");

  char myHost[] = "Teensy1";
  ether1.setHostName(myHost);
  ether1.begin();
  if(!ether1.linkIsUp())
    Serial.printf("Ethernet is disconnected");
  else
    Serial.println(ether1.getMyIP());

  in1.begin(); 
   
 
  sgtl.enable();
  sgtl.volume(1);
  sgtl.unmuteLineout();

  char s1[] = "Stream5dudule";
  in1.subscribe(s1);

  Serial.println("Done setup");
}

#define EVERY 1000
long count = 0;
long timer1 = -3000;

void loop() 
{
  if(millis() - timer1 > 10000)
  {

    Serial.printf("---------- Main: %i\n", millis()/1000);
    Serial.printf("LinkIs Up %i, IP ", ether1.linkIsUp());
    Serial.println(ether1.getMyIP());
    printActiveStreams(STREAM_IN);
    ether1.printHosts();
    printActiveSubs();
    timer1 = millis();
  }
  // regular Ethernet processing is tied to yield() and delay() 
  delay(100); // code delays do not disturb processing
}
