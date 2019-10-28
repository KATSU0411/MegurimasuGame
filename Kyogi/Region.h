#pragma once
#include "DxLib.h"

//======================================================================================
//	REGIONƒNƒ‰ƒX
//======================================================================================
class REGION
{
public:
	LONG    posx;
	LONG    posy;
	LONG    width;
	LONG    height;

public:
	REGION() {}
	REGION(LONG x, LONG y, LONG w, LONG h) {
		posx = x;
		posy = y;
		width = w;
		height = h;
	}

	REGION(RECT rect)
	{
		posx = rect.left;
		posy = rect.top;
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	~REGION() {}

	RECT Rect() {
		RECT rect = { posx, posy, posx + width, posy + height };
		return rect;
	};

	POINT Centor() {
		POINT pos = { posx + width/2, posy + height/2 };
		return pos;
	}


	// operator
	REGION operator+(POINT p) {
		return REGION(posx + p.x, posy + p.y, width, height);
	}

	REGION operator+=(POINT p) {
		posx += p.x;
		posy += p.y;
		return REGION(posx + p.x, posy + p.y, width, height);
	}

	REGION operator-(POINT p) {
		return REGION(posx - p.x, posy - p.y, width, height);
	}

	REGION operator-=(POINT p) {
		posx -= p.x;
		posy -= p.y;
		return REGION(posx + p.x, posy + p.y, width, height);
	}

	REGION operator*(int s){
		return REGION(posx, posy, width*s, height*s);
	}

	REGION operator/(int s){
		return REGION(posx, posy, width/s, height/s);
	}

	REGION operator=(REGION r) {
		posx = r.posx;
		posy = r.posy;
		width = r.width;
		height = r.height;
		return REGION(posx, posy, width, height);
	}

	REGION operator=(RECT rect) {
		posx = rect.left;
		posy = rect.top;
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		return REGION(posx, posy, width, height);
	}
};