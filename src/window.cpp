///////////////////////////////////////////////////////////////////////////////
/// @file	window.cpp
/// @date	2014/07/27
/// @author	Masayuki
/// @brief	ウィンドウクラス
///////////////////////////////////////////////////////////////////////////////

#include "common.hpp"
#include "window.hpp"
#include "app.hpp"
#include "dc.hpp"

mxWindow::WindowMap mxWindow::m_wndMap;

mxWindow* mxWindow::fromHandle(HWND hWnd)
{
	if (m_wndMap.find(hWnd) != m_wndMap.end()) {
		return m_wndMap[hWnd];
	}
	return NULL;
}

bool mxWindow::handleMessage(
		LRESULT *lRes,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		)
{
	switch (uMsg) {
	case MM_MCINOTIFY:
		return onMciNotify(wParam, lParam);

	case WM_CLOSE:
		return onClose();

	case WM_COMMAND:
		return onCommand(
				LOWORD(wParam),
				reinterpret_cast<HWND>(lParam),
				HIWORD(wParam));

	case WM_CREATE:
		return onCreate(lRes, reinterpret_cast<CREATESTRUCT*>(lParam));

	case WM_DESTROY:
		return onDestroy();

	case WM_ERASEBKGND:
		return onEraseBkgnd(lRes, reinterpret_cast<HDC>(wParam));

	case WM_LBUTTONDBLCLK:
		return onLButtonDown(TRUE, mxPoint(lParam), wParam);

	case WM_LBUTTONDOWN:
		return onLButtonDown(FALSE, mxPoint(lParam), wParam);

	case WM_MOUSEMOVE:
		return onMouseMove(mxPoint(lParam), wParam);

	case WM_PAINT: {
		mxPaintDC dc(this);
		return onPaint(&dc); }

	case WM_SIZE:
		return onSize(wParam, mxSize(lParam));

	case WM_TIMER:
		return onTimer(wParam);

	}
	return false;
}

bool mxWindow::onDestroy()
{
	if (this == getApp()->getMainWnd()) {
		::PostQuitMessage(0);
		return true;
	}
	return false;
}
