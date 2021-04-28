#include <iostream>
#include <SDL.h>
#include <string>
#include "audio.h"
#include "graphics.h"


int main(int argc, char ** argv){
	std::string fileName = "";
	while (std::cin>>fileName) {
		SDL_Event event;
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) continue;
		Audio* audio = new Audio();
		if (!audio->init(fileName)) continue;

		Graphics* graphics = new Graphics();
		if (!graphics->init()) continue;

		audio->play();
		bool running = true;
		Uint32 startTime, endTime, delta;
		startTime = endTime = delta = 0;
		short timePerFrame = 15; //miliseconds

		while (running) {
			if (!startTime) startTime = SDL_GetTicks();
			else delta = endTime - startTime;
			if (delta < timePerFrame) SDL_Delay(timePerFrame - delta);
			graphics->update(audio->getData(), audio->getNumData());
			graphics->paint();
			if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
				break;
/*
		  int BARS = 100;

		  double* MAX = audio->data;
		  if (MAX != NULL) {
			  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
			  SDL_RenderClear(renderer);
			  int thick = WIDTH / BARS;
			  for (int i = 0; i < BARS; i++) {
				  SDL_Rect rect;
				  int max = MAX[i];//(MAX[i] > 2 ? log(MAX[i]) : MAX[i]) * 30;

				  rect.x = i*thick;
				  rect.y = int(HEIGHT - max);
				  rect.w = thick;
				  rect.h = max;
				  rect.x = i*thick;
				  rect.y = int(HEIGHT / 2 - max);
				  rect.w = thick;
				  rect.h = max;
				  Color c = rgb((double)i / BARS);
				  SDL_SetRenderDrawColor(renderer, c.red, c.green, c.blue, 255);
				  SDL_RenderFillRect(renderer, &rect);
			  }

			  SDL_RenderPresent(renderer);
		  }
		  while (SDL_PollEvent(&event)) {
			  if (event.type == SDL_QUIT)
				  running = false;
		  }*/

			startTime = endTime;
			endTime = SDL_GetTicks();
	  }

	  audio->close();
	  graphics->close();
	  SDL_Quit();
	}
	return 0;
}