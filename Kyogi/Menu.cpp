#include "Menu.h"

// --------------------------------------------------------------
// �R���X�g���N�^
// --------------------------------------------------------------
Menu::Menu() {

}

// --------------------------------------------------------------
// �f�X�g���N�^
// --------------------------------------------------------------
Menu::~Menu() {
	m_pButton.clear();
}

// --------------------------------------------------------------
// ���t���[������
// --------------------------------------------------------------
void Menu::Update() {

}

// --------------------------------------------------------------
// ���t���[���`�揈��
// --------------------------------------------------------------
void Menu::Render() {


	// �{�^���`��
	for (UINT i=0; i < m_pButton.size(); i++){
		// GrHandle���ݒ�ς݂łȂ���Ύw��F�ŕ`��
		if(m_pButton[i].GrHandle == -1){
		
			DrawBox(m_pButton[i].reg, m_pButton[i].Color, true);

			// �n�C���C�g�t���OON�Ńn�C���C�g�\��
			if (m_pButton[i].flgHighLight) {
				SetDrawBlendMode(DX_BLENDMODE_ADD, 64);
				DrawBox(m_pButton[i].reg, GetColor(255, 255, 255), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		// GrHandle���ݒ�ς݂ł���Ύw��摜��`��
		else {
			RECT rec = m_pButton[i].reg.Rect();
			DrawExtendGraph(rec.left, rec.top, rec.right, rec.bottom, m_pButton[i].GrHandle, true);
		}
		UINT hr = DrawStringCentor(m_pButton[i].reg.Centor(), m_pButton[i].print, BLACK);
	}
}

// --------------------------------------------------------------
// �{�^���ǉ�
// --------------------------------------------------------------
void Menu::AddButton(RECT rect, std::string StrName, std::string StrPrint, int GrHandle, UINT Color){
	REGION reg = rect;
	AddButton(reg, StrName, StrPrint, GrHandle, Color);
}

// --------------------------------------------------------------
// �{�^���ǉ�
// --------------------------------------------------------------
void Menu::AddButton(REGION reg, std::string StrName, std::string StrPrint, int GrHandle, UINT Color){
	
	CButton AddBut;
	AddBut.reg = reg;
	std::char_traits<char>::copy(AddBut.name, StrName.c_str(), StrName.size() + 1);
	//StrName.copy(AddBut.name, StrName.size() + 1);
	AddBut.GrHandle = GrHandle;
	AddBut.Color = Color;
	std::char_traits<char>::copy(AddBut.print, StrPrint.c_str(), StrPrint.size() + 1);
	//StrPrint.copy(AddBut.print, StrPrint.size() + 1);

	m_pButton.push_back(AddBut);
	return;
}

// --------------------------------------------------------------
// �J�[�\����ɂ���{�^���|�C���^��Ԃ�(�������null)
// --------------------------------------------------------------
CButton* Menu::OnCursorButton(POINT CursorPos) {
		
	CButton* OutBut=nullptr;
	for (UINT i=0; i < m_pButton.size(); i++){
		if(m_pButton[i].OnCursor(CursorPos))
			OutBut = &m_pButton[i];
	}

	if (OutBut != nullptr) {
		for (UINT i=0; i < m_pButton.size(); i++){
			m_pButton[i].flgHighLight = false;
		}
		OutBut->flgHighLight = true;
	}
	return OutBut;
}

// --------------------------------------------------------------
// �n�C���C�g���Z�b�g
// --------------------------------------------------------------
void Menu::ResetHighLight() {
	for (UINT i=0; i < m_pButton.size(); i++){
		m_pButton[i].flgHighLight = false;
	}
}

// --------------------------------------------------------------
// no�Ԗڂ̃{�^������Ԃ��i�Ȃ����ret)
// --------------------------------------------------------------
CButton* Menu::GetButtonInfo(UINT no) {
	if(no>=m_pButton.size()) return nullptr;

	CButton* Out=nullptr;

	Out = &m_pButton[no];

	return Out;
}
