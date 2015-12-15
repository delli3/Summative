//Standard Engine header
//Contains all the nescessary headers for anything using the Engine

#pragma once

#define MOUSE_LB 1
#define MOUSE_RB 2
#define MOUSE_MB 3

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_opengl.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "Vector2D.h"
#include "Block.h"
#include "Buffer.h"
#include "IMath.h"
#include "Character.h"
#include "Button.h"
#include "Buffer.h"
#include "Effects.h"
#include "curl/curl.h"


class Engine{
public:
	const int FPS = 60;
	ALLEGRO_DISPLAY* GetDisplay();
	ALLEGRO_DISPLAY_MODE GetDisplayData();
	ALLEGRO_TIMER* GetTimer();
	ALLEGRO_FONT* GetDebugFont();
	void DrawFPS(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, double detla);
	
	void Init();
	void Cleanup();

	bool bExit;
private:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_DISPLAY_MODE disp_data;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT* debug_font;
};

extern Engine* GEngine = new Engine();

