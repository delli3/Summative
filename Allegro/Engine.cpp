#include "Engine.h"

ALLEGRO_DISPLAY* Engine::GetDisplay(){
	return display;
}

ALLEGRO_TIMER* Engine::GetTimer(){
	return timer;
}

ALLEGRO_DISPLAY_MODE Engine::GetDisplayData(){
	return disp_data;
}

ALLEGRO_FONT* Engine::GetDebugFont(){
	return debug_font;
}

ALLEGRO_EVENT_QUEUE* Engine::GetEventQueue(){
	return event_queue;
}

void Engine::DrawFPS(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, double delta){
	ALLEGRO_COLOR tColor;
	if (1 / delta >= 30){
		 tColor = al_map_rgb(0, 255, 0);
	}
	else{
		tColor = al_map_rgb(255, 0 , 0);
	}
	al_draw_textf(font, al_map_rgb(0, 0, 0), al_get_display_width(display) - 74, 17, 0, "%.2f FPS", 1 / delta);
	al_draw_textf(font, al_map_rgb(0, 0, 0), al_get_display_width(display) - 74, 33, 0, "%.2fMS", delta * 1000);
	al_draw_textf(font, tColor, al_get_display_width(display) - 75, 16, 0, "%.2f FPS", 1 / delta);
	al_draw_textf(font, tColor, al_get_display_width(display) - 75, 32, 0, "%.2fMS", delta * 1000);
}

void Engine::Init(){
	//Allegro
	if (!al_init()) {
		fprintf(stderr, "Allegro could not initialize\n");
		bExit = true;
	}
	else {
		printf("Loaded Allegro %s\n", ALLEGRO_VERSION_STR);
	}

	//Primitives
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "Allegro Primitives Addon could not initialize\n");
		bExit = true;
	}
	else {
		printf("Loaded Allegro Primitives Addon %s\n", ALLEGRO_VERSION_STR);
	}

	//Image
	if (!al_init_image_addon()) {
		fprintf(stderr, "Allegro Image Addon could not initialize\n");
		bExit = true;
	}
	else {
		printf("Loaded Allegro Image Addon %s\n", ALLEGRO_VERSION_STR);
	}

	//Keyboard Drivers
	if (!al_install_keyboard()) {
		fprintf(stderr, "Keyboard drivers could not be installed\n");
		bExit = true;
	}
	else{
		printf("Keyboard drivers installed\n");
	}

	//Mouse Drivers
	if (!al_install_mouse()){
		fprintf(stderr, "Mouse drivers could not be installed\n");
		bExit = true;
	}
	else{
		printf("Mouse drivers installed\n");
	}

	//Font addon
	if (!al_init_font_addon() || !al_init_ttf_addon()){
		fprintf(stderr, "Font addon could not be loaded\n");
		bExit = true;
	}
	else{
		printf("Font addon loaded");
	}

	debug_font = al_create_builtin_font();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0f / FPS);	//Run the game at 60FPS

	al_set_new_display_flags(ALLEGRO_OPENGL);

	display = al_create_display(1280, 720);

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	States[0] = new PlayState();
}

void Engine::Cleanup(){
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_shutdown_primitives_addon();
}

bool Engine::ShouldTick(){
	return !bExit;
}
bool Engine::ShouldRedraw(){
	return bRedraw;
}

void Engine::Quit(){
	bExit = true;
}

void Engine::HandleInput(ALLEGRO_EVENT* ev){
	switch (ev->keyboard.keycode) {
		//Close window if escape key is pressed
	case ALLEGRO_KEY_ESCAPE:
		GEngine->Quit();
		break;
	}

	States[0]->HandleEvents(ev);
}

void Engine::Tick(){
	States[0]->Tick();
	bRedraw = true;
}

void Engine::Draw(){
	States[0]->Draw();
	bRedraw = false;
}