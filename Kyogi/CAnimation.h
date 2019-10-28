#pragma once

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// �C���N���[�h�w�b�_
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#include <Windows.h>
#include <vector>
using namespace std;

#include "StepTimer.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	�\���̒�`
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct _NOTIFY
{
	_NOTIFY() {
		flag = false;
	}
	DWORD	elapse;
	bool	flag;
} ANIM_NOTIFY;


namespace PROCON
{
	enum ANIMSTATUS
	{
		OnAnim,
		OnWait,
		OnFinish,
		InValid,
	};

	//===================================================================
	// CAnimation:  �A�j���[�V�����p�N���X
	//===================================================================
	class CAnimation
	{
	private:
		DX::StepTimer m_Timer;

		DWORD		StartTime;				// �J�n����
		DWORD		EndTime;				// �I������
		DWORD		During;					// �A�j���[�V��������
		DWORD		WaitTime;				// �J�n�O�̑҂�����
		DWORD		Offset;					// �I�t�Z�b�g

		float		StartValue;				// �J�n�l
		float		EndValue;				// �I���l
		ANIMSTATUS	Status;					// �I���t���O

		vector<ANIM_NOTIFY>	m_Notify;

		DWORD		getTime();

	public:

		CAnimation();						// �R���X�g���N�^
		~CAnimation();						// �f�X�g���N�^

		ANIMSTATUS GetStatus() const { return Status; }

		bool	Start(DWORD wait, DWORD during, float val_s, float val_e);	// �J�n
		void	Stop();								// �A�j���[�V������~
		float	Get(ANIMSTATUS *status);			// �l�̎擾

		void	Reset();

		// �ʒm
		void	AddNotify(DWORD elapse);
		bool	GetNotify(UINT ID);
		bool	GetNotifyFlag(UINT ID);
		bool	GetNotNotifyFlag(UINT ID);
		float	GetNotifyFlac(UINT ID);

	};
}