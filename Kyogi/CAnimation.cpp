//-------------------------------------------------------------------------
//	�C���N���[�h�E�w�b�_
//-------------------------------------------------------------------------
#include "pch.h"
#include "CAnimation.h"


using namespace DX;
using namespace PROCON;


//=======================================================================
// �R���X�g���N�^
//=======================================================================
CAnimation::CAnimation()
{
	Reset();
}

//=======================================================================
// �f�X�g���N�^
//=======================================================================
CAnimation::~CAnimation()
{
	m_Notify.clear();
}

//=======================================================================
// ���Z�b�g
//=======================================================================
void CAnimation::Reset()
{
	StartTime = 0;
	WaitTime = 0;

	Status = InValid;
	//Offset = getTime();
	m_Timer.ResetElapsedTime();
	for (UINT i = 0; i < m_Notify.size(); i++) {
		m_Notify[i].flag = false;
	}
	m_Notify.clear();
}

//=======================================================================
// �A�j���[�V�����J�n
//	DWORD	during		�A�j���[�V��������[ms]
//	float	val_s		�J�n�l
//	float	val_e		�I���l
//	[RETURN]	true: ����	false: ���s(�A�j���[�V������)
//=======================================================================
bool CAnimation::Start(DWORD wait, DWORD during, float val_s, float val_e)
{
	WaitTime    = wait;
	During		= during;

	DWORD CurrentTime = getTime();
	//Offset = CurrentTime;
	StartTime	= CurrentTime + WaitTime;
	EndTime		= StartTime + During;

	StartValue	= val_s;
	EndValue	= val_e;

	Status		= OnAnim;

	return true;
}

//=======================================================================
// �A�j���[�V������~
//	DWORD	during		�A�j���[�V��������[ms]
//	float	val_s		�J�n�l
//	float	val_e		�I���l
//	[RETURN]	true: ����	false: ���s(�A�j���[�V������)
//=======================================================================
void CAnimation::Stop()
{
//	Reset();
	DWORD CurrentTime = getTime();
	EndTime = CurrentTime;
}

//=======================================================================
//	���Ԃ�i�߂�
//=======================================================================
DWORD CAnimation::getTime()
{
	m_Timer.Tick([&]() {});

	return (DWORD)(1000.0 * m_Timer.GetTotalSeconds());
}

//=======================================================================
//	�l�̎擾
//	[RETURN]	�J�n�l�`�I���l�̐��`��Ԓl
//=======================================================================
float CAnimation::Get(ANIMSTATUS *status)
{

	if (Status == InValid) { *status = InValid; return 0; }

	float	frac = 0;

	DWORD CurrentTime = getTime();

	// �J�n�O
	if (CurrentTime < StartTime) {
		Status = OnWait;
		frac = 0;
	}

	// �A�j���[�V������
	else if ( CurrentTime>=StartTime && CurrentTime <= EndTime ) {
		Status = OnAnim;
		frac = (float)(CurrentTime-StartTime) / During;
	}

	// �J�n�O�E�I����
	else {
		Status = OnFinish;
		frac = 1;
	}

	*status = Status;

	return frac * (EndValue - StartValue) + StartValue;
}


//=======================================================================
//	�ʒm�̒ǉ�
//=======================================================================
void CAnimation::AddNotify(DWORD elapse)
{
	ANIM_NOTIFY	notify;

	notify.elapse = elapse;

	m_Notify.push_back(notify);
}


//=======================================================================
//	ID�Ԗڂ̒ʒm���擾�@(�w�莞�Ԃ��o�߂���1��̂�true)
//=======================================================================
bool CAnimation::GetNotify(UINT ID)
{
	bool ret = false;

	DWORD CurrentTime = getTime();

	if (ID >= m_Notify.size())		return false;
	if (CurrentTime < StartTime)	return false;
	if (Status == InValid)			return false;

	DWORD	ElapsedTime = CurrentTime - StartTime;

	
	if ( m_Notify[ID].elapse < ElapsedTime ) {
		if ( m_Notify[ID].flag==false )	{
			m_Notify[ID].flag = true;
			ret = true;
		}
	}

	return ret;
}

//=======================================================================
//	�ʒm�t���O�̎擾�@(�w�莞�Ԃ��o�߂���΂�����true)
//=======================================================================
bool CAnimation::GetNotifyFlag(UINT ID)
{
	if (Status == InValid)		return false;
	if (ID >= m_Notify.size())	return false;

	GetNotify(ID);

	return m_Notify[ID].flag;
}

//=======================================================================
//	�ʒm�t���O�̎擾�@(�w�莞�Ԃ��o�߂���܂ł�����true)
//=======================================================================
bool CAnimation::GetNotNotifyFlag(UINT ID)
{
	if (Status == InValid)		return false;
	if (ID >= m_Notify.size())	return false;

	GetNotify(ID);

	return !(m_Notify[ID].flag);
}


////=======================================================================
////	ID�Ԗڂ̒ʒm����Fraction���擾
////=======================================================================
//float CAnimation::GetNotifyFlac(UINT ID)
//{
//	if ( ID >= m_Notify.size() )	return 0;
//
//	GetNotify(ID);
//
//	return  ((float)m_Notify[ID].elapse / During) * (EndValue - StartValue) + StartValue;
//}


//=======================================================================
//	ID�Ԗڂ̒ʒm����Fraction���擾
//=======================================================================
float CAnimation::GetNotifyFlac(UINT ID)
{
	if (ID >= m_Notify.size())	return 0;


	DWORD CurrentTime = getTime();

	DWORD s;
	if (ID == 0) 
		s = StartTime;
	else 
		s = m_Notify[ID - 1].elapse + StartTime;

	DWORD m = m_Notify[ID].elapse + StartTime - s;

	if (CurrentTime < s)	return 0;

	DWORD e = CurrentTime - s;
	if (((float)e / (float)m) > 1) return 1;
	return (float)e / (float)m;


//	return  ((float)m_Notify[ID].elapse / During) * (EndValue - StartValue) + StartValue;
}
