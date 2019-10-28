//////////////////////////////////////////////
//	�t�B�[���h�N���X
//	�t�B�[���h����Ɗe�`�[���̓_���v�Z�݂̂��s��
//////////////////////////////////////////////

#pragma once
#define XMAXFIELD 12
#define YMAXFIELD 12


typedef enum {
	STATE_NONE,
	STATE_ALLYFIELD,		// ���̈�
	STATE_ENEMYFIELD,		// �G�̈�
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

		// �擾�֐�
		int		GetAllyPoint();
		int		GetEnemyPoint();
		bool		GetAllyWin();

		// ����֐�
		void		ChangeFieldInfo(int x, int y, int State);

	private:
		// ������
		int		Init(int x, int y);

		// �_���T��
		int		SearchPoint(bool flgAlly);

	public:

	private:
		FieldInfo		**m_Field;
		int			m_XFIELD = 0;
		int			m_YFIELD = 0;
		
		
		
};