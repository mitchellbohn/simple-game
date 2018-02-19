#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <sstream>

#include "SDL2\SDL.h"

#define DEBUG(s) //cerr << s << endl;

using namespace std;


class ProtoGame {
	protected:
	int maxX, maxY;
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool done;
	bool dead;
	public:
	int getMaxX() {return maxX;}
	int getMaxY() {return maxY;}
	SDL_Renderer* getRenderer() { return renderer; }
	void init(string WindowName="Game", int newMaxX=640, int newMaxY=480) {
		maxX=newMaxX;
		maxY=newMaxY;
		done=false;
		dead=false;
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow(
			WindowName.c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			maxX,
			maxY,
			SDL_WINDOW_OPENGL
		);
		if (window == NULL) {
		
			cout << "Could not create window:\n" << SDL_GetError() << endl;
			done = false;
			return;
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL) {
		
			cout << "Could not create renderer:\n" << SDL_GetError() << endl;
			done = false;
			return;
		}
		srand (time(NULL));
	}
	virtual void setup() = 0;
	virtual void loop(float dt) = 0;
	virtual void cleanup() = 0;
	virtual void eventHandler(SDL_Event e) = 0;
	void run() {
		setup();
		float last = SDL_GetTicks();
		bool dead = false;
		while (!dead && !done) {
			SDL_Event event;
			float dt=((float)SDL_GetTicks()-last)/1000.0;
			last = SDL_GetTicks();
			loop(dt);
			SDL_RenderPresent(renderer);
			if (SDL_PollEvent(&event)) {
				eventHandler(event);
			}
			SDL_Delay(1000/60);
		}
		if (!done) cleanup();
	}
	void exit() {
		if (done) return;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};

class AnimationFrame {
	SDL_Texture *texture;
	ProtoGame *g;
	int w, h, time;
	bool done;
	public:
	int getTime() { return time; }
	void render(ProtoGame *g, int x, int y) {
		SDL_Rect src, dest;
		src.x=(int)0; src.y=(int)0; src.w=w; src.h=h;
		dest.x=(int)x; dest.y=(int)y; dest.w=w; dest.h=h;
		SDL_RenderCopy(g->getRenderer(), texture, &src, &dest);
	}
    /*void init(SDL_Surface *newSurface, SDL_Rect src, float newTime=0.1) {
		time = newTime;
	
	}*/
	void setup(ProtoGame *newG, string filename, int newTime) {
		done = false;
		g = newG;
		time = newTime;
		SDL_Surface *image=SDL_LoadBMP(filename.c_str());
		if (image == NULL) {
			
			cout << "Could not create image:\n" << SDL_GetError() << endl;
			done = true;
			return;
		}
		SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 0, 0, 255));
		texture=SDL_CreateTextureFromSurface(g->getRenderer(), image);
		if (texture == NULL) {
			
			cout << "Could not create texture:\n" << SDL_GetError() << endl;
			done = true;
			return;
		}
		SDL_FreeSurface(image);
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	}
	void cleanup() {
		SDL_DestroyTexture(texture);
	}
};




class Sprite {
  protected:
	ProtoGame *g;
	string filename;
	int w, h, count;
	bool done;
	float aX, aY, vX, vY, pX, pY, T0;
	//SDL_Texture *texture;
	vector <AnimationFrame> frames;
	int totalAnimationTime, totalGameTime; //in ms
  public:
	void add(AnimationFrame af) {
		frames.push_back(af);
		totalAnimationTime+=af.getTime();
	}
	virtual void loop(float dt) {
		vX=vX+aX*dt;
		vY=vY+aY*dt;
		pX=pX+vX*dt;
		pY=pY+vY*dt;
		totalGameTime+=(int)(dt*1000.0);
	};
	virtual void cleanup() {
		for (unsigned i=0;i<frames.size();i++) {
			frames[i].cleanup();
		}
	};
	bool isDone() { return done; }
	void init(ProtoGame *newG, string newFilename, int newCount = 1, 
		float newAX=0.0,
		float newAY=0.0,
		float newVX=0.0,
		float newVY=0.0,
		float newPX=0.0,
		float newPY=0.0,
		int newT0=0) {
		g = newG;
		filename=newFilename;
		w=0;
		h=0;
		count = newCount;
		totalGameTime=newT0;
		totalAnimationTime=0;
		aX=newAX;
		aY=newAY;
		vX=newVX;
		vY=newVY;
		pX=newPX;
		pY=newPY;
	}
	virtual void setup(){
		for (int i = 0; i<count; i++) {
			AnimationFrame af;
			stringstream sstring;
			sstring << filename << i << ".bmp";
			af.setup(g, sstring.str().c_str(), 100);
			add(af);
		}
	}
	void render() {
		if (frames.size()==0) return;
		if (frames.size()==1) {
			frames[0].render(g, pX, pY);
			return;
		}
		
		int withinFrame;
		withinFrame=totalGameTime % totalAnimationTime;
		int talley = 0;
		for (int i=0; i<frames.size(); i++) {
			talley+=frames[i].getTime();
			if (withinFrame<talley) {
				frames[i].render(g, pX, pY);
				break;
			}
		}
	}
	void exit() {
	}
};



class Ball:public Sprite {
	public:
	void init(ProtoGame *newG,
	string newFilename,
	int newCount,
	float newAX=0.0,
	float newAY=0.0,
	float newVX=0.0,
	float newVY=0.0,
	float newPX=0.0,
	float newPY=0.0,
	int newT0=0) {
		g=newG;
		filename=newFilename;
		aX=newAX;
		aY=newAY;
		vX=newVX;
		vY=newVY;
		pX=newPX;
		pY=newPY;
		count=newCount;
		totalGameTime=newT0;
	}
	void loop(float dt) {
		Sprite::loop(dt);
		if (pX>g->getMaxX()) vX=-vX;
		if (pX<0) vX=-vX;
		if (pY>g->getMaxY()) vY=-vY;
		if (pY<0) vY=-vY;
	}
};




class Game:public ProtoGame {
	protected:
	vector <Sprite *> characters;
	public:
	void add(Sprite *c) { characters.push_back(c); }
	void loop(float dt) {
		for (unsigned int i=0; i<characters.size(); i++) {
			characters[i]->loop(dt);
		}
		for (unsigned int i=0; i<characters.size(); i++) {
			characters[i]->render();
		}
	}
};




class TheGame: public Game {
	vector<Ball *> balls;
	void addSomeBalls() {
		for (int i=0;i<10;i++) {
			Ball *b = new Ball();
			int px=rand() % maxX;
			int py=rand() % maxY;
			float vx=10.0*(float)(rand()%100)/100.0;
			float vy=10.0*(float)(rand()%100)/100.0;
			int t0=rand() % 1000;
			b->init(this, "P15_", 6, 5.0, 10.0, vx, vy, px, py, t0);
			DEBUG("After init a ball ");
			b->setup();
			DEBUG("Adding a Ball ");
			add(b);
			balls.push_back(b);
		}
	}
	void setup() {
		DEBUG("In Setup");
		Sprite *back = new Sprite();
		back->init(this, "Back", 1);
		back->setup();
		add(back);
		addSomeBalls();
	}
	void eventHandler(SDL_Event e) {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym==SDLK_RETURN) {
				addSomeBalls();
			}
			else if (e.key.keysym.sym==SDLK_ESCAPE) {
				done=true;
			}
		}
	}
	void cleanup() {
		for (unsigned int i=0; i<balls.size(); i++)
			delete balls[i];
	}
};

TheGame MyGame;

int main(int argc, char* argv[]) {
	MyGame.init("MyGame", 640, 480);
	DEBUG("After init ");
	MyGame.run();
	MyGame.exit();
	return 0;
}
