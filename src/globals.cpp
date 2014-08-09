///////////////////////////////////////////////////////////////////////////////
/// @file	globals.cpp
/// @date	2014/07/27
/// @author	Masayuki
/// @brief	グローバル関数
///////////////////////////////////////////////////////////////////////////////

#include "globals.hpp"
#include "window.hpp"
#include "spi.hpp"

ATOM mxRegisterWndClass(
		LPCTSTR lpszClassName,
		HICON hIcon,
		HBRUSH hBr,
		DWORD dwExStyles,
		HICON hIconSm
		)
{
	mxZero<WNDCLASSEX> wx;

	wx->cbSize = sizeof(WNDCLASSEX);
	wx->hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wx->hIcon = (hIcon) ? hIcon : ::LoadIcon(NULL, IDI_APPLICATION);
	wx->hIconSm = (hIconSm) ? hIconSm : wx->hIcon;
	wx->hInstance = ::GetModuleHandle(NULL);
	wx->hbrBackground = (hBr) ? hBr : reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wx->lpfnWndProc = msykPrivate::mxWindowProc;
	wx->lpszClassName = lpszClassName;
	wx->style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | dwExStyles;

	ATOM atom = ::RegisterClassEx(wx.get());
	if (!atom) {
		throw mxWin32Exception(_T("RegisterClassEx"));
	}
	return atom;
}

LRESULT CALLBACK msykPrivate::mxWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		)
{
	mxWindow *pWnd = NULL;
	if (uMsg == WM_CREATE) {
		CREATESTRUCT *lpcs = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWnd = reinterpret_cast<mxWindow*>(lpcs->lpCreateParams);
		// mxWindowのインスタンスとHWNDを関連付けする
		pWnd->setHWnd(hWnd);
	}
	else {
		pWnd = mxWindow::fromHandle(hWnd);
	}

	LRESULT lRes = 0;
	try {
		if (!pWnd || !pWnd->handleMessage(&lRes, uMsg, wParam, lParam)) {
			lRes = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	catch (const mxException &e) {
		::MessageBox(hWnd, e.what(), NULL, MB_OK);
	}

	if (pWnd && uMsg == WM_DESTROY) {
		// mxWindowのインスタンスとHWNDを関連付けを解除する
		pWnd->setHWnd(NULL);
	}

	return lRes;
}

INT_PTR CALLBACK msykPrivate::mxDialogProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		)
{
	mxWindow *pWnd = NULL;
	if (uMsg == WM_INITDIALOG) {
		pWnd = reinterpret_cast<mxWindow*>(lParam);
		// mxWindowのインスタンスとHWNDを関連付けする
		pWnd->setHWnd(hWnd);
	}
	else {
		pWnd = mxWindow::fromHandle(hWnd);
	}

	LRESULT lRes = 0;
	try {
		if (!pWnd || !pWnd->handleMessage(&lRes, uMsg, wParam, lParam)) {
			return FALSE;
		}
	}
	catch (const mxException &e) {
		::MessageBox(hWnd, e.what(), NULL, MB_OK);
	}

	if (pWnd && uMsg == WM_DESTROY) {
		// mxWindowのインスタンスとHWNDを関連付けを解除する
		pWnd->setHWnd(NULL);
	}

	return SetDlgMsgResult(hWnd, uMsg, lRes);
}

UINT_PTR CALLBACK msykPrivate::mxOFNHookProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		)
{
	mxWindow *pWnd = NULL;
	if (uMsg == WM_INITDIALOG) {
		OPENFILENAME *pOfn = reinterpret_cast<OPENFILENAME*>(lParam);
		pWnd = reinterpret_cast<mxWindow*>(pOfn->lCustData);
		// mxWindowのインスタンスとHWNDを関連付けする
		pWnd->setHWnd(hWnd);
	}
	else {
		pWnd = mxWindow::fromHandle(hWnd);
	}

	LRESULT lRes = 0;
	try {
		if (!pWnd || !pWnd->handleMessage(&lRes, uMsg, wParam, lParam)) {
			return FALSE;
		}
	}
	catch (const mxException &e) {
		::MessageBox(hWnd, e.what(), NULL, MB_OK);
	}

	if (pWnd && uMsg == WM_DESTROY) {
		// mxWindowのインスタンスとHWNDを関連付けを解除する
		pWnd->setHWnd(NULL);
	}

	return SetDlgMsgResult(hWnd, uMsg, lRes);
}

int CALLBACK msykPrivate::mxBrowseCallbackProc(
		HWND hWnd,
		UINT uMsg,
		LPARAM lParam,
		LPARAM lpData
		)
{
	mxWindow *pWnd = NULL;
	if (uMsg == BFFM_INITIALIZED) {
		pWnd = reinterpret_cast<mxWindow*>(lpData);
		// mxWindowのインスタンスとHWNDを関連付けする
		pWnd->setHWnd(hWnd);
	}
	else {
		pWnd = mxWindow::fromHandle(hWnd);
	}

	LRESULT lRes = 0;
	try {
		if (!pWnd || !pWnd->handleMessage(&lRes, uMsg, 0, lParam)) {
			return 0;
		}
	}
	catch (const mxException &e) {
		::MessageBox(hWnd, e.what(), NULL, MB_OK);
	}

	return lRes;
}

int WINAPI msykPrivate::mxSpiProgressCallback(
		int nNum,
		int nDenom,
		long lData
		)
{
	mxSPI *pThis = reinterpret_cast<mxSPI*>(lData);
	return pThis->onCallback(nNum, nDenom);

}

LPITEMIDLIST mxGetItemIDListFromPath(LPCTSTR lpszPath)
{
	if (!lpszPath) {
		return NULL;
	}

	LPITEMIDLIST pidl = NULL;
	LPSHELLFOLDER pDesktopFolder;
	if (SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder))) {
		ULONG chEaten;
		ULONG dwAttributes;
		OLECHAR olePath[MAX_PATH + 1];
#ifdef UNICODE
		::_tcscpy(olePath, lpszPath);
#else
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_lpRoot, -1, olePath, MAX_PATH);
#endif
		pDesktopFolder->ParseDisplayName(
			NULL,
			NULL,
			olePath,
			&chEaten,
			&pidl,
			&dwAttributes);
		pDesktopFolder->Release();
	}

	return pidl;
}
