// Multi Stream Audio test for Teensy Ethernet Audio Library
// Requires:  two Teensy 4.1s with Ethernet adaptors 
//            or one Teensy 4.1 and a PC with Voicemeeter on the same network
//            An Audio Board on one Teensy is helpful to view signals, but not mandatory.

/* If using Voicemeeter: Loop back the signal from the Teensy and display its peak level and output to I2S 
  Go to the VBAN screen
    subscribe to the incoming Stream1 from Teensy1.local 
    output Stream1 to your network broadcast address (X.X.X.255)
    turn VBAN on
  On the main Voicemeeter screen
    Input the incoming stream and output it again on BUS A - to create a loopback to the Teensy
  After a few seconds the input signal should appear on both the main Voicemeeter and VBAN screens.
  The sine wave looped back from Voicemeeter should appear on the I2S output and as a peak reaging on the serial monitor.

  Two Teensys:
    Just connect both to the network.
    In1.peak should read 0.900 and the signal should appear on one channel of the I2S.
    in2.peak will read 1.000 because no Audio buffers are being provided. 
*/

#define HAVE_AUDIO_BOARD // uncomment if Teensy Audio Board is mounted (won't fail if left defined without an audio board)
#include <Audio.h>

#ifdef HAVE_AUDIO_BOARD
  #include <Wire.h>
  #include <SPI.h>
  #include <SD.h>
  #include <SerialFlash.h>
#endif

#include "control_ethernet.h"
#include "input_net.h"
#include "output_net.h"
#include "OpenAudio_ArduinoLibrary.h"
#include <string>

// Configuration SD card
//#define SD_CS_PIN 10 // Pin CS pour la carte SD, à modifier selon votre configuration
#define SD_CS_PIN    BUILTIN_SDCARD
#define SD_MOSI_PIN  11
#define SD_SCK_PIN   13

const float sample_rate_Hz = 48000.0f;
const int   audio_block_samples = 128;  // Always 128, which is AUDIO_BLOCK_SAMPLES from AudioStream.h
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

AudioSDPlayer_F32  audioSDPlayer(audio_settings);
AudioOutputI2S_F32 audioOutput(audio_settings);

AudioControlEthernet   ether1;
AudioOutputNet         out1(1);
AudioSynthSineCosine_F32    sineCos;

//create audio connections
AudioConnection_F32      patchCord1(audioSDPlayer, 0, audioOutput, 0);
AudioConnection_F32      patchCord2(audioSDPlayer, 1, audioOutput, 1);
AudioConnection_F32      patchCord3(audioSDPlayer, 0, out1, 0);




#include "utils.h" // diagnostic print functions - assumes "AudioControlEthernet  ether1;""
void setup() 
{
  //AudioMemory(10); // more than required

  Serial.begin(115200);
  while (!Serial && millis() < 5000) 
  {
    delay(100);
  }
  Serial.println("\n\nStarting Multi Audio Stream Test");

  // Initialisation SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Erreur : Carte SD introuvable !");
    while (1);
  }
  Serial.println("Carte SD initialisée.");

    // Initialisation de la bibliothèque audio
  //AudioMemory(50); // Allouer suffisamment de mémoire pour les buffers audio
  AudioMemory_F32(30, audio_settings);
    float fr = 600.0f; //  Sine wave frequency and correction for sample rate
  sineCos.amplitude(0.2); sineCos.frequency(fr*44117.647f/sample_rate_Hz);

  
  char myHost[] = "Teensy1";
  ether1.setHostName(myHost);
  ether1.begin();
  if(!ether1.linkIsUp())
    Serial.printf("Ethernet is disconnected");
  else
    Serial.println(ether1.getMyIP());
  //out1.subscribe("Stream1dudule");
  std::string streamName = "Stream1dudule";
  out1.subscribe(streamName.data());  // Ou .c_str() si acceptant const char*
  out1.begin();

  

  //prepare SD player
  audioSDPlayer.begin();
  Serial.println("Done setup");
}

#define EVERY 1000
long count = 0;
long timer1 = -3000;


void loop() 
{
  
 if (!audioSDPlayer.isPlaying())
     { //wait until previous play is done
     //start playing audio
     Serial.println("Starting audio player: SDTEST1.WAV");
     audioSDPlayer.play("SDTEST1.WAV");
     delay(5000);
     }
 
  delay(500);

  
  
  if(millis() - timer1 > 10000)
  {

    Serial.printf("LinkIs Up %i, IP ", ether1.linkIsUp());
    Serial.println(ether1.getMyIP());                         
    printActiveStreams(STREAM_OUT);
    ether1.printHosts();
    printActiveSubs();
    timer1 = millis();
  
  }
// regular Ethernet processing is tied to yield() and delay() 
  delay(100); // code delays do not disturb processing

}
