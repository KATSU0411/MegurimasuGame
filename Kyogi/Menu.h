//////////////////////////////////////////////
//	メニュークラス
//	ボタンの管理
//////////////////////////////////////////////

#pragma once
#include "Main.h"
#include "DxLib.h"
#include "Region.h"
#include "CButton.h"
#include <vector>


class Menu {
	public:
		Menu();
		~Menu();
		
		void		Update();
		void		Render();

		void		AddButton(RECT rect, std::string StrName = "", std::string StrPrint = "", int GrHandle = -1, UINT Color = GRAY);		
		void		AddButton(REGION reg, std::string StrName = "", std::string StrPrint = "", int GrHandle = -1, UINT Color = GRAY);		
		CButton*	OnCursorButton(POINT CursorPos);
		void		ResetHighLight();

		CButton*	GetButtonInfo(UINT no);


	private:

	private:
		std::vector<CButton> m_pButton;

};
