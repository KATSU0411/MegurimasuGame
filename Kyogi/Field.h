//////////////////////////////////////////////
//	フィールドクラス
//	フィールド操作、各チームの点数計算、各チームのエージェント情報をもっている
//////////////////////////////////////////////

#pragma once
#include "DxLib.h"

#define XMAXFIELD 12
#define YMAXFIELD 12


typedef enum {
	STATE_NONE,
	STATE_ALLYFIELD,		// 自領域
	STATE_ENEMYFIELD,		// 敵領域
	STATE_SEARCHED,			// 探査済み
	STATE_COUNT,
}FieldState;

typedef enum {
	MOVE_RANDOM,
	MOVE_SCORERAND,
	MOVE_SCORE,
	MOVE_TILE,
}MoveAlg;

typedef struct {
	int Score;
	FieldState State;
}FieldInfo;

class Field {
	public:
		Field();
		~Field();

		//void		Update();

		// 取得関数
			// 各チームの点数取得
		int			GetAllyTileScore(){ return m_AllyTileScore; }
		int			GetAllyAreaScore(){ return m_AllyAreaScore; }
		int			GetAllyTotalScore(){ return m_AllyTileScore + m_AllyAreaScore; }
		int			GetEnemyTileScore(){ return m_EnemyTileScore; }
		int			GetEnemyAreaScore(){ return m_EnemyAreaScore; }
		int			GetEnemyTotalScore(){ return m_EnemyTileScore + m_EnemyAreaScore; }

			// フィールド情報取得
		int			GetFieldSizeX(){ return m_XFIELD; }
		int			GetFieldSizeY(){ return m_YFIELD; }
		FieldState	GetFieldState(int x, int y);
		int			GetFieldScore(int x, int y);
		int			GetTurnRemain(){ return m_TURN; }

			// 座標情報取得
		POINT		GetAllyPos(int AgentNo);
		POINT		GetEnemyPos(int AgentNo);
		void		GetAllyPos(POINT *buf);
		void		GetEnemyPos(POINT *buf);


		// 操作関数
		void		MoveStack(bool flgAlly, int AgentNo, int XMove, int YMove);
		void		MoveStackAuto(bool flgAlly, int AgentNo, MoveAlg AlgorithmNo);
		int			Move();

	private:
		// 初期化
		void		Init();

		// 探査関数
		void		SearchScore(bool flgAlly);
		void		ClosedAreaSearch(FieldInfo **SearchField, int SearchState, int x, int y);
		bool		SearchOvelap(POINT Pos);

		// 操作関数
		void		ChangeFieldInfo(int x, int y, FieldState State);

		// 移動アルゴリズム
		void		MoveRandom(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveScoreRandom(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveScore(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveTile(const POINT* AgentPos, POINT* AgentNextPos);

		// QR用の文字列生成
		void		EncodeQR(bool flgAlly);


		int			GenerateScore();
		void		GenerateField(int x = 0, int y = 0);


	public:

	private:
		FieldInfo		**m_pField;
		int				m_XFIELD = 0;
		int				m_YFIELD = 0;
		int				m_TURN = 0;

		POINT			m_AllyPos[2];
		POINT			m_AllyNextPos[2];
		POINT			m_EnemyPos[2];
		POINT			m_EnemyNextPos[2];
	

		// 各チームのタイルスコアと領域スコア
		int				m_AllyTileScore = 0;
		int				m_AllyAreaScore = 0;
		int				m_EnemyTileScore = 0;
		int				m_EnemyAreaScore = 0;

		bool			m_flgHrizon;
		
		
		
};