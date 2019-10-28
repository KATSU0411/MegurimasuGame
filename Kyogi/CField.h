//////////////////////////////////////////////
//	フィールドクラス
//	フィールド操作と各チームの点数計算のみを行う
//////////////////////////////////////////////

#pragma once
#define XMAXFIELD 12
#define YMAXFIELD 12


typedef enum {
	STATE_NONE,
	STATE_ALLYFIELD,		// 自領域
	STATE_ENEMYFIELD,		// 敵領域
}FieldState;

typedef struct {
	int Point;
	FieldState State;
}FieldInfo;

class Field {
	public:
		Field(int x, int y);
		~Field();

		void Update();

		// 取得関数
		int		GetAllyPoint();
		int		GetEnemyPoint();
		bool		GetAllyWin();

		// 操作関数
		void		ChangeFieldInfo(int x, int y, int State);

	private:
		// 初期化
		int		Init(int x, int y);

		// 点数探査
		int		SearchPoint(bool flgAlly);

	public:

	private:
		FieldInfo		**m_Field;
		int			m_XFIELD = 0;
		int			m_YFIELD = 0;
		
		
		
};