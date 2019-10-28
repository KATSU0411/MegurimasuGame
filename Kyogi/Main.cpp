#include "DxLib.h"
#include "Field.h"
#include "Main.h"
#include "StepTimer.h"
#include "Region.h"
#include "Menu.h"
#include "CAnimation.h"
#include <stdio.h>
#include <string>

// 定数群
#define BOXSIZE 55			// セルサイズ
#define BUTTONSIZE 70		// ボタンサイズ

#define AGENTPEOPLE 2		// エージェント人数

// ネームスペース
using namespace std;

// 関数群
unsigned int GetStateColor(int State);
POINT CursorPos2FieldPos(POINT CursorPos);
void Update(const DX::StepTimer& Timer);
void Render(const DX::StepTimer& Timer);
void ButtonClick();
int GetAgentNo(char *str);


// グローバル変数群
Field g_Field;
DX::StepTimer g_Timer;
SYSMODE g_SysMode = SYS_START;
SYSMODE g_NewSysMode = SYS_START;
Menu *g_MenuTop = nullptr;
Menu **g_MenuPlaying = nullptr;
int g_NetHandle = 0;
PROCON::CAnimation g_TimeAnim;


// 定数宣言
const int YFIELD = g_Field.GetFieldSizeY();
const int XFIELD = g_Field.GetFieldSizeX();
const int YDRAWSTART = (int)(WINDOW_HEIGHT/2 - (YFIELD/2.0f) * BOXSIZE);
const int XDRAWSTART = (int)(WINDOW_WIDTH/2 - (XFIELD/2.0f) * BOXSIZE);
double OPERATETIME = 10;


// キーバッファ宣言
char g_KeyBuf[256]={0};

// マウス情報
POINT g_CursorPos;
bool flgMouseLeft;
bool flgMouseRight;
bool flgMouseMiddle;

// フラグ
bool flgServer;
bool flgConnecting;

// プログラムは WinMain から始まります
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

	// Window設定
	ChangeWindowMode(true);
	SetMainWindowText("巡りマス");
	SetWindowSizeChangeEnableFlag(false);
	SetGraphMode(WINDOW_WIDTH,WINDOW_HEIGHT,32);
	SetBackgroundColor(255,255,255);

	// 初期化処理
	if( DxLib_Init() == -1 ) return -1 ;			// エラーが起きたら直ちに終了

	// --------------------------------------------------------------
	// ボタン作成
	// --------------------------------------------------------------
	g_MenuTop = new Menu;
	g_MenuPlaying = new Menu*[2];
	g_MenuPlaying[0] = new Menu;
	g_MenuPlaying[1] = new Menu;
	
	FILE *fp;
	double time = 0;
	fopen_s(&fp, "..\\Operate_Time.txt","r");
	if (fp != nullptr) {
		fscanf_s(fp, "%lf", &time);
		fclose(fp);
	}
	if (time > 0) {
		OPERATETIME = time;
	}

	// トップメニュー(通信確認）
	{
		string test = string::basic_string("Test");
		REGION RegBut = {(WINDOW_WIDTH/16), (WINDOW_HEIGHT/4), WINDOW_WIDTH/4, WINDOW_HEIGHT/2};
		//g_MenuTop->AddButton(RegBut, "Server", "他マシンに接続する");
		//RegBut.posx += WINDOW_WIDTH/3;
		//g_MenuTop->AddButton(RegBut, "Client", "接続を待機する");
		RegBut.posx += WINDOW_WIDTH/3;
		g_MenuTop->AddButton(RegBut, "None", "接続せず開始する");
	}

	// メインボタン(操作用ボタン)
	{
		// エージェント1,2
		std::string Str;

		for(int i=0; i<AGENTPEOPLE; i++){

			REGION RegBut = {WINDOW_WIDTH/24, WINDOW_HEIGHT/3, BUTTONSIZE - 1, BUTTONSIZE - 1};
			if (i == 1) RegBut = REGION(WINDOW_WIDTH - WINDOW_WIDTH/24 - 3 * BUTTONSIZE, WINDOW_HEIGHT/3, BUTTONSIZE - 1, BUTTONSIZE - 1);


		g_MenuPlaying[i]->AddButton(RegBut, "UpLeft", "");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "Up", "↑");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "UpRight", "");

		RegBut.posx -= 2 * BUTTONSIZE;
		RegBut.posy += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "Left", "←");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "Centor", "●");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "Right", "→");

		RegBut.posx -= 2 * BUTTONSIZE;
		RegBut.posy += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "DownLeft", "");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "Down", "↓");
		RegBut.posx += BUTTONSIZE;
		g_MenuPlaying[i]->AddButton(RegBut, "DownRight", "");

		}


	}



	// メインループ
	while (g_KeyBuf[KEY_INPUT_ESCAPE] == 0 
		&& ProcessMessage() == 0 
		&& ScreenFlip() == 0
		&& ClearDrawScreen()==0 
		&& SetDrawScreen( DX_SCREEN_BACK )==0
		&& GetHitKeyStateAll(g_KeyBuf) == 0) {

		Update(g_Timer);
		Render(g_Timer);

		// とりあえずここでモードチェンジ
		g_SysMode = g_NewSysMode;
		g_Timer.Tick([&](){});

	}

	
	

	// 接続終了処理
	if (flgConnecting) {
		CloseNetWork(g_NetHandle);
	}

	DxLib_End() ;				// ＤＸライブラリ使用の終了処理

	return 0 ;				// ソフトの終了 
}

// --------------------------------------------------------------
// 毎フレーム処理
// --------------------------------------------------------------
void Update(const DX::StepTimer& Timer) {
	// 全フェーズ共通処理

	// マウス情報取得
	{
		int MouseFlags;
		int x, y;
		GetMousePoint(&x, &y);
		g_CursorPos.x = x;
		g_CursorPos.y = y;

		MouseFlags = GetMouseInput();
		flgMouseLeft =		(MouseFlags & MOUSE_INPUT_LEFT) ? true : false;
		flgMouseRight =		(MouseFlags & MOUSE_INPUT_RIGHT) ? true: false;
		flgMouseMiddle =	(MouseFlags & MOUSE_INPUT_MIDDLE)? true: false;
	}


	static double ElapsedTime = 0;
	// フェーズ別処理
	switch (g_SysMode) {

	// トップ画面
	case SYS_START:
		
		if (flgMouseLeft) {
			// ボタンイベント走査
			ButtonClick();

		}

		break;

	// 接続フェーズ
	case SYS_CONNECTING:
		if(flgServer == true){
			// 画面の初期化
			ClearDrawScreen() ;

			// 相手IPアドレスの入力
			char StrIpAdrs[STR_MAX];
			POINT pos = {WINDOW_WIDTH/2,WINDOW_HEIGHT/2 - 100};
			DrawStringCentor(pos,"相手IPアドレスを入力してください(IPv4)", BLACK);
			SetKeyInputStringColor(BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK);
			if (KeyInputSingleCharString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50, 16, StrIpAdrs, true) != 1) {
				ChangeSysMode(SYS_START);
				break;
			}


			// IPDATA用に分割
			char *OutIP[STR_MAX];

			// 不正入力除外（多分できてない）
			if (StringSplit(OutIP, StrIpAdrs, '.') != 4) {
				ChangeSysMode(SYS_START);
				break;
			}

			// 入力したIPに接続
			IPDATA IpAdrs;
			IpAdrs.d1 = atoi(OutIP[0]);
			IpAdrs.d2 = atoi(OutIP[1]);
			IpAdrs.d3 = atoi(OutIP[2]);
			IpAdrs.d4 = atoi(OutIP[3]);
			g_NetHandle = ConnectNetWork(IpAdrs, PORT);
			if (g_NetHandle != -1) {
				flgConnecting = true;
				ChangeSysMode(SYS_OPERATE);
			}
			else {
				ChangeSysMode(SYS_START);
			}

		}
		else {
			PreparationListenNetWork(PORT);

			// 接続が確認できるまで待機
			POINT pos = {WINDOW_WIDTH/2,WINDOW_HEIGHT/2 - 100};
			DrawStringCentor(pos,"待機中です", BLACK);
			g_NetHandle = GetNewAcceptNetWork();

			// 接続が確認できれば次に進む
			if(g_NetHandle != -1){
				flgConnecting = true;
				ChangeSysMode(SYS_OPERATE);
			}
		}
		break;

	// 意思表示フェーズ
	case SYS_OPERATE:
		if (g_TimeAnim.GetNotifyFlag(0)) {
			ChangeSysMode(SYS_MOVE);
		}
		if (flgMouseLeft) {
			// ボタンイベント走査
			ButtonClick();

		}

		break;

	// 移動フェーズ
	case SYS_MOVE:
		if(g_Field.Move() == -1) ChangeSysMode(SYS_END);
		else					ChangeSysMode(SYS_OPERATE);
		break;
	}
}

// --------------------------------------------------------------
// 毎フレーム描画処理
// --------------------------------------------------------------
void Render(const DX::StepTimer& Timer) {

	if (g_SysMode == SYS_START) {
		g_MenuTop->Render();
	}else if (g_SysMode == SYS_CONNECTING){

	}else{
		// とりあえず描画
		for (int y = 0; y < YFIELD; y++) {
			for (int x = 0; x < XFIELD; x++) {

				// ステイタスに応じたフィールドボックス生成
				int FState = g_Field.GetFieldState(x,y);
				unsigned int FColor = GetStateColor(FState);
				
				REGION FBox = {x*BOXSIZE + XDRAWSTART, y*BOXSIZE + YDRAWSTART, BOXSIZE, BOXSIZE};
				DrawBox(FBox, FColor, true);
				DrawBox(FBox, BLACK, false);

				// 点数描画
				int FScore = g_Field.GetFieldScore(x,y);
				char SScore[50];
				sprintf_s(SScore,"%d",FScore);
				DrawString(x*BOXSIZE+XDRAWSTART+20,y*BOXSIZE+YDRAWSTART+20,SScore,BLACK);
			}
		}

		// エージェント座標取得
		POINT AllyPos[2],EnemyPos[2];
		g_Field.GetAllyPos(AllyPos);
		g_Field.GetEnemyPos(EnemyPos);

		DrawCircle(AllyPos[0].x*BOXSIZE + XDRAWSTART + BOXSIZE/2, AllyPos[0].y*BOXSIZE + YDRAWSTART + BOXSIZE/2, BOXSIZE/2, DEEPSKYBLUE, false);
		DrawCircle(AllyPos[1].x*BOXSIZE + XDRAWSTART + BOXSIZE/2, AllyPos[1].y*BOXSIZE + YDRAWSTART + BOXSIZE/2, BOXSIZE/2, DEEPSKYBLUE, false);
		DrawCircle(EnemyPos[0].x*BOXSIZE + XDRAWSTART + BOXSIZE/2, EnemyPos[0].y*BOXSIZE + YDRAWSTART + BOXSIZE/2, BOXSIZE/2, DARKORANGE, false);
		DrawCircle(EnemyPos[1].x*BOXSIZE + XDRAWSTART + BOXSIZE/2, EnemyPos[1].y*BOXSIZE + YDRAWSTART + BOXSIZE/2, BOXSIZE/2, DARKORANGE, false);


		// 各チームスコア表示
		int AllyScore = g_Field.GetAllyTotalScore();
		int EnemyScore = g_Field.GetEnemyTotalScore();

		static char SScore[50];
		sprintf_s(SScore,"Ally Team Score %d",AllyScore);
		DrawString(10, 50, SScore, CYAN);
		sprintf_s(SScore,"Enemy Team Score %d",EnemyScore);
		DrawString(10, 100, SScore, GOLD);

		char StrCent[STR_MAX];
		sprintf_s(StrCent,"残り%dターン  %3.1lf秒", g_Field.GetTurnRemain(), OPERATETIME*(1 - g_TimeAnim.GetNotifyFlac(0)));
		POINT pos = {WINDOW_WIDTH/2, 20};
		DrawStringCentor(pos, StrCent, BLACK);


		// 操作ボタン表示
		for (int i = 0; i < AGENTPEOPLE; i++) {
			g_MenuPlaying[i]->Render();
		}

		// 操作ボタンから操作するエージェントまで線を引く
		for (int i = 0; i < AGENTPEOPLE; i++) {
			CButton* CentorBut = nullptr;
			CentorBut = g_MenuPlaying[i]->GetButtonInfo(4);
			POINT Centor = CentorBut->reg.Centor();

			DrawLine(Centor.x, Centor.y, AllyPos[i].x*BOXSIZE + XDRAWSTART + BOXSIZE/2, AllyPos[i].y*BOXSIZE + YDRAWSTART + BOXSIZE/2, DEEPSKYBLUE);
		}


	}

	// fps取得と描画
	static uint32_t FPS;
	FPS = Timer.GetFramesPerSecond();
	static char SFPS[50];
	sprintf_s(SFPS,"%dfps",FPS);
	DrawString(10,10,SFPS,BLACK);

}

// --------------------------------------------------------------
// ボタンクリック時処理
// --------------------------------------------------------------
void ButtonClick() {
	CButton *ClickButton = nullptr;
	// フェーズ別処理
	switch (g_SysMode) {

	// トップ画面
	case SYS_START:
		
		// サーバー側かクライアント側か
		ClickButton = g_MenuTop->OnCursorButton(g_CursorPos);
		if (ClickButton == nullptr) {
			// nullならret
			return;
		}
		else if(strcmp(ClickButton->name,"Server") == 0) {
			flgServer = true;
			ChangeSysMode(SYS_CONNECTING);
		}
		else if(strcmp(ClickButton->name,"Client") == 0) {
			flgServer = false;
			ChangeSysMode(SYS_CONNECTING);
		}

		// ネットワーク接続をしない(ローカルですべて行う)
		else if(strcmp(ClickButton->name,"None") == 0) {
			flgConnecting = false;
			ChangeSysMode(SYS_OPERATE);
		}

		break;

	case SYS_OPERATE:

		// エージェント操作
		int MoveX, MoveY;
		MoveX = MoveY = 0;
		for (int AgentNo = 0; AgentNo < AGENTPEOPLE; AgentNo++) {
			ClickButton = g_MenuPlaying[AgentNo]->OnCursorButton(g_CursorPos);
			if (ClickButton == nullptr) continue;

			else if (strcmp(ClickButton->name, "UpRight") == 0) {
				MoveX = 1;
				MoveY = -1;
			}
			else if (strcmp(ClickButton->name, "UpLeft") == 0) {
				MoveX = -1;
				MoveY = -1;
			}
			else if (strcmp(ClickButton->name, "DownRight")  == 0) {
				MoveX = 1;
				MoveY = 1;
			}
			else if (strcmp(ClickButton->name, "DownLeft")  == 0) {
				MoveX = -1;
				MoveY = 1;
			}
			else if (strcmp(ClickButton->name, "Up")  == 0) {
				MoveX = 0;
				MoveY = -1;
			}
			else if (strcmp(ClickButton->name, "Left")  == 0) {
				MoveX = -1;
				MoveY = 0;
			}
			else if (strcmp(ClickButton->name, "Right")  == 0) {
				MoveX = 1;
				MoveY = 0;
			}
			else if (strcmp(ClickButton->name, "Down")  == 0) {
				MoveX = 0;
				MoveY = 1;
			}
			else if (strcmp(ClickButton->name, "Centor")  == 0) {
				MoveX = 0;
				MoveY = 0;
			}

			g_Field.MoveStack(true, AgentNo, MoveX, MoveY);
		}

		break;
	}
}

// --------------------------------------------------------------
// モードチェンジ
// --------------------------------------------------------------
void ChangeSysMode(SYSMODE NewSysMode) {
	switch (NewSysMode) {
		case SYS_OPERATE:
			// とりあえず操作時間を10秒に
			g_TimeAnim.Reset();
			g_TimeAnim.Start(0, 30000, 0, 1);
			g_TimeAnim.AddNotify((DWORD)(OPERATETIME * 1000));

			// ボタンハイライトリセット
			g_MenuPlaying[0]->ResetHighLight();
			g_MenuPlaying[1]->ResetHighLight();
		break;

		case SYS_MOVE:

			// ネット接続をしていれば相手の移動情報の取得
			if (flgConnecting) {

			}

			// ネット接続してない場合何かしらのアルゴリズムで敵の移動先を決定
			else {
				g_Field.MoveStackAuto(false, 0, MOVE_SCORERAND);
				g_Field.MoveStackAuto(false, 1, MOVE_SCORERAND);
				//g_Field.MoveStackAuto(true, 0, MOVE_SCORERAND);
				//g_Field.MoveStackAuto(true, 1, MOVE_SCORERAND);
			}
		break;			

	}
	g_NewSysMode = NewSysMode;
}

// --------------------------------------------------------------
// フィールド情報に合った色を返す
// --------------------------------------------------------------
unsigned int GetStateColor(int State) {

	switch (State) {
	case STATE_NONE:
		return WHITE;
	case STATE_ALLYFIELD:
		return CYAN;
	case STATE_ENEMYFIELD:
		return GOLD;
	default:
		return BLACK;
	}
}


// --------------------------------------------------------------
// カーソル座標からフィールド座標への変換
// --------------------------------------------------------------
POINT CursorPos2FieldPos(POINT CursorPos) {
	POINT OutPos;
	OutPos.x = (CursorPos.x - XDRAWSTART)/BOXSIZE;
	OutPos.y = (CursorPos.y - YDRAWSTART)/BOXSIZE;

	return OutPos;
}

// --------------------------------------------------------------
// region用のDrawBox
// --------------------------------------------------------------
int DrawBox(REGION region, unsigned int Color, int FillFlag) {
	RECT rect = region.Rect();
	return DrawBox(rect.left, rect.top, rect.right, rect.bottom, Color, FillFlag);
}

// --------------------------------------------------------------
// 中央座標指定で描画するDrawString
// --------------------------------------------------------------
int DrawStringCentor(POINT centor, const char *String, UINT Color) {
	int width,height;
	int StrLen = strlen(String);
	width = GetDrawStringWidth(String, StrLen);
	height = GetFontSize();

	return DrawString(centor.x - width/2, centor.y - height/2, String, Color);
}


// --------------------------------------------------------------
// 特定文字で文字列分割
// --------------------------------------------------------------
int StringSplit(char *dst[], char *src, const char delim) {
	int count = 0;

	for (;;) {
		while (*src == delim) {
			src++;
		}

		if (*src == '\0') break;

		dst[count++] = src;

		while (*src && !(*src == delim)) {
			src++;
		}

		if (*src == '\0') break;

		*src++ = '\0';
	}
	return count;
}

// --------------------------------------------------------------
// エージェントナンバー取得(文字列の末尾数値を返す)
// --------------------------------------------------------------
int GetAgentNo(char *str) {
	int i = 0;
	while(str[i] != '\0') i++;
	i--;
	return atoi(&str[i]);
}

// --------------------------------------------------------------
// 重み付き乱数生成の結果を返す
// --------------------------------------------------------------
int GetWeightRandom(int WeightArr[9]) {
	int total = 1;
	//int size = sizeof(WeightArr)/sizeof(int);
	int size = 9;

	for (int i = 0; i < size; i++) {
		total += WeightArr[i];
	}
	
	int retIndex = 0;
	int randvalue = rand() % total;
	for (int i = 0; i < size; i++) {
		randvalue -= WeightArr[i];
		if (randvalue < 0) {
			retIndex = i;
			break;
		}
	}

	return retIndex;
}
