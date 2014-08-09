///////////////////////////////////////////////////////////////////////////////
/// @file	dc.hpp
/// @date	2014/07/27
/// @author	Masayuki
/// @brief	class mxDC, class mxWindowDC, class mxPaintDC
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_DC_HPP_
#define MSYK_DC_HPP_

#include "common.hpp"
class mxWindow;

typedef int (CALLBACK* ENUMFONTPROC)(ENUMLOGFONTEX*,NEWTEXTMETRICEX*,DWORD,LPARAM);

///////////////////////////////////////////////////////////////////////////////
/// デバイスコンテキストの実装クラス
///
class mxDC : public mxAutoClose<HDC>
{
	typedef mxAutoClose<HDC> BaseClass;

public:
	/// コンストラクタ
	mxDC() : BaseClass() {
	}
	/// デストラクタ
	virtual ~mxDC() {
	}

	////////////////////////////////////////////////////////////////////////////
	/// @name 描画関係のメソッド
	//@{
	/// 背景モードを設定します
	/// @param iBkMode 背景モード
	/// @return 以前の背景モード
	/// @throw mxWin32Exception
	int setBkMode(int iBkMode) {
		int nRet = ::SetBkMode(*this, iBkMode);
		if (!nRet) {
			throw mxWin32Exception(_T("SetBkMode"));
		}
		return nRet;
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name デバイスコンテキスト関係のメソッド
	//@{
	/// デバイスコンテキストにGDIオブジェクトを選択します
	/// @param hObj GDIオブジェクトのハンドル
	/// @return 以前のオブジェクト
	HGDIOBJ selectObject(HGDIOBJ hObj) {
		return ::SelectObject(*this, hObj);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name 図形関係のメソッド
	//@{
	/// 長方形を塗りつぶします
	/// @param rc		長方形
	/// @param hBrush	ブラシハンドル
	/// @throw mxWin32Exception
	void fillRect(const mxRect &rc, HBRUSH hBrush) {
		if (!::FillRect(*this, &rc, hBrush)) {
			throw mxWin32Exception(_T("FillRect"));
		}
	}
	/// 定義済みのブラシで長方形を塗りつぶします
	/// @param rc		長方形
	/// @param nObject	ブラシのタイプ
	/// @throw mxWin32Exception
	void fillRect(const mxRect &rc, int nObject) {
		if (!::FillRect(*this, &rc,
				reinterpret_cast<HBRUSH>(::GetStockObject(nObject))))
		{
			throw mxWin32Exception(_T("FillRect"));
		}
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name ビットマップ関係のメソッド
	//@{
	/// ビットブロック転送を行います
	void bitBlt(
			mxDC &dstDC,
			const mxRect &dstRect,
			const mxPoint &srcPoint = mxPoint(),
			DWORD dwRop = SRCCOPY) const
	{
		::BitBlt(dstDC,
				dstRect.left,
				dstRect.top,
				dstRect.width(),
				dstRect.height(),
				*this,
				srcPoint.x,
				srcPoint.y,
				dwRop);
	}
	/// ビットブロック転送(拡大/縮小)を行います
	void stretchBlt(
			mxDC &dstDC,
			const mxRect &dstRect,
			const mxRect &srcRect,
			DWORD dwRop = SRCCOPY) const
	{
		::StretchBlt(dstDC,
				dstRect.left,
				dstRect.top,
				dstRect.width(),
				dstRect.height(),
				*this,
				srcRect.left,
				srcRect.top,
				srcRect.width(),
				srcRect.height(),
				dwRop);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name フォント・テキスト関係のメソッド
	//@{
	/// 長方形領域に合わせて整形したテキストを描画します
	/// @param lpString	描画するテキスト
	/// @param lpRect	テキストを描画する長方形領域
	/// @param uFormat	テキスト描画オプション
	/// @param nCount	テキストの長さ(-1を指定すると自動計算)
	int drawText(LPCTSTR lpString, LPRECT lpRect, UINT uFormat, int nCount = -1) {
		return ::DrawText(*this, lpString, nCount, lpRect, uFormat);
	}
	/// フォントを列挙します
	int enumFontFamilies(
			LOGFONT *lpLogfont,				///< フォント情報
			ENUMFONTPROC lpEnumFontProc,	///< コールバック関数
			LPARAM lParam					///< 追加データ
			)
	{
		return ::EnumFontFamiliesEx(
				*this,
				lpLogfont,
				reinterpret_cast<FONTENUMPROC>(lpEnumFontProc),
				lParam,
				0);
	}
	/// テキストの色を設定します
	/// @param crColor	テキストの色
	COLORREF setTextColor(COLORREF crColor) {
		return ::SetTextColor(*this, crColor);
	}
	/// テキストを描画します
	/// @param pt		描画開始位置
	/// @param lpString	描画するテキスト
	/// @param nCount	テキストの長さ(-1を指定すると自動計算)
	/// @throw mxWin32Excepiton
	void textOut(const mxPoint &pt, LPCTSTR lpString, int nCount = -1) {
		if (nCount == -1) {
			nCount = _tcslen(lpString);
		}
		if (!::TextOut(*this, pt.x, pt.y, lpString, nCount)) {
			throw mxWin32Exception(_T("TextOut"));
		}
	}
	//@}
};

///////////////////////////////////////////////////////////////////////////////
/// GetDC/ReleaseDCのラッパークラス
///
class mxWindowDC : public mxDC
{
public:
	/// コンストラクタ
	/// @param pWnd ウィンドウオブジェクト
	mxWindowDC(mxWindow *pWnd = NULL);
	/// デストラクタ
	virtual ~mxWindowDC() {
		::ReleaseDC(m_hWnd, m_handle);
		m_handle = NULL;
	}

private:
	HWND m_hWnd;
};

///////////////////////////////////////////////////////////////////////////////
/// BeginPaint/EndPaintのラッパークラス
///
class mxPaintDC : public mxDC
{
public:
	/// コンストラクタ
	/// @param pWnd ウィンドウオブジェクト
	mxPaintDC(mxWindow *pWnd);
	/// デストラクタ
	virtual ~mxPaintDC() {
		::EndPaint(m_hWnd, &m_ps);
		m_handle = NULL;
	}
	/// 再描画領域を取得します
	mxRect getPaintRect() const {
		return mxRect(m_ps.rcPaint);
	}

private:
	HWND m_hWnd;		///< ウィンドウハンドル
	PAINTSTRUCT m_ps;	///< 描画情報
};

#endif /* MSYK_DC_HPP_ */
