#include <iostream>

#include "SDL2\SDL.h"

using namespace std;

class Sprite {
	protected:
	char *filename;
	int w,h;
	bool done;
	public:
	virtual void setup()=0;
	virtual void loop(float dt)=0;
	bool isdone();
	virtual void cleanup()=0;
	void init(const char newFilename) {
		filename=newFilename;
		w=0;
		h=0;
	}
	void exit() {
	}
};

class Ball:public Sprite {
	void setup(){
		SDL_Surface *image=SDL_LoadBMP("Test.bmp");
		if (image == NULL) {
			cout << "Could not create image:\n" << SDL_GetError() << endl;
			return 1;
		}
		SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer, image);
		if (texture == NULL) {
			cout << "Could not create texture:\n" << SDL_GetError() << endl;
			return 1;
		}
		SDL_FreeSurface(image);
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		SDL_Rect src={0,0,w,h};
	}

}

class Game {
	int maxX, maxY;
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool done;
	bool dead;
	void init(const char *WindowName="Game", int newMaxX=640, int newMaxY=480) {
		
		SDL_Window *window;
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow(
			"An SDL2 window",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640,
			480,
			SDL_WINDOW_OPENGL
		);
		if (window == NULL) {
			cout << "Could not create window:\n" << SDL_GetError() << endl;
			done = false;
			return;
		}
		SDL_Renderer *renderer;
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL) {
			cout << "Could not create renderer:\n" << SDL_GetError() << endl;
			done = false;
			return;
		}
		void exit() {
			if (done) return;
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
}
	
}

class TheGame: public Game {
	void setup() {
	}
	void loop(float dt) {
	}
};

Game MyGame;

int main(int argc, char* argv[]) {
	MyGame.init();
	
	SDL_Surface *image=SDL_LoadBMP("Test.bmp");
	if (image == NULL) {
		cout << "Could not create image:\n" << SDL_GetError() << endl;
		done = true;
		return;
	}
	SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer, image);
	if (texture == NULL) {
		cout << "Could not create texture:\n" << SDL_GetError() << endl;
		done = true;
		return;
	}
	SDL_FreeSurface(image);
	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_Rect src={0,0,w,h};
	SDL_Rect dest;
	float aX;
	float vX;
	float aY;
	float vY;
	float pY;
	float pY;
	long last = SDLGetTicks();
	bool dead = false;
	while (!dead) {
		float dt=((float)SDL_GetTicks()-last)/1000.0;
		last = SDLGetTicks();
		vX=vX+aX*dt;
		pX=pX+vX*dt;
		vY=vY+aY*dt;
		pY=pY+vY*dt;
		if (pX>640) vX=-vX;
		if (pX<0) vX=-vX;
		if (pX>480) vY=-vY;
		if (pX<0) vY=-vY;
		dest.x=(int)pX; dest.y=(int)pY; dest.w=w; dest.h=h;
		SDL_RenderCopy(renderer, texture, &src, &dest);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/60);
	}
	//SDL_Delay(3000);
	void exit() {
		if (done) return;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	return 0;
}
