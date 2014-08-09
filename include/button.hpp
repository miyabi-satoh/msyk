///////////////////////////////////////////////////////////////////////////////
/// @file	button.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	class mxButton
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_BUTTON_HPP_
#define MSYK_BUTTON_HPP_

#include "window.hpp"
#include <windowsx.h>

///////////////////////////////////////////////////////////////////////////////
/// ボタンクラス
///
class mxButton : public mxWindow
{
public:
	/// コンストラクタ
	mxButton() : mxWindow() {}
	/// デストラクタ
	virtual ~mxButton() {}

	/// チェック状態を取得します
	/// @return チェック状態
	LRESULT getCheck() const {
		return Button_GetCheck(*this);
	}
	/// チェック状態を設定します
	/// @param nCheck チェック状態
	void setCheck(int nCheck) {
		Button_SetCheck(*this, nCheck);
	}

	/// 状態を取得します
	/// @return 状態
	LRESULT getState() const {
		return Button_GetState(*this);
	}
	/// ハイライト状態を設定します
	/// @param bState TRUEならハイライト
	void setState(BOOL bState) {
		Button_SetState(*this, bState);
	}

	/// スタイルを設定します
	/// @param dwStyle スタイル
	/// @see http://msdn.microsoft.com/en-us/library/windows/desktop/bb775951(v=vs.85).aspx
	/// @param bRedraw 再描画フラグ
	void setStyle(DWORD dwStyle, BOOL bRedraw) {
		Button_SetStyle(*this, dwStyle, bRedraw);
	}
};

#endif /* MSYK_BUTTON_HPP_ */
