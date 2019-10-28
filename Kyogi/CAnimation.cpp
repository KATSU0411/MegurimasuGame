//-------------------------------------------------------------------------
//	インクルード・ヘッダ
//-------------------------------------------------------------------------
#include "pch.h"
#include "CAnimation.h"


using namespace DX;
using namespace PROCON;


//=======================================================================
// コンストラクタ
//=======================================================================
CAnimation::CAnimation()
{
	Reset();
}

//=======================================================================
// デストラクタ
//=======================================================================
CAnimation::~CAnimation()
{
	m_Notify.clear();
}

//=======================================================================
// リセット
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
// アニメーション開始
//	DWORD	during		アニメーション時間[ms]
//	float	val_s		開始値
//	float	val_e		終了値
//	[RETURN]	true: 成功	false: 失敗(アニメーション中)
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
// アニメーション停止
//	DWORD	during		アニメーション時間[ms]
//	float	val_s		開始値
//	float	val_e		終了値
//	[RETURN]	true: 成功	false: 失敗(アニメーション中)
//=======================================================================
void CAnimation::Stop()
{
//	Reset();
	DWORD CurrentTime = getTime();
	EndTime = CurrentTime;
}

//=======================================================================
//	時間を進める
//=======================================================================
DWORD CAnimation::getTime()
{
	m_Timer.Tick([&]() {});

	return (DWORD)(1000.0 * m_Timer.GetTotalSeconds());
}

//=======================================================================
//	値の取得
//	[RETURN]	開始値～終了値の線形補間値
//=======================================================================
float CAnimation::Get(ANIMSTATUS *status)
{

	if (Status == InValid) { *status = InValid; return 0; }

	float	frac = 0;

	DWORD CurrentTime = getTime();

	// 開始前
	if (CurrentTime < StartTime) {
		Status = OnWait;
		frac = 0;
	}

	// アニメーション中
	else if ( CurrentTime>=StartTime && CurrentTime <= EndTime ) {
		Status = OnAnim;
		frac = (float)(CurrentTime-StartTime) / During;
	}

	// 開始前・終了後
	else {
		Status = OnFinish;
		frac = 1;
	}

	*status = Status;

	return frac * (EndValue - StartValue) + StartValue;
}


//=======================================================================
//	通知の追加
//=======================================================================
void CAnimation::AddNotify(DWORD elapse)
{
	ANIM_NOTIFY	notify;

	notify.elapse = elapse;

	m_Notify.push_back(notify);
}


//=======================================================================
//	ID番目の通知を取得　(指定時間を経過した1回のみtrue)
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
//	通知フラグの取得　(指定時間を経過すればずっとtrue)
//=======================================================================
bool CAnimation::GetNotifyFlag(UINT ID)
{
	if (Status == InValid)		return false;
	if (ID >= m_Notify.size())	return false;

	GetNotify(ID);

	return m_Notify[ID].flag;
}

//=======================================================================
//	通知フラグの取得　(指定時間を経過するまでずっとtrue)
//=======================================================================
bool CAnimation::GetNotNotifyFlag(UINT ID)
{
	if (Status == InValid)		return false;
	if (ID >= m_Notify.size())	return false;

	GetNotify(ID);

	return !(m_Notify[ID].flag);
}


////=======================================================================
////	ID番目の通知時のFractionを取得
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
//	ID番目の通知時のFractionを取得
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
