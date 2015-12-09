#include "Engine.h"
#include "World.h"
#include "Buffer.h"

#define GRID_SIZE 32
const int FPS = 60;

void printk();

int main() {
	ALLEGRO_FONT *font = NULL;	//A font for debugging purposes
	ALLEGRO_DISPLAY *display;			//The display window
	ALLEGRO_EVENT_QUEUE *event_queue;	//The "event_queue"
	ALLEGRO_TIMER *timer;				//The loop timer
	Buffer dubBuff = { NULL, 0.f, 0.f, 5.f, 5.f, false, false };
	int wWidth = 640, wHeight = 480;	//Width and height of the window
	bool done = false;					//Whether the main loop is "done" (aka terminated)
	bool bOpenGL = true;		//Whether to use OpenGL
	World* CurrentWorld = new World(Vector2D(8192.f, 4092.f), GRID_SIZE);	//Creates the current world as well as a grid to store all the blocks
	Vector2D Clicked;	//The location of a click
	GridTile clickedTile;	//The clicked tile from the world grid
	bool bClicked = false;	//Whether a click was registered
	bool bRedraw = false;	//Whether to redraw the screen
	FILE *fptr;
	EBlockType SelectedBlock = EBlockType::B_Brick;		//the block type the user selects
	Block blocks[8192];	//Array of all block in the world
	BlockType Type[15];
	bool bDrawFPS = true, bDrawMouseLoc = false, bDrawClickID = false;

	//Mouse Drag
	bool bMouseDrag = false;
	Vector2D DragStart;
	float DragTime = 0.f;
	double fps, delta;
	Vector2D DragVelocity = Vector2D(-1.f, -1.f);

	//Load Allegro and all required modules
	if (!al_init()) {
		fprintf(stderr, "Allegro could not initialize\n");
		return -1;
	}
	else {
		printf("Loaded Allegro %s\n", ALLEGRO_VERSION_STR);
	}
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "Allegro Primitives Addon could not initialize\n");
		return -1;
	}
	else {
		printf("Loaded Allegro Primitives Addon %s\n", ALLEGRO_VERSION_STR);
	}
	if (!al_init_image_addon()) {
		fprintf(stderr, "Allegro Image Addon could not initialize\n");
		return -1;
	}
	else {
		printf("Loaded Allegro Image Addon %s\n", ALLEGRO_VERSION_STR);
	}
	if (!al_install_keyboard()) {
		fprintf(stderr, "Keyboard drivers could not be installed\n");
		return -1;
	}
	else{
		printf("Keyboard drivers installed\n");
	}
	if (!al_install_mouse()){
		fprintf(stderr, "Mouse drivers could not be installed\n");
		return -1;
	}
	else{
		printf("Mouse drivers installed\n");
	}

	//FOR DEBUG
	al_init_font_addon();
	//END LOADING OF ALL MODULES


	//Creates a builtin font that can be used on any computer without require ttf font files everywhere
	font = al_create_builtin_font();
	if (!font) {
		fprintf(stderr, "Could load font\n");
		return -1;
	}
	else {
		printf("Loaded font\n");
	}


	//create event loop stuff
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0f / FPS);	//Run the program at 60FPS
	dubBuff.image = al_create_bitmap(4096, 2048);


	//Set ALLEGRO_DISPLAY flags
	if (bOpenGL){
		al_set_new_display_flags(ALLEGRO_OPENGL);
	}

	//Create the main display window
	display = al_create_display(wWidth, wHeight);

	//Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());


	Type[0] = BlockType("Rainbow", al_load_bitmap("Textures/Rainbow.png"));
	Type[1] = BlockType("Brick", al_load_bitmap("Textures/Brick.png"));
	Type[2] = BlockType("Grass", al_load_bitmap("Textures/Grass.png"));
	Type[3] = BlockType("Dirt", al_load_bitmap("Textures/Dirt.png"));
	Type[4] = BlockType("Stone", al_load_bitmap("Textures/Stone.png"));
	Type[5] = BlockType("Fancy", al_load_bitmap("Textures/Fancy.png"));

	//Clear screen to black
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	//Sets the draw target to the grid bitmap
	al_set_target_bitmap(dubBuff.image);
	int sum = 0;

	//Draws a grid based on the tiles of the world
	/*for (int i = 0; i < 128; i++){
		for (int j = 0; j < 64; j++){
			al_draw_line(i * CurrentWorld->gridSize, (j + 1) * CurrentWorld->gridSize, (i + 1) * CurrentWorld->gridSize, (j + 1) * CurrentWorld->gridSize, al_map_rgb(0, 255, 0), 1);
			al_draw_line((i + 1) * CurrentWorld->gridSize, j * CurrentWorld->gridSize, (i + 1) * CurrentWorld->gridSize, (j + 1) * CurrentWorld->gridSize, al_map_rgb(0, 255, 0), 1);
			al_draw_textf(font, al_map_rgb(0, 255, 0), i * CurrentWorld->gridSize, j * CurrentWorld->gridSize, 0, "%d", sum);
			sum++;
		}
	}*/

	for (int i = 0; i < 128; i++){
		al_draw_line(i * GRID_SIZE, 0, i * GRID_SIZE, 2048, al_map_rgb(50, 50, 50), 1);
	}
	for (int i = 0; i < 64; i++){
		al_draw_line(0, i * GRID_SIZE, 4096, i * GRID_SIZE, al_map_rgb(50, 50, 50), 1);
	}
	//Sets the target bitmap back to the default buffer
	al_set_target_bitmap(al_get_backbuffer(display));

	//Draws everything to the screen
	al_flip_display();

	printf("Read saved level? (y/n): ");
	char cRead;
	scanf("%c", &cRead);
	if (tolower(cRead) == 'y'){
		fflush(stdin);
		char loadLevel[64];
		printf("Enter level name: ");
		scanf("%s", loadLevel);
		fflush(stdin);
		strcat(loadLevel, ".bvl");
		fptr = fopen(loadLevel, "rb");

		for (int i = 0; i < 8192; i++){
			fseek(fptr, sizeof(Block)*i, SEEK_SET);
			fread(&blocks[i], sizeof(Block), 1, fptr);
		}
		fclose(fptr);
		fptr = NULL;
	}

	//Starts the timer which runs the following while loop at a certain rate (60FPS)	
	al_start_timer(timer);

	//Gets a starting time in order to calculate a delta time
	double old_time = al_get_time();

	//Main tick loop
	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		//Gets current state of the mouse
		ALLEGRO_MOUSE_STATE state;
		al_get_mouse_state(&state);

		//End the loop if the window's close button is clicked
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		//On KeyDown
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
				//Close window if escape key is pressed

				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_D:
				case ALLEGRO_KEY_RIGHT:
					dubBuff.bdx = true;
					dubBuff.dx = -5;
					break;
				case ALLEGRO_KEY_A:
				case ALLEGRO_KEY_LEFT:
					dubBuff.bdx = true;
					dubBuff.dx = 5;
					break;
				case ALLEGRO_KEY_S:
				case ALLEGRO_KEY_DOWN:
					dubBuff.bdy = true;
					dubBuff.dy = -5;
					break;
				case ALLEGRO_KEY_W:
				case ALLEGRO_KEY_UP:
					dubBuff.bdy = true;
					dubBuff.dy = 5;
					break;
				case ALLEGRO_KEY_I:
					bDrawFPS = !bDrawFPS;
					break;
				case ALLEGRO_KEY_O:
					bDrawClickID = !bDrawClickID;
					break;
				case ALLEGRO_KEY_P:
					bDrawMouseLoc = !bDrawMouseLoc;
					break;
				case ALLEGRO_KEY_1:
					SelectedBlock = EBlockType::B_Rainbow;
					break;
				case ALLEGRO_KEY_2:
					SelectedBlock = EBlockType::B_Brick;
					break;
				case ALLEGRO_KEY_3:
					SelectedBlock = EBlockType::B_Grass;
					break;
				case ALLEGRO_KEY_4:
					SelectedBlock = EBlockType::B_Dirt;
					break;
				case ALLEGRO_KEY_5:
					SelectedBlock = EBlockType::B_Stone;
					break;
				case ALLEGRO_KEY_6:
					SelectedBlock = EBlockType::B_Fancy;
					break;
				default:
					break;
			}
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_LEFT:
			case ALLEGRO_KEY_RIGHT:
				dubBuff.bdx = false;
				break;
			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
			case ALLEGRO_KEY_DOWN:
				dubBuff.bdy = false;
				break;
			default:
				break;
			}
		}

		//On mouse click
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			switch (ev.mouse.button){
			case MOUSE_LB: 
				bClicked = true;

				//Get the mouse's location
				Clicked = Vector2D(state.x + (dubBuff.x * -1), state.y + (dubBuff.y * -1));

				//Get the tile that was clicked
				clickedTile = CurrentWorld->getClickedTile(Clicked);
				//if the tile is not already occupied by a block, create a new block
				if (!clickedTile.occupied){
					blocks[clickedTile.id] = Block(clickedTile.location, SelectedBlock);
					blocks[clickedTile.id].bSpawned = true;
					clickedTile.occupied = true;
				}
				break;
			case MOUSE_RB: 
				bMouseDrag = true;
				DragStart = Vector2D(ev.mouse.x, ev.mouse.y);
				break;
			case MOUSE_MB:	
				printf("mmb pressed\n");
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			bClicked = false;

			if (ev.mouse.button == MOUSE_RB){
				bMouseDrag = false;
				DragVelocity = DragStart / DragTime;
			}
		}

		//Tick
		if (ev.type == ALLEGRO_EVENT_TIMER){
			bRedraw = true;
			if (dubBuff.bdx) {
				dubBuff.x += dubBuff.dx;
				CurrentWorld->offset.x += dubBuff.dx;
			}
			if (dubBuff.bdy) {
				dubBuff.y += dubBuff.dy;
				CurrentWorld->offset.y += dubBuff.dy;
			}
			if (bMouseDrag){
				dubBuff.y -= DragStart.y - state.y;
				dubBuff.x -= DragStart.x - state.x;
				CurrentWorld->offset -= DragStart - Vector2D(state.x, state.y);
				DragStart = Vector2D(state.x, state.y);
				DragTime += delta;
			}

			//Inertia!!
			/*if (DragVelocity > Vector2D(0, 0) && !bMouseDrag){
				DragVelocity -= Vector2D(0.05f, 0.05f);
				dubBuff.y -= DragStart.y - state.y;
				dubBuff.x -= DragStart.x - state.x;
				CurrentWorld->offset -= DragStart - Vector2D(state.x, state.y);
			}*/

			CurrentWorld->Tick();
		}


		//Redraw the screen 
		//DO NOT PUT TICK CODE HERE!!!
		if (bRedraw && al_event_queue_is_empty(event_queue)){
			//Draws the framerate of the program on the screen
			double new_time = al_get_time();
			delta = new_time - old_time;
			fps = 1 / (delta);
			old_time = new_time;
			ALLEGRO_COLOR tColor;

			if (fps > 30) {
				tColor = al_map_rgb(0, 255, 0);
			}
			else {
				tColor = al_map_rgb(255, 0, 0);
			}

			al_set_target_bitmap(dubBuff.image);
			//Foreach loop that goes through every block

			for (auto& elem : blocks){
				//If the block has been created, draw it!
				if (elem.bSpawned){
					al_draw_bitmap(Type[static_cast<int>(elem.type)].texture, elem.position.x, elem.position.y, ALLEGRO_VIDEO_BITMAP);
				}
			}

			al_set_target_bitmap(al_get_backbuffer(display));

			al_draw_bitmap_region(dubBuff.image, dubBuff.x * -1, dubBuff.y * -1, wWidth, wHeight, 0, 0, 0);

			//al_draw_bitmap(dubBuff.image, dubBuff.x, dubBuff.y, 0);

			//Draw mouse position
			if (bDrawMouseLoc){
				al_draw_textf(font, al_map_rgb(255, 255, 255), al_get_display_width(display) - 75, 112, 0, "x : %d", state.x);
				al_draw_textf(font, al_map_rgb(255, 255, 255), al_get_display_width(display) - 75, 128, 0, "y : %d", state.y);
			}

			if (bClicked && bDrawClickID) {
				al_draw_textf(font, al_map_rgb(255, 255, 255), al_get_display_width(display) - 75, 96, 0, "%d", clickedTile.id);
			}

			//Draw FPS
			if (bDrawFPS){
				al_draw_textf(font, al_map_rgb(0, 0, 0), al_get_display_width(display) - 74, 17, 0, "%.2f FPS", fps);
				al_draw_textf(font, al_map_rgb(0, 0, 0), al_get_display_width(display) - 74, 33, 0, "%.2fMS", delta * 1000);
				al_draw_textf(font, tColor, al_get_display_width(display) - 75, 16, 0, "%.2f FPS", fps);
				al_draw_textf(font, tColor, al_get_display_width(display) - 75, 32, 0, "%.2fMS", delta * 1000);
			}
			//Flips the buffer to the screen
			al_flip_display();

			//Clears the screen so that no strange overwriting occurs
			al_clear_to_color(al_map_rgb(0, 0, 0));
			bRedraw = false;
		}
	}
	fflush(stdin);

	printf("Save level? (y/n): ");
	char cSave;
	scanf("%c", &cSave);
	fflush(stdin);
	if (tolower(cSave) == 'y'){
		char levelName[64];
		printf("Enter a file name: ");
		scanf("%s", levelName);
		strcat(levelName, ".bvl");

		fptr = fopen(levelName, "wb+");

		for (auto& b : blocks){
			fwrite(&b, sizeof(Block), 1, fptr);
		}

		fclose(fptr);
	}

	//Destroy everything after the loop is exited
	al_destroy_bitmap(dubBuff.image);
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_shutdown_primitives_addon();

	delete CurrentWorld;

	return 0;
}

void printk(){
	printf("k\n");
}