#pragma once
#include <iostream>
#include <string>
#include <SDL_audio.h>
#include <fftw3.h>
#include <math.h>

#define NSAMPLES 4096
#define SAMPLE_RATE 48000

struct AudioData{
  Uint8* pos;
  Uint32 length;
  SDL_AudioFormat format;
  fftw_complex *in;
  fftw_complex *out;
  fftw_plan plan;
};

class Audio{
	private:
	AudioData audio;
	SDL_AudioSpec wavSpec;
	Uint8* wavStart; //Buffer of audio
	Uint32 wavLength;
	SDL_AudioDeviceID device;
	static void PlayAudioCallback(void* userdata, Uint8* stream, int len);
	void process(AudioData* audio, Uint8* stream);
	static double* data;
	static const int NUM_DATA = 30;
public:
	bool init(std::string fileName);
	void play();
	void pause();
	void close();
	double* getData();
	int getNumData();
};

double Get16bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format);
