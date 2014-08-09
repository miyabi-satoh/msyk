///////////////////////////////////////////////////////////////////////////////
/// @file	font.hpp
/// @date	2014/07/29	
/// @author	Masayuki
/// @brief	class mxFont
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_FONT_HPP_
#define MSYK_FONT_HPP_

#include "common.hpp"

///////////////////////////////////////////////////////////////////////////////
/// フォントクラス
///
class mxFont : public mxAutoClose<HFONT>
{
	typedef mxAutoClose<HFONT> BaseClass;

public:
	/// コンストラクタ
	mxFont(): BaseClass() {
	}
	/// デストラクタ
	virtual ~mxFont() {
	}

	/// フォントを作成します
	void create(
			LPCTSTR lpszFace,
			int nHeight = 0,
			int nWeight = FW_DONTCARE,
			DWORD dwCharSet = ANSI_CHARSET,
			DWORD dwOutputPrecision = OUT_DEFAULT_PRECIS,
			DWORD dwClipPrecision = CLIP_DEFAULT_PRECIS,
			DWORD dwQuality = DEFAULT_QUALITY,
			DWORD dwPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
			BOOL bItalic = FALSE,
			BOOL bUnderline = FALSE,
			BOOL bStrikeOut = FALSE,
			int nEscapement = 0,
			int nOrientation = 0,
			int nWidth = 0
			)
	{
		this->safeClose();
		m_handle = ::CreateFont(
				nHeight,
				nWidth,
				nEscapement,
				nOrientation,
				nWeight,
				bItalic,
				bUnderline,
				bStrikeOut,
				dwCharSet,
				dwOutputPrecision,
				dwClipPrecision,
				dwQuality,
				dwPitchAndFamily,
				lpszFace);
		if (m_handle == NULL) {
			throw mxWin32Exception(_T("CreateFont"));
		}
	}
};



#endif /* MSYK_FONT_HPP_ */
