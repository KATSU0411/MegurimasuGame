//////////////////////////////////////////////
//	�t�B�[���h�N���X
//	�t�B�[���h����A�e�`�[���̓_���v�Z�A�e�`�[���̃G�[�W�F���g���������Ă���
//////////////////////////////////////////////

#pragma once
#include "DxLib.h"

#define XMAXFIELD 12
#define YMAXFIELD 12


typedef enum {
	STATE_NONE,
	STATE_ALLYFIELD,		// ���̈�
	STATE_ENEMYFIELD,		// �G�̈�
	STATE_SEARCHED,			// �T���ς�
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

		// �擾�֐�
			// �e�`�[���̓_���擾
		int			GetAllyTileScore(){ return m_AllyTileScore; }
		int			GetAllyAreaScore(){ return m_AllyAreaScore; }
		int			GetAllyTotalScore(){ return m_AllyTileScore + m_AllyAreaScore; }
		int			GetEnemyTileScore(){ return m_EnemyTileScore; }
		int			GetEnemyAreaScore(){ return m_EnemyAreaScore; }
		int			GetEnemyTotalScore(){ return m_EnemyTileScore + m_EnemyAreaScore; }

			// �t�B�[���h���擾
		int			GetFieldSizeX(){ return m_XFIELD; }
		int			GetFieldSizeY(){ return m_YFIELD; }
		FieldState	GetFieldState(int x, int y);
		int			GetFieldScore(int x, int y);
		int			GetTurnRemain(){ return m_TURN; }

			// ���W���擾
		POINT		GetAllyPos(int AgentNo);
		POINT		GetEnemyPos(int AgentNo);
		void		GetAllyPos(POINT *buf);
		void		GetEnemyPos(POINT *buf);


		// ����֐�
		void		MoveStack(bool flgAlly, int AgentNo, int XMove, int YMove);
		void		MoveStackAuto(bool flgAlly, int AgentNo, MoveAlg AlgorithmNo);
		int			Move();

	private:
		// ������
		void		Init();

		// �T���֐�
		void		SearchScore(bool flgAlly);
		void		ClosedAreaSearch(FieldInfo **SearchField, int SearchState, int x, int y);
		bool		SearchOvelap(POINT Pos);

		// ����֐�
		void		ChangeFieldInfo(int x, int y, FieldState State);

		// �ړ��A���S���Y��
		void		MoveRandom(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveScoreRandom(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveScore(const POINT* AgentPos, POINT* AgentNextPos);
		void		MoveTile(const POINT* AgentPos, POINT* AgentNextPos);

		// QR�p�̕����񐶐�
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
	

		// �e�`�[���̃^�C���X�R�A�Ɨ̈�X�R�A
		int				m_AllyTileScore = 0;
		int				m_AllyAreaScore = 0;
		int				m_EnemyTileScore = 0;
		int				m_EnemyAreaScore = 0;

		bool			m_flgHrizon;
		
		
		
};