//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST 2.x SDK Samples
// Filename     : vstxsynth.h
// Created by   : Steinberg Media Technologies
// Description  : Example VstXSynth
//
// A simple 2 oscillators test 'synth',
// Each oscillator has waveform, frequency, and volume
//
// *very* basic monophonic 'synth' example. you should not attempt to use this
// example 'algorithm' to start a serious virtual instrument; it is intended to demonstrate
// how VstEvents ('MIDI') are handled, but not how a virtual analog synth works.
// there are numerous much better examples on the web which show how to deal with
// bandlimited waveforms etc.
//
// � 2006, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __vstxsynth__
#define __vstxsynth__

#include "public.sdk/source/vst2.x/audioeffectx.h"

// Use this define to save the music to file.
//#define SAVE_MUSIC

// Size of a buffer with random numbers
#define RANDOM_BUFFER_SIZE 65536
#define DELAY_MULTIPLICATOR 128
#define MAX_DELAY_LENGTH (DELAY_MULTIPLICATOR * 130) // Some safety for miscalculation stuff...

// Number of additive overtones
#define NUM_OVERTONES 16
#define NUM_STEREO_VOICES 4

#ifndef PI
#define PI 3.1415926
#endif

//------------------------------------------------------------------------------------------
enum
{
	// Global
	kNumPrograms = 50,
	kNumOutputs = 2,

	kVolume = 0,
	kDuration,
	kDelayFeed, // Resonance at the start
	kDelayLength,

	kNumParams
};

//------------------------------------------------------------------------------------------
// VstXSynthProgram
//------------------------------------------------------------------------------------------
class VstXSynthProgram
{
friend class VstXSynth;
public:
	VstXSynthProgram ();
	~VstXSynthProgram () {}

private:
	float fVolume;
	float fDelayFeed;
	int iDelayLength;
	char name[kVstMaxProgNameLen+1];
};

//------------------------------------------------------------------------------------------
// VstXSynth
//------------------------------------------------------------------------------------------
class VstXSynth : public AudioEffectX
{
public:
	VstXSynth (audioMasterCallback audioMaster);
	~VstXSynth ();

	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* events);

	virtual void setProgram (VstInt32 program);
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);
	
	virtual void setSampleRate (float sampleRate);
	virtual void setBlockSize (VstInt32 blockSize);
	
	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
		
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	virtual VstInt32 canDo (char* text);

	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();

	virtual VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName);
	virtual VstInt32 getCurrentMidiProgram (VstInt32 channel, MidiProgramName* currentProgram);
	virtual VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category);
	virtual bool hasMidiProgramsChanged (VstInt32 channel);
	virtual bool getMidiKeyName (VstInt32 channel, MidiKeyName* keyName);

private:
	float moogFilter(float frequency, float resonance, float in);
	float b0, b1, b2, b3, b4; // moog filter parameters

	int iADSR; // 0: Attack, 2: Decay, 3: Release
	float fADSRVal; // Volume from ADSR envelope
	float fPhase[NUM_OVERTONES]; // Phase of the instrument

	float fScaler; // 2pi / sampleRate
	int sampleID; // ID of the processed sample

	// Random number generator
	float frand(void);
	unsigned long seed;
	float randomBuffer[RANDOM_BUFFER_SIZE];
	float lowNoise[RANDOM_BUFFER_SIZE];
	// The same as random Buffer, but contains exp(4*(randomBuffer-1))
	float expRandomBuffer[RANDOM_BUFFER_SIZE];
	
	// Reverberation
	float reverbBuffer[MAX_DELAY_LENGTH][NUM_STEREO_VOICES];
	int reverbBufferLength[NUM_STEREO_VOICES]; // Actual length taken for pull-out

	VstXSynthProgram* programs;
	VstInt32 channelPrograms[16];

	// The next program will be turned to current when death counter goes to 0.
	// Starting at 256, the death counter pulls down the volume.
	int deathCounter;
	VstInt32 currentNote;
	VstInt32 currentVelocity; // That is the midi volume
	VstInt32 nextNote;
	VstInt32 nextVelocity;

	void initProcess ();
	void noteOn (VstInt32 note, VstInt32 velocity);
	void noteOff ();
	void fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);

#ifdef SAVE_MUSIC
	static int firstNoteTime; // This one is static along buffers so that streams are synchronized
	int lastNote;
	int savedNoteID;
	int savedNoteTime[1000000];
	int savedNote[1000000];
	int savedVelocity[1000000];
	int savedInstrumentID;
	int savedInstrumentTime[1000000]; // Made relative to firstNoteTime
	int savedInstrumentParameter[kNumParams][1000000];
#endif
};

#endif
