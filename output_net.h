/* Multi-channel Network Audio output for Teensy Audio Library 
 * does NOT take update_responsibility
 * Richard Palmer - 2024
 * Released under GNU Affero General Public License v3.0 or later
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef output2_Net_h_
#define output2_Net_h_

#define AUDIO_OUTPUT_CHANNELS 2

#include "Arduino.h"
#include "AudioStream_F32.h"
#include "Audio.h"
#include "audio_net.h"
#include "control_ethernet.h"

//#define ON_DEBUG

/*
 * This file just handles high-level block transfers and logical audio and control functions 
 * Audio data can be broadcast from a master to a number of slaves using an IP broadcast address, 
 * or simply transferred between two consenting hosts
 */

#define DEFAULT_CHANNELS 2

class AudioOutputNet : public AudioStream_F32
{
public:
	AudioOutputNet(uint8_t outCh = DEFAULT_CHANNELS) : AudioStream_F32(outCh, inputQueueArray) 
	{
		_outChans = outCh;
		}
	friend class AudioControlEthernet; // may not be required
	
	void begin(void);	
	void update(void);

	int subscribe(char *sName, IPAddress remoteIP = IPAddress((uint32_t)0)); // default to broadcast IP
	// int subscribe(char *streamName, char *hostName) is not yet implemented
	int missedTransmit(bool reset = true);	// get (and reset) the number of missed transmit buffer on update

protected:
	bool queueBlocks(void);
	audio_block_f32_t *inputQueueArray[MAXCHANNELS];
	audio_block_f32_t *block[MAXCHANNELS];	
	std::queue <queuePkt> _myQueueO;
	int _myStreamO = EOQ; // valid streamID is 0..255

private:
	bool outputBegun = false;
	char _myStreamName[VBAN_STREAM_NAME_LENGTH] = "*";
	uint32_t didNotTransmit = 0;
	uint32_t _nextFrame = 0; 
	uint8_t _outChans;

	// debug 
	bool printMe;
	void printHdr(vban_header *hdr);
	void printSamples(int16_t *buff, int len, int chans);
};
#endif
