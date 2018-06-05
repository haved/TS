#include <gtk/gtk.h>

#include "../PureArduino/ArduinoInterface.hpp"

#define LED_COUNT WIDTH*HEIGHT
CRGB colorFrom[LED_COUNT];
CRGB colorTo[LED_COUNT];
int transProg[4];
int transGoal[4];
int transGoingMask = 0;

void setTile(int screen, int x, int y, CRGB color) {

}
void fillRect(int screen, int x, int y, int width, int height, CRGB color) {

}
void fillScreen(int screen, CRGB color) {

}
void startTransition(int screen, int frames) {

}

GtkWidget* drawing_area;
void updateScreens() {
	gtk_widget_queue_draw(drawing_area);
}

bool anyTransitionRunning() {
	return transGoingMask != 0;
}
ButtonState<bool> currentButtonState;
void getButtonStates(ButtonState<bool>& state) {
	state = currentButtonState;
}

int getKeyIndex(int keyval) {
	switch(keyval) {
	case GDK_KEY_space: return BUTTON_A;
	case GDK_KEY_Up: return BUTTON_UP;
	case GDK_KEY_Down: return BUTTON_DOWN;
	case GDK_KEY_Left: return BUTTON_LEFT;
	case GDK_KEY_Right: return BUTTON_RIGHT;
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

#define TILE_SIZE 20
#define SCREEN_MARGIN 20
gboolean on_draw_event(GtkWidget* widget, cairo_t* cr, gpointer user_data) {
	static int i = 0;
	i+=10;
	cairo_set_source_rgb(cr, 0.6, sin(i), 0.6);
	cairo_set_line_width(cr, 1);

	cairo_rectangle(cr, 0, 0, 100, 100);
	cairo_stroke_preserve(cr);
	cairo_fill(cr);

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
	gtk_widget_set_size_request(drawing_area, TILE_SIZE*WIDTH, TILE_SIZE*HEIGHT*4+SCREEN_MARGIN*3);
	gtk_container_add(GTK_CONTAINER(window), drawing_area);

	g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw_event), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "PureArduinoSim");
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
