///////////////////////////////////////////////////////////////////////////////
/// @file	dc.cpp
/// @date	2014/07/27
/// @author	Masayuki
/// @brief	デバイスコンテキストクラス
///////////////////////////////////////////////////////////////////////////////

#include "dc.hpp"
#include "window.hpp"

mxWindowDC::mxWindowDC(mxWindow *pWnd)
	: mxDC()
{
	m_hWnd = static_cast<HWND>(*pWnd);
	m_handle = ::GetDC(m_hWnd);
}

mxPaintDC::mxPaintDC(mxWindow *pWnd)
	: mxDC()
{
	m_hWnd = static_cast<HWND>(*pWnd);
	m_handle = ::BeginPaint(m_hWnd, &m_ps);
}


