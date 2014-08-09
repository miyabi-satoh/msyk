///////////////////////////////////////////////////////////////////////////////
/// @file	static.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	class mxStatic
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_STATIC_HPP_
#define MSYK_STATIC_HPP_

#include "window.hpp"

///////////////////////////////////////////////////////////////////////////////
/// スタティックコントロールクラス
///
class mxStatic : public mxWindow
{
public:
	/// コンストラクタ
	mxStatic() : mxWindow() {}
	/// デストラクタ
	virtual ~mxStatic() {}

	/// アイコンを設定します
	/// @param hIcon アイコンのハンドル
	/// @return 以前のアイコンのハンドル
	HICON setIcon(HICON hIcon) {
		return Static_SetIcon(*this, hIcon);
	}
	/// アイコンを取得します
	HICON getIcon() const {
		return Static_GetIcon(*this, NULL);
	}
};



#endif /* MSYK_STATIC_HPP_ */
