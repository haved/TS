#include <gtk/gtk.h>
#include <cstring>

#include "../PureArduino/ArduinoInterface.hpp"
#include "../Sound/SoundSystem.hpp"

//Screen 0 is P1_ATK
//Screen 1 is P1_DEF
//screen 2 is P2_ATK
//Screen 3 is P2_DEF

#define LED_COUNT WIDTH*HEIGHT
CRGB colorFrom[4][LED_COUNT];
CRGB colorTo[4][LED_COUNT];
CRGB leds[4][LED_COUNT];
CRGB *writeTo[4] = {leds[0], leds[1], leds[2], leds[3]};
int transProg[4];
int transGoal[4];
int transGoingMask = 0;

inline int max(int a, int b) { return a>b?a:b; }
inline int min(int a, int b) { return a<b?a:b; }

//All internal screen coords have 0 in top left corner seen from P1
//The GameLogic thinks 0,0 is to the left of player1 and right of player2
//0,0 is closest to the gap between ATK and DEF
int getInternalScreenCoord(int screen, int x, int y) {
	if(screen == PLAYER1+ATK || screen == PLAYER2+DEF)
		y = HEIGHT-1-y;
	return x+y*WIDTH;
}

void setTile(int screen, int x, int y, CRGB color) {
	writeTo[screen][getInternalScreenCoord(screen, x, y)] = color;
}
void fillRect(int screen, int x1, int y1, int width, int height, CRGB color) {
	for(int x = x1; x < x1+width; x++)
		for(int y = y1; y < y1+height; y++)
			writeTo[screen][getInternalScreenCoord(screen, x, y)] = color;
}

void fillScreen(int screen, CRGB color) {
	for(int i = 0; i < LED_COUNT; i++)
		writeTo[screen][i] = color;
}

void startTransition(int screen, int frames) {

	memcpy(colorFrom[screen], leds[screen], LED_COUNT*sizeof(CRGB));

	writeTo[screen] = colorTo[screen];

	if(frames <= 0)
		frames = 1; //Will instantly meet the goal upon next render
	transProg[screen] = 0;
	transGoal[screen] = frames;
	transGoingMask |= 1<<screen;
}

GtkWidget* drawing_area;
void updateScreens() {
	for(int screen = 0; screen < 4; screen++) {
		if(transGoingMask & 1<<screen) {
			transProg[screen]++;
			float trans = transProg[screen]/(float)transGoal[screen];
			for(int j = 0; j < LED_COUNT; j++)
				leds[screen][j] = interpolate(colorFrom[screen][j], colorTo[screen][j], trans);
			if(transProg[screen]>=transGoal[screen]) {
				writeTo[screen] = leds[screen];
				transGoingMask &= ~(1<<screen);
			}
		}
	}
	gtk_widget_queue_draw(drawing_area);
}

bool anyTransitionRunning() {
	return transGoingMask != 0;
}
ButtonState<bool> currentButtonState;
void getButtonStates(ButtonState<bool>& state) {
	state = currentButtonState;
}

CRGB getWrittenColor(int screen, int x, int y) {
	int coord = getInternalScreenCoord(screen, x, y);
	return writeTo[screen][coord];
}

CRGB getCurrentColor(int screen, int x, int y) {
	int coord = getInternalScreenCoord(screen, x, y);
	return leds[screen][coord];
}

int getKeyIndex(int keyval) {
	switch(keyval) {
	case GDK_KEY_Up: return BUTTON_UP;
	case GDK_KEY_Down: return BUTTON_DOWN;
	case GDK_KEY_Left: return BUTTON_LEFT;
	case GDK_KEY_Right: return BUTTON_RIGHT;
	case GDK_KEY_space: return BUTTON_A;
	case GDK_KEY_Return: return BUTTON_B;
	case GDK_KEY_BackSpace: return BUTTON_MENU;

	case GDK_KEY_w: return BTN_OFFSET_P2 + BUTTON_UP;
	case GDK_KEY_s: return BTN_OFFSET_P2 + BUTTON_DOWN;
	case GDK_KEY_a: return BTN_OFFSET_P2 + BUTTON_LEFT;
	case GDK_KEY_d: return BTN_OFFSET_P2 + BUTTON_RIGHT;
	case GDK_KEY_e: return BTN_OFFSET_P2 + BUTTON_A;
	case GDK_KEY_q: return BTN_OFFSET_P2 + BUTTON_B;
	case GDK_KEY_Escape: return BTN_OFFSET_P2 + BUTTON_MENU;
	default: return -1;
	}
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	int keyIndex = getKeyIndex(event->keyval);
	if(keyIndex < 0)
		return false; //We didn't handle the key event
	currentButtonState.raw[keyIndex] = true;
	return true;
}

gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	int keyIndex = getKeyIndex(event->keyval);
	if(keyIndex < 0)
		return false; //We didn't handle the key event
	currentButtonState.raw[keyIndex] = false;
	return true;
}

void loop(); //In GameLogic.cpp
gboolean main_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data) {
	loop();
	return true; //Keep going next frame
}

#define TILE_SIZE_DEF 16
#define TILE_SIZE_ATK 24
#define BIGGEST_TILE_SIZE max(TILE_SIZE_DEF, TILE_SIZE_ATK)
#define SCREEN_MARGIN 20

static const int screenIndexTable[] = {PLAYER2+DEF, PLAYER2+ATK, PLAYER1+ATK, PLAYER1+DEF};
static const int tileSizeTable[] = {TILE_SIZE_DEF, TILE_SIZE_ATK, TILE_SIZE_ATK, TILE_SIZE_DEF};

gboolean on_draw_event(GtkWidget* widget, cairo_t* cr, gpointer user_data) {
	int yOffset = 0;
	for(int i = 0; i < 4; i++) {
	    int screenIndex = screenIndexTable[i];
		int tileSize = tileSizeTable[i];
		int xOffset = (int)((BIGGEST_TILE_SIZE-tileSize)*WIDTH/2.f);
		for(int x = 0; x < WIDTH; x++) {
			for(int y = 0; y < HEIGHT; y++) {
				int coord = x+y*WIDTH;
				CRGB color = leds[screenIndex][coord];
				cairo_set_source_rgb(cr, color.r/255.f, color.g/255.f, color.b/255.f);
				cairo_rectangle(cr, xOffset + x*tileSize, yOffset + y*tileSize, tileSize-1, tileSize-1);
				cairo_stroke_preserve(cr);
				cairo_fill(cr);
			}
		}
		yOffset += SCREEN_MARGIN + tileSize * HEIGHT;
	}

	return false;
}

int main(int argc, char** argv) {
	gtk_init(&argc, &argv);
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 8);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
	gtk_widget_add_events(window, GDK_KEY_RELEASE_MASK);
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "key_release_event", G_CALLBACK (on_key_release), NULL);
	gtk_widget_add_tick_callback(window, main_tick, NULL, NULL);

    drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area, BIGGEST_TILE_SIZE*WIDTH, (TILE_SIZE_ATK+TILE_SIZE_DEF)*HEIGHT*2+SCREEN_MARGIN*3);
	gtk_container_add(GTK_CONTAINER(window), drawing_area);

	g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw_event), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "PureArduinoSim");
	gtk_widget_show_all(window);

	startAudioSystem("../Sound/");

	gtk_main();

	stopAudioSystem();
	return 0;
}
