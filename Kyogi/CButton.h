//////////////////////////////////////////////
//	�{�^���N���X
//	�{�^���̍쐬�ƃN���b�N�̔�����s��
//	�g�p����e�N�X�`���n���h���Ɩ��O���������Ă���
//////////////////////////////////////////////
#pragma once
#include "DxLib.h"
#include "Region.h"
#include <string>

class CButton {
	public:
		
		CButton(){};
		~CButton(){}

		bool OnCursor(POINT cursor) {
			RECT rec = reg.Rect();
			if(cursor.x >= rec.left && cursor.x <= rec.right
				&& cursor.y >= rec.top && cursor.y <= rec.bottom)
				return true;
			else
				return false;
		}

	REGION reg;
	char name[STR_MAX];
	char print[STR_MAX];
	int GrHandle;
	UINT Color = GetColor(255, 255, 255);
	bool flgHighLight = false;

};
