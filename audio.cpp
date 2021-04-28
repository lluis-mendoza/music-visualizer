#include "audio.h"

bool Audio::init(std::string fileName) {
	fileName += ".wav";
	std::string aux = "./songs/" + fileName;
	const char * f = aux.c_str();

	if (SDL_LoadWAV(f, &wavSpec, &wavStart, &wavLength) == NULL) {
		 std::cerr << "Error loading the file" << std::endl;
		 return false;
	}

	audio.in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*NSAMPLES);
	audio.out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*NSAMPLES);
	audio.plan = fftw_plan_dft_1d(NSAMPLES, audio.in, audio.out, FFTW_FORWARD, FFTW_MEASURE);

	audio.pos = wavStart;
	audio.length = wavLength;
	wavSpec.freq = SAMPLE_RATE;
	wavSpec.channels = 2;
	wavSpec.samples = NSAMPLES;
	wavSpec.callback = PlayAudioCallback;
	wavSpec.userdata = &audio;
	wavSpec.format = AUDIO_S16;
	audio.format = wavSpec.format;

	device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (device == 0) {
		 std::cerr << "Error: " << SDL_GetError() << std::endl;
		 return false;
	}
	data = NULL;
	return true;
}
void Audio::play()
{
	  SDL_PauseAudioDevice(device, 0);
}
void Audio::pause()
{
	  SDL_PauseAudioDevice(device, 1);
}
void Audio::close() {
	  fftw_destroy_plan(audio.plan);
	  fftw_cleanup();
	  fftw_free(audio.in);
	  fftw_free(audio.out);

	  SDL_CloseAudioDevice(device);
	  SDL_FreeWAV(wavStart);
}
//The function to call when the audio device needs more data
void Audio::PlayAudioCallback(void* userdata, Uint8* stream, int len){

  AudioData* audio = (AudioData*)userdata;
  if (audio->length == 0){
    return;
  }

  Uint32 length = (Uint32)len;
  length = (length > audio->length ? audio->length : length);

  SDL_memcpy(stream, audio->pos, len);

  audio->pos += length;
  audio->length -= length;
  Audio* aux = new Audio();
  aux->process(audio, stream);
}


double Get16bitAudioSample(Uint8* bytebuffer,SDL_AudioFormat format)
{
    Uint16 val= 0x0;

    if(SDL_AUDIO_ISLITTLEENDIAN(format))
        val=(uint16_t)bytebuffer[0] | ((uint16_t)bytebuffer[1] << 8);
    else
        val=((uint16_t)bytebuffer[0] << 8) | (uint16_t)bytebuffer[1];

    if(SDL_AUDIO_ISSIGNED(format))
        return ((int16_t)val)/32768.0;
    
    return val/65535.0;
}
void Audio::process( AudioData* audio, Uint8* stream){

  double MAX[NUM_DATA];
  double FREQ[NUM_DATA];
  for(int i = 0; i<NUM_DATA; ++i){
    MAX[i] = 1.7E-308;
    FREQ[i] = i*(SAMPLE_RATE/NSAMPLES)+i;
  }
  for(int i=0;i<NSAMPLES;i++)
  {
    //Hann function
    double multiplier= 0.5*(1-cos(2*M_PI*i/NSAMPLES));
        
    audio->in[i][0]=Get16bitAudioSample(stream, audio->format)*multiplier;
    audio->in[i][1]=0.0;

    stream += 2;
  }  
  fftw_execute(audio->plan);
  for(int i = 0; i<NSAMPLES; i++){
    double re = audio->out[i][0];
    double im = audio->out[i][1];
    double magnitude=sqrt((re*re)+(im*im));
    
    double freq = i*((double)SAMPLE_RATE/NSAMPLES);
    for(int i=0;i < NUM_DATA;i++)
      if(freq>FREQ[i] && freq<=FREQ[i+1])
        if(magnitude > MAX[i])
          MAX[i]=magnitude;
  }
  data = new double[NUM_DATA];
  for (int i = 0; i < NUM_DATA; ++i) {
	  data[i] = MAX[i];
  }
}
double* Audio::getData() {
	return data;
}
int Audio::getNumData()
{
	return NUM_DATA;
}
double* Audio::data = {};

