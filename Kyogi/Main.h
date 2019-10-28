#pragma once

#include "Region.h"

// ウィンドウサイズ
#define WINDOW_HEIGHT 720
#define	WINDOW_WIDTH 1280
#define STR_MAX 255			// 最大文字数
#define PORT 57810			// 接続ポート番号

// カラー群
const unsigned int WHITE = GetColor(255,255,255);
const unsigned int BLACK = GetColor(0,0,0);
const unsigned int CYAN = GetColor(0,255,255);
const unsigned int DEEPSKYBLUE = GetColor(0,0xbf,255);
const unsigned int GOLD = GetColor(0xff,0xd7,0);
const unsigned int DARKORANGE = GetColor(0xff,0x8c,0);
const unsigned int GRAY = GetColor(0x80, 0x80, 0x80);

typedef enum SYSMODE {
	SYS_START,		// 開始前
	SYS_INPUTIP,	// IP入力
	SYS_CONNECTING, // 接続待ち
	SYS_OPERATE,	// 意思表示フェーズ
	SYS_MOVE,		// 行動フェーズ
	SYS_END,
}SYSMODE;


int DrawBox(REGION region, unsigned int Color, int FillFlag);
int DrawStringCentor(POINT centor, const char *String, UINT Color);
void ChangeSysMode(SYSMODE NewSysMode);
int StringSplit(char *dst[], char *src, const char delim);
int GetWeightRandom(int WeightArr[]);