///////////////////////////////////////////////////////////////////////////////
/// @file	dibsection.cpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	DIBSectionのラッパークラス
///////////////////////////////////////////////////////////////////////////////

#include "dibsection.hpp"

void mxDibSection::create(const mxSize &sz)
{
	destroy();

	mxZero<BITMAPINFO> bmi;
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = sz.cx;
	bmi->bmiHeader.biHeight = sz.cy;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biSizeImage = sz.cy * GetPitch(sz.cx, 32);

	m_hBitmap = ::CreateDIBSection(
		NULL,
		bmi.get(),
		DIB_RGB_COLORS,
		&m_pBits,
		NULL,
		0);
	if (!m_hBitmap) {
		throw mxWin32Exception(_T("CreateDIBSection"));
	}

	m_handle = ::CreateCompatibleDC(NULL);
	selectObject(m_hBitmap);
	::GetObject(m_hBitmap, sizeof(BITMAP), &m_bmp);
	::ZeroMemory(m_pBits, height() * pitch());
}

void mxDibSection::create(const BITMAPINFO *pBmi, const void *pData)
{
	create(mxSize(pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight));

	if (pData) {
		// ソースとなるHBITMAPを作成する
		void *pBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(
			NULL,
			pBmi,
			DIB_RGB_COLORS,
			&pBits,
			NULL,
			0);
		if (!hBitmap) {
			destroy();
			throw mxWin32Exception(_T("CreateDIBSection"));
		}
		::CopyMemory(pBits, pData, pBmi->bmiHeader.biSizeImage);

		HDC hdc = ::CreateCompatibleDC(NULL);
		::SelectObject(hdc, hBitmap);
		::BitBlt(*this, 0, 0, width(), height(), hdc, 0, 0, SRCCOPY);
		::DeleteDC(hdc);
		::DeleteObject(hBitmap);
	}
}

bool mxDibSection::create(LPCTSTR lpszFile)
{
	std::auto_ptr<Gdiplus::Bitmap> pBitmap;

	pBitmap.reset(Gdiplus::Bitmap::FromFile(lpszFile));
	if (pBitmap->GetLastStatus() != Gdiplus::Ok) {
		return false;
	}

	// 同じ大きさのDIBSectionを作成する
	create(mxSize(pBitmap->GetWidth(), pBitmap->GetHeight()));

	// HBITMAPを取得してコピーする
	HBITMAP hBitmap =NULL;
	pBitmap->GetHBITMAP(0, &hBitmap);

	HDC hdc = ::CreateCompatibleDC(NULL);
	::SelectObject(hdc, hBitmap);
	::BitBlt(*this, 0, 0, width(), height(), hdc, 0, 0, SRCCOPY);
	::DeleteDC(hdc);
	::DeleteObject(hBitmap);

	return true;
}
