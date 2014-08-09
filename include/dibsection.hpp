///////////////////////////////////////////////////////////////////////////////
/// @file	dibsection.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxUseGdiplus, struct mxRGBA, class mxDibSection
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_DIBSECTION_HPP_
#define MSYK_DIBSECTION_HPP_

#include "common.hpp"
#include "dc.hpp"
#include <gdiplus.h>

///////////////////////////////////////////////////////////////////////////////
/// GDI+の初期化・終了を行うクラス
///
class mxUseGdiplus : public mxSingleton<mxUseGdiplus>
{
public:
	mxUseGdiplus() {
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartup(&m_token, &input, NULL);
	}
	~mxUseGdiplus() {
		Gdiplus::GdiplusShutdown(m_token);
	}

private:
	ULONG_PTR m_token;	///< トークン
};

/// GDI+を使用するためのマクロ
#define USE_GDIPLUS (mxUseGdiplus::singleton())

///////////////////////////////////////////////////////////////////////////////
/// 32bitピクセル構造体
///
struct mxRGBA
{
	BYTE b;	///< 青
	BYTE g;	///< 緑
	BYTE r;	///< 赤
	BYTE a;	///< アルファ

	/// コンストラクタ
	mxRGBA(
			BYTE red = 0,		///< 赤
			BYTE green = 0,		///< 緑
			BYTE blue = 0,		///< 青
			BYTE alpha = 255	///< アルファ
			)
	{
		b = blue;
		g = green;
		r = red;
		a = alpha;
	}
	/// デストラクタ
	~mxRGBA() {
	}

	/// @name 演算子
	//@{
	/// 比較演算子
	/// @return 等しければtrue
	bool operator==(const mxRGBA &rhs) const {
		return b == rhs.b && g == rhs.g && r == rhs.r;
	}
	/// 比較演算子
	/// @return 異なればtrue
	bool operator!=(const mxRGBA &rhs) const {
		return b != rhs.b || g != rhs.g || r != rhs.r;
	}
	/// 代入演算子
	mxRGBA& operator=(const COLORREF &rhs) {
		b = GetBValue(rhs);
		g = GetGValue(rhs);
		r = GetRValue(rhs);
		return *this;
	}
	/// 代入演算子
	mxRGBA& operator=(const RGBQUAD &rhs) {
		b = rhs.rgbBlue;
		g = rhs.rgbGreen;
		r = rhs.rgbRed;
		return *this;
	}
	/// COLORREFへのキャスト
	operator COLORREF() const { return RGB(r, g, b); }
	//@}
};

///////////////////////////////////////////////////////////////////////////////
/// DIBSectionのラッパークラス
///
class mxDibSection : public mxDC
{
public:
	/// コンストラクタ
	mxDibSection(): mxDC(), m_hBitmap() {
		::ZeroMemory(&m_bmp, sizeof(BITMAP));
		m_pBits = NULL;
	}
	/// コンストラクタ
	mxDibSection(const mxSize &sz): mxDC(), m_hBitmap() {
		::ZeroMemory(&m_bmp, sizeof(BITMAP));
		m_pBits = NULL;
		create(sz);
	}
	/// コピーコンストラクタ
	mxDibSection(const mxDibSection &rhs) : mxDC(), m_hBitmap() {
		::ZeroMemory(&m_bmp, sizeof(BITMAP));
		m_pBits = NULL;
		create(rhs.getSize());
		rhs.bitBlt(*this, getRect());
	}
	/// デストラクタ
	virtual ~mxDibSection() {
	}

	/// 指定サイズの32bitビットマップを作成します
	/// @param sz サイズ
	/// @throw mxWin32Exception
	void create(const mxSize &sz);
	/// ビットマップ情報から32bitビットマップを作成します
	/// @param pBmi		ビットマップ情報
	/// @param pData	ピクセルデータ
	/// @throw mxWin32Exception
	void create(const BITMAPINFO *pBmi, const void *pData = NULL);
	/// ファイルから32bitビットマップを作成します
	/// @param lpszFile ファイルパス
	/// @retval true	成功
	/// @retval false	失敗
	bool create(LPCTSTR lpszFile);
	/// ビットマップを破棄します
	void destroy() {
		this->safeClose();
		m_hBitmap.safeClose();
		m_pBits = NULL;
		::ZeroMemory(&m_bmp, sizeof(BITMAP));
	}

	/// 幅を取得します
	int width() const { return m_bmp.bmWidth; }
	/// 高さを取得します
	int height() const { return m_bmp.bmHeight; }
	/// ピッチ幅を取得します
	int pitch() const { return m_bmp.bmWidthBytes; }
	/// ピクセルデータへのポインタを取得します
	mxRGBA* bits() { return static_cast<mxRGBA*>(m_pBits); }
	/// ピクセルデータへのポインタを取得します
	const mxRGBA* bits() const { return static_cast<const mxRGBA*>(m_pBits); }
	/// ピクセルデータのサイズ(pitch * height)を取得します
	int getSizeImage() const { return pitch() * height(); }
	/// サイズを取得します
	mxSize getSize() const { return mxSize(width(), height()); }
	/// 長方形を取得します
	mxRect getRect() const { return mxRect(0, 0, width(), height()); }
	/// 指定座標に対応するピクセルデータのオフセットを取得します
	int getPixelPos(const mxPoint &pt) const {
		return pt.x + (((height() - 1) - pt.y) * pitch()) / sizeof(mxRGBA);
	}
	/// 指定座標のピクセルデータを取得します
	mxRGBA getPixel(const mxPoint &pt) { return bits()[getPixelPos(pt)]; }
	/// 空かどうかを判定します
	bool isEmpty() const { return width() == 0 || height() == 0; }

	/// 代入演算子
	mxDibSection& operator=(const mxDibSection &rhs) {
		this->create(rhs.getSize());
		rhs.bitBlt(*this, getRect());
		return *this;
	}

protected:
	mxAutoClose<HBITMAP> m_hBitmap;	///< ビットマップハンドル
	void	*m_pBits;				///< ピクセルデータ
	BITMAP  m_bmp;					///< ビットマップ情報
};

#endif /* MSYK_DIBSECTION_HPP_ */
