#pragma once

#include "Engine.h"

//An enum representing the directions in which an animation can move
enum class EADirection{
	D_Back = -1,
	D_Forward = 1
};

class SpriteSheet{
public:
	void PushFrame();
	void PushFrame(EADirection Direction);
	ALLEGRO_BITMAP* GetFrameBitmap();

	SpriteSheet(){ Sheet = NULL; }

	SpriteSheet(ALLEGRO_BITMAP* Sheet, int width, int height, int frames);
protected:
	int FrameHeight;
	int FrameWidth;

	int FrameIndex;
	int FrameCount;
	ALLEGRO_BITMAP* Sheet;
};