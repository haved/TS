#pragma once
#include "ArduinoInterface.hpp"

struct Boat {
	int width;
	int height;
	int x;
	int y;
	CRGB color;

	Boat() {}
	Boat(int width, int height, CRGB color) : width(width), height(height), x((WIDTH-width)/2), y((HEIGHT-height)/2), color(color) {}

	inline bool inBounds() {
		return x>=0 && y>=0 && x+width<=WIDTH && y+height<=HEIGHT;
	}
	inline void rotate() {
		int oldWidth = width, oldHeight=height;
		x+=(width-1)/2;
		y+=height/2;
		width = oldHeight;
		height = oldWidth;
		x-=(width-1)/2;
		y-=height/2;
		x = min(max(x, 0), WIDTH-width);
		y = min(max(y, 0), HEIGHT-height);
	}
	inline void render(int screen, CRGB color) {
		for(int xx = x; xx < x+width; xx++)
			for(int yy = y; yy < y+height; yy++)
				setTile(screen, xx, yy, color);
	}
	inline void render(int screen) {
	    render(screen, color);
	}
	inline bool equals(const Boat& other) {
		return x==other.x && y==other.y && width==other.width && height==other.height;
	}
	inline bool overrides(const Boat& other) {
		return x+width > other.x && y+height>other.y && x<other.x+other.width && y<other.y+other.height;
	}
};
