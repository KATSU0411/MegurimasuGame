#pragma once

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// インクルードヘッダ
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#include <Windows.h>
#include <vector>
using namespace std;

#include "StepTimer.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	構造体定義
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
	// CAnimation:  アニメーション用クラス
	//===================================================================
	class CAnimation
	{
	private:
		DX::StepTimer m_Timer;

		DWORD		StartTime;				// 開始時間
		DWORD		EndTime;				// 終了時間
		DWORD		During;					// アニメーション時間
		DWORD		WaitTime;				// 開始前の待ち時間
		DWORD		Offset;					// オフセット

		float		StartValue;				// 開始値
		float		EndValue;				// 終了値
		ANIMSTATUS	Status;					// 終了フラグ

		vector<ANIM_NOTIFY>	m_Notify;

		DWORD		getTime();

	public:

		CAnimation();						// コンストラクタ
		~CAnimation();						// デストラクタ

		ANIMSTATUS GetStatus() const { return Status; }

		bool	Start(DWORD wait, DWORD during, float val_s, float val_e);	// 開始
		void	Stop();								// アニメーション停止
		float	Get(ANIMSTATUS *status);			// 値の取得

		void	Reset();

		// 通知
		void	AddNotify(DWORD elapse);
		bool	GetNotify(UINT ID);
		bool	GetNotifyFlag(UINT ID);
		bool	GetNotNotifyFlag(UINT ID);
		float	GetNotifyFlac(UINT ID);

	};
}