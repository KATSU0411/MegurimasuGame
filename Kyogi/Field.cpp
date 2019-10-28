// Field�N���X
#include <stdlib.h>
#include <time.h>
#include <string>
#include <fstream>

#include "Field.h"
#include "Main.h"

// --------------------------------------------------------------
// �R���X�g���N�^
// --------------------------------------------------------------
Field::Field() {

	srand((unsigned int)time(NULL));						// �����평����
	Init();

}

// --------------------------------------------------------------
// �f�X�g���N�^
// --------------------------------------------------------------
Field::~Field() {

	for (int i = 0; i < m_YFIELD; i++) {
		delete[] m_pField[i];
		m_pField[i] = 0;
	}
	delete[] m_pField;
	m_pField = 0;

}

// --------------------------------------------------------------
// ����������
// --------------------------------------------------------------
void Field::Init() {

	GenerateField();

	// �G�[�W�F���g�����ʒu
	POINT pos[2];
	bool flgPosOK = false;
	pos[0].x = rand()%((m_XFIELD)/2 - m_XFIELD % 2);
	pos[0].y = rand()%((m_YFIELD)/2 - m_YFIELD % 2);
	pos[1].x = rand()%((m_XFIELD)/2 + m_XFIELD % 2) + m_XFIELD/2;
	pos[1].y = rand()%((m_YFIELD)/2 + m_YFIELD % 2) + m_YFIELD/2;
	if(m_flgHrizon){
		if (rand() % 2 == 0) {
			m_AllyPos[0] = m_EnemyPos[0] = pos[0];
			m_AllyPos[1] = m_EnemyPos[1] = pos[1];
			m_EnemyPos[0].y = - m_EnemyPos[0].y + m_YFIELD - 1;
			m_EnemyPos[1].y = - m_EnemyPos[1].y + m_YFIELD - 1;
		}
		else {
			m_AllyPos[0] = m_EnemyPos[0] = pos[0];
			m_AllyPos[1] = m_EnemyPos[1] = pos[1];
			m_AllyPos[0].y =  - m_AllyPos[0].y + m_YFIELD - 1;
			m_AllyPos[1].y =  - m_AllyPos[1].y + m_YFIELD - 1;
		}
	}
	else {
		if (rand() % 2 == 0) {
			m_AllyPos[0] = m_EnemyPos[0] = pos[0];
			m_AllyPos[1] = m_EnemyPos[1] = pos[1];
			m_EnemyPos[0].x = - m_EnemyPos[0].x + m_XFIELD - 1;
			m_EnemyPos[1].x = - m_EnemyPos[1].x + m_XFIELD - 1;
		}
		else {
			m_AllyPos[0] = m_EnemyPos[0] = pos[0];
			m_AllyPos[1] = m_EnemyPos[1] = pos[1];
			m_AllyPos[0].x =  - m_AllyPos[0].x + m_XFIELD - 1;
			m_AllyPos[1].x =  - m_AllyPos[1].x + m_XFIELD - 1;
		}

	}

	m_pField[m_AllyPos[0].y][m_AllyPos[0].x].State = STATE_ALLYFIELD;
	m_pField[m_AllyPos[1].y][m_AllyPos[1].x].State = STATE_ALLYFIELD;
	m_pField[m_EnemyPos[0].y][m_EnemyPos[0].x].State = STATE_ENEMYFIELD;
	m_pField[m_EnemyPos[1].y][m_EnemyPos[1].x].State = STATE_ENEMYFIELD;

	m_AllyNextPos[0] = m_AllyPos[0];
	m_AllyNextPos[1] = m_AllyPos[1];
	m_EnemyNextPos[0] = m_EnemyPos[0];
	m_EnemyNextPos[1] = m_EnemyPos[1];

	SearchScore(false);
	SearchScore(true);

	EncodeQR(false);
	EncodeQR(true);
}

// --------------------------------------------------------------
// �w�肵�����[�U�̓_������
// --------------------------------------------------------------
void Field::SearchScore(bool flgAlly) {
	// flgAlly	true�Ȃ疡���̓_��
	//			false�Ȃ�G�̓_���𑖍�

	int *TileScore = nullptr;
	int *AreaScore = nullptr;
	int SearchState = 0;
	static const int XFieldSize = sizeof(FieldInfo) * m_XFIELD;

	// �ǂ���̃`�[���𑖍�����̂��̎w��ƃ[���|�C���g��
	if(flgAlly){
		SearchState = STATE_ALLYFIELD ;
		TileScore = &m_AllyTileScore;
		AreaScore = &m_AllyAreaScore;
	}else{
		SearchState = STATE_ENEMYFIELD ;
		TileScore = &m_EnemyTileScore;
		AreaScore = &m_EnemyAreaScore;
	}
	*TileScore = *AreaScore = 0;

	// �����p�t�B�[���h�z��
	FieldInfo **SearchField;

	// �m�ۂƃR�s�[
	// ����m�ۂ���̕|���������o�ϐ��ɂ��ׂ����H
	SearchField = new FieldInfo*[m_YFIELD];
	for (int i = 0; i < m_YFIELD; i++) {
		SearchField[i] = new FieldInfo[m_XFIELD];
		CopyMemory(SearchField[i], m_pField[i], XFieldSize);
	}

	// �ċA�I�ɗ̈�G���A�𑖍�
	for (int y = 0; y < m_YFIELD; y++) {
		ClosedAreaSearch(SearchField, SearchState, 0, y);
		ClosedAreaSearch(SearchField, SearchState, m_XFIELD - 1, y);
	}
	for (int x = 0; x < m_XFIELD; x++) {
		ClosedAreaSearch(SearchField, SearchState, x, 0);
		ClosedAreaSearch(SearchField, SearchState, x, m_YFIELD - 1);
	}

	// �_������
	for (int y = 0; y < m_YFIELD; y++) {
		for (int x = 0; x < m_XFIELD; x++) {
			if(SearchField[y][x].State == STATE_SEARCHED)continue;
			// �^�C���|�C���g���Z
			if(SearchField[y][x].State == SearchState) {
				*TileScore += SearchField[y][x].Score;
				continue;
			}

			// �̈�|�C���g�v�Z
			else {
				if (SearchField[y][x].Score < 0) {
					*AreaScore += -(SearchField[y][x].Score);
				}else{
					*AreaScore += SearchField[y][x].Score;
				}
			}

		}
	}
}

// --------------------------------------------------------------
// �ċA�I�Ɉ͂܂�Ă��Ȃ��̈��S�Ēׂ�
// --------------------------------------------------------------
void Field::ClosedAreaSearch(FieldInfo **SearchField, int SearchState, int x, int y){
	// �̈�O�̏ꍇret
	if(x<0 || x >= m_XFIELD || y<0 || y >= m_YFIELD) return;

	// �����ς݂܂��͑����`�[���̗̈�ł���ꍇret
	if(SearchField[y][x].State == SearchState ||
		SearchField[y][x].State == STATE_SEARCHED) return;


	SearchField[y][x].State = STATE_SEARCHED;

	ClosedAreaSearch(SearchField, SearchState, x + 1, y);
	ClosedAreaSearch(SearchField, SearchState, x - 1, y);
	ClosedAreaSearch(SearchField, SearchState, x, y + 1);
	ClosedAreaSearch(SearchField, SearchState, x, y - 1);

	return;
}

// --------------------------------------------------------------
// �G�[�W�F���g�̈ړ��ۑ�(���΍��W)
// --------------------------------------------------------------
void Field::MoveStack(bool flgAlly, int AgentNo, int XMove, int YMove) {
	
	if(!(AgentNo == 0 || AgentNo == 1)) return;
	if(XMove < -1 || XMove > 1) return;
	if(YMove < -1 || YMove > 1) return;

	POINT *AgentNextPos = nullptr;	
	POINT *AgentPos = nullptr;
	POINT OutPos = {0,0};

	// �ǂ���̃`�[�����ړ�������̂�
	if(flgAlly){
		AgentNextPos = &m_AllyNextPos[AgentNo];
		AgentPos = &m_AllyPos[AgentNo];
	}else{
		AgentNextPos = &m_EnemyNextPos[AgentNo];
		AgentPos = &m_EnemyPos[AgentNo];
	}
	AgentNextPos->x = AgentPos->x + XMove;
	AgentNextPos->y = AgentPos->y + YMove;


	// �͈͊O�ɂȂ�ꍇ�͏C��
	if (AgentNextPos->x < 0) AgentNextPos->x = 0;
	else if(AgentNextPos->x >= m_XFIELD)AgentNextPos->x = m_XFIELD - 1;

	if (AgentNextPos->y < 0) AgentNextPos->y = 0;
	else if(AgentNextPos->y >= m_YFIELD)AgentNextPos->y = m_YFIELD - 1;
	

	return ;
}

// --------------------------------------------------------------
// �G�[�W�F���g�̈ړ��ۑ�(�I�[�g)
// --------------------------------------------------------------
void Field::MoveStackAuto(bool flgAlly, int AgentNo, MoveAlg AlgorithmNo) {

	if(!(AgentNo == 0 || AgentNo == 1)) return;

	POINT *AgentNextPos = nullptr;	
	POINT *AgentPos = nullptr;

	// �ǂ���̃`�[�����ړ�������̂�
	if(flgAlly){
		AgentNextPos = &m_AllyNextPos[AgentNo];
		AgentPos = &m_AllyPos[AgentNo];
	}else{
		AgentNextPos = &m_EnemyNextPos[AgentNo];
		AgentPos = &m_EnemyPos[AgentNo];
	}

	switch (AlgorithmNo) {
		case MOVE_RANDOM: 
			MoveRandom(AgentPos, AgentNextPos);
			break;
		
		case MOVE_SCORERAND:
			MoveScoreRandom(AgentPos, AgentNextPos);
			break;

	}


	// �͈͊O�ɂȂ�ꍇ�͏C��
	if (AgentNextPos->x < 0) AgentNextPos->x = 0;
	else if(AgentNextPos->x >= m_XFIELD)AgentNextPos->x = m_XFIELD - 1;

	if (AgentNextPos->y < 0) AgentNextPos->y = 0;
	else if(AgentNextPos->y >= m_YFIELD)AgentNextPos->y = m_YFIELD - 1;
}

// --------------------------------------------------------------
// �ړ��A���S���Y���Q
// --------------------------------------------------------------

// ���S�����_��
void Field::MoveRandom(const POINT* AgentPos, POINT* AgentNextPos) {
	
	int XMove, YMove;
	XMove = rand()%3 - 1;
	YMove = rand()%3 - 1;

	AgentNextPos->x = AgentPos->x + XMove;
	AgentNextPos->y = AgentPos->y + YMove;
	return;
}

// �d�ݕt�������_��
void Field::MoveScoreRandom(const POINT* AgentPos, POINT* AgentNextPos) {
	
	int Weight[9];
	int total = 0;

	// �܂�����̏󋵎擾
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			int x = j-1;
			int y = i-1;
			Weight[i*3 + j] = 0;

			if (AgentNextPos->x + x < 0)				continue;
			else if(AgentNextPos->x + x >= m_XFIELD)	continue;

			else if (AgentNextPos->y + y < 0)			continue;
			else if(AgentNextPos->y + y >= m_YFIELD)	continue;

			Weight[i*3 + j] = 1;

			if(m_pField[AgentPos[i].y + y][AgentPos[i].x + x].State == m_pField[AgentPos[i].y][AgentPos[i].x].State) continue;


			Weight[i*3 + j] = m_pField[AgentPos[i].y + y][AgentPos[i].x + x].Score + 18;
		}
	}

	int Index = GetWeightRandom(Weight);

	int XMove = Index % 3 - 1;
	int YMove = Index / 3 - 1;

	AgentNextPos->x = AgentPos->x + XMove;
	AgentNextPos->y = AgentPos->y + YMove;
	return;
}

// --------------------------------------------------------------
// �G�[�W�F���g�̈ړ�
// --------------------------------------------------------------
int Field::Move(){

	bool flgAllySuffer[2];
	bool flgEnemySuffer[2];
	for(int i=0; i<2; i++){
		// ����Ă���Έړ��𖳌���
		flgAllySuffer[i] = SearchOvelap(m_AllyNextPos[i]);
		flgEnemySuffer[i] = SearchOvelap(m_EnemyNextPos[i]);
	}

	for(int i=0; i<2; i++){
		if(flgAllySuffer[i]) m_AllyNextPos[i] = m_AllyPos[i];
		if(flgEnemySuffer[i]) m_EnemyNextPos[i] = m_EnemyPos[i];

		// �ړ��悪�G�^�C���ł���Ώ������Ĉړ��𖳌�
		if (m_pField[m_AllyNextPos[i].y][m_AllyNextPos[i].x].State == STATE_ENEMYFIELD) {
			m_pField[m_AllyNextPos[i].y][m_AllyNextPos[i].x].State = STATE_NONE;
			m_AllyNextPos[i] = m_AllyPos[i];
		}

		if (m_pField[m_EnemyNextPos[i].y][m_EnemyNextPos[i].x].State == STATE_ALLYFIELD) {
			m_pField[m_EnemyNextPos[i].y][m_EnemyNextPos[i].x].State = STATE_NONE;
			m_EnemyNextPos[i] = m_EnemyPos[i];
		}

	}

	for(int i=0; i<2; i++){
		m_AllyPos[i] = m_AllyNextPos[i];
		m_EnemyPos[i] = m_EnemyNextPos[i];

		// �ړ���Ɏ��^�C����u��
		m_pField[m_EnemyPos[i].y][m_EnemyPos[i].x].State = STATE_ENEMYFIELD;
		m_pField[m_AllyPos[i].y][m_AllyPos[i].x].State = STATE_ALLYFIELD;
		
	}

	SearchScore(true);
	SearchScore(false);

	m_TURN--;
	if(m_TURN <= 0) return -1;
	else			return 0;
}

// --------------------------------------------------------------
// �ړ��悪����Ă��Ȃ�������
// --------------------------------------------------------------
bool Field::SearchOvelap(POINT Pos) {

	bool Out = true;
	int count = 0;
	for(int j=0; j<2; j++){
		// �ړ��悪����Ă�����ړ��𖳌���
		if ((Pos.x == m_AllyNextPos[j].x
			&& Pos.y == m_AllyNextPos[j].y)) count++;

		if ((Pos.x == m_EnemyNextPos[j].x
			&& Pos.y == m_EnemyNextPos[j].y)) count++;

	}

	if(count == 1) Out = false;
	return Out;
}

// --------------------------------------------------------------
// �w�肵�����W�̃X�e�[�^�X�ύX
// --------------------------------------------------------------
void Field::ChangeFieldInfo(int x, int y, FieldState State){
	
	// �p�����[�^�G���[�Ȃ牽�������Ԃ�(�����Ԃ��������ǂ����H)
	if(!(State>= 0 && State<=STATE_COUNT)) return;

	// ���W�G���[����
	if(x<0 || x >= m_XFIELD || y<0 || y >= m_YFIELD) return;


	m_pField[y][x].State = State;

	return;

}

// --------------------------------------------------------------
// QR�R�[�h�p�̕����񐶐�
// --------------------------------------------------------------
void Field::EncodeQR(bool flgAlly) {
	std::string Out;
	int a;

	// X : Y:
	Out += std::to_string(m_XFIELD) + " " + std::to_string(m_YFIELD) + ":";

	for (int y = 0; y < m_YFIELD; y++) {
		for (int x = 0; x < m_XFIELD; x++) {
			Out += std::to_string(m_pField[y][x].Score) + " ";
		}
		Out.pop_back();
		Out += ":";
	}


	std::string filename;
	// �e�G�[�W�F���g�̈ʒu���
	if (flgAlly) {
		// �����̏ꍇ
		Out += std::to_string(m_AllyPos[0].x) + " " + std::to_string(m_AllyPos[0].y) + ":";
		Out += std::to_string(m_AllyPos[1].x) + " " + std::to_string(m_AllyPos[1].y) + ":";
		filename = "..\\FieldInfo_Red.txt";
	}
	else {
		// �G�̏ꍇ
		Out += std::to_string(m_EnemyPos[0].x) + " " + std::to_string(m_EnemyPos[0].y) + ":";
		Out += std::to_string(m_EnemyPos[1].x) + " " + std::to_string(m_EnemyPos[1].y) + ":";
		filename = "..\\FieldInfo_Blue.txt";
	}
	std::ofstream file(filename);
	file << Out;
	file.close();



}

// --------------------------------------------------------------
// �X�R�A�̃A���S���Y��
// --------------------------------------------------------------
int Field::GenerateScore(){

	int Score = 0;
	//for (int i = 0; i < 16; i++)  Score += (rand() % 100) / 90;
	for (int i = 0; i < 16; i++)  Score += (int)((((16 - Score) / 16.0)*(rand() % 100)) / 70);


	// 10%���炢�Ń}�C�i�X�_
	if (rand() % 10 == 0) {
		Score *= -1;
		if (rand() % 16 == 0) {
			Score = 0;
		}
	}

	return Score;
}

// --------------------------------------------------------------
// �t�B�[���h���� 
// --------------------------------------------------------------
void Field::GenerateField(int x, int y) {
	if (x <= 0) x = rand() % 4 + 9;
	if (y <= 0) y = rand() % 4 + 9;

	m_XFIELD = x;
	m_YFIELD = y;
	m_TURN	 = m_XFIELD * m_YFIELD - 20;
	if(m_TURN > 120) m_TURN = 120;
	m_pField = new FieldInfo*[m_YFIELD];
	for (int i = 0; i < m_YFIELD; i++) {
		m_pField[i] = new FieldInfo[m_XFIELD];
	}


	m_flgHrizon = rand()%2 ? true : false;

	int Score = 0;

	if(m_flgHrizon){
		for (int y = 0; y <= m_YFIELD/2; y++) {
			for (int x = 0; x < m_XFIELD; x++) {
				Score = GenerateScore();
				
				m_pField[y][x].Score =
					m_pField[m_YFIELD - 1 - y][x].Score =
						Score;

				m_pField[y][x].State =
					m_pField[m_YFIELD - 1 - y][x].State =
						STATE_NONE;
			}
		}

	}else {
		for (int y = 0; y < m_YFIELD; y++) {
			for (int x = 0; x <= m_XFIELD/2; x++) {
				Score = GenerateScore();

				m_pField[y][x].Score =
					m_pField[y][m_XFIELD - 1 - x].Score =
						Score;

				m_pField[y][x].State =
					m_pField[y][m_XFIELD - 1 - x].State =
						STATE_NONE;
			}
		}
	}
}


// --------------------------------------------------------------
// Get�֐��Q
// --------------------------------------------------------------

// �w�肵�����W�̃X�e�[�^�X�擾
FieldState	Field::GetFieldState(int x, int y){ 
	if(x<0 || x >= m_XFIELD || y<0 || y >= m_YFIELD) return STATE_NONE;
	return m_pField[y][x].State;
}

// �w�肵�����W�̃X�R�A�擾
int	Field::GetFieldScore(int x, int y){ 
	if(x<0 || x >= m_XFIELD || y<0 || y >= m_YFIELD) return 0;
	return m_pField[y][x].Score;
}

// �w�肵�����ԃG�[�W�F���g�̍��W�擾
POINT Field::GetAllyPos(int AgentNo){ 
	POINT out={0, 0};
	if(!(AgentNo == 0 || AgentNo == 1)) return out;
	out = m_AllyPos[AgentNo]; 
	return out;
}

// �w�肵���G�G�[�W�F���g�̍��W�擾
POINT Field::GetEnemyPos(int AgentNo){
	POINT out={0, 0};
	if(!(AgentNo == 0 || AgentNo == 1)) return out;
	out = m_EnemyPos[AgentNo]; 
	return out;
}

// �w�肵�����ԃG�[�W�F���g�̍��W�擾(�o�b�t�@�n��)
void Field::GetAllyPos(POINT *buf) {
	static const int PosSize = sizeof(POINT) * 2;
	CopyMemory(buf, m_AllyPos, PosSize);
}

// �w�肵���G�G�[�W�F���g�̍��W�擾(�o�b�t�@�n��)
void Field::GetEnemyPos(POINT *buf) {
	static const int PosSize = sizeof(POINT) * 2;
	CopyMemory(buf, m_EnemyPos, PosSize);
}
