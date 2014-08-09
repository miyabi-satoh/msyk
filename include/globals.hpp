///////////////////////////////////////////////////////////////////////////////
/// @file	globals.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	グローバル関数
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_GLOBAL_HPP_
#define MSYK_GLOBAL_HPP_

#include "common.hpp"
#include <shlwapi.h>

///////////////////////////////////////////////////////////////////////////////
/// ウィンドウクラスを登録します
/// 以下のクラススタイルはデフォルトで設定されます
///  - CS_HREDRAW
///  - CS_VREDRAW
///  - CS_DBLCLKS
/// @return 登録されたクラスを一意的に識別するアトム
/// @throw mxWin32Exception
ATOM mxRegisterWndClass(
		LPCTSTR lpszClassName,	///< ウィンドウクラス名
		HICON hIcon = NULL,		///< アイコン
		HBRUSH hBr = NULL,		///< 背景ブラシ
		DWORD dwExStyles = 0,	///< 追加のクラススタイル
		HICON hIconSm = NULL	///< スモールアイコン
		);

namespace msykPrivate {
/// @name 内部使用コールバック関数
//@{
///////////////////////////////////////////////////////////////////////////////
/// ウィンドウプロシージャ
LRESULT CALLBACK mxWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		);
///////////////////////////////////////////////////////////////////////////////
/// ダイアログボックスプロシージャ
INT_PTR CALLBACK mxDialogProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
);
///////////////////////////////////////////////////////////////////////////////
/// ファイル選択ダイアログボックスのフックプロシージャ
UINT_PTR CALLBACK mxOFNHookProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		);
///////////////////////////////////////////////////////////////////////////////
/// フォルダ選択ダイアログボックスのコールバック関数
int CALLBACK mxBrowseCallbackProc(
		HWND hWnd,
		UINT uMsg,
		LPARAM lParam,
		LPARAM lpData
		);
///////////////////////////////////////////////////////////////////////////////
/// Susie Plug-in GetPictureのコールバック関数
int WINAPI mxSpiProgressCallback(
		int nNum,
		int nDenom,
		long lData
		);
//@}
};	// namespace msykPrivate

///////////////////////////////////////////////////////////////////////////////
/// @name 各種リソースのロードメソッド
//@{
/// アイコンをロードします
/// @param nID リソース識別子
/// @return アイコンのハンドル
/// @throw mxWin32Exception
inline HICON mxLoadIcon(int nID) {
	HICON hIcon = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(nID));
	if (!hIcon) {
		throw mxWin32Exception(_T("LoadIcon"));
	}
	return hIcon;
}
/// メニューをロードします
/// @param nID リソース識別子
/// @return メニューのハンドル
/// @throw mxWin32Exception
inline HMENU mxLoadMenu(int nID) {
	HMENU hMenu = ::LoadMenu(::GetModuleHandle(NULL), MAKEINTRESOURCE(nID));
	if (!hMenu) {
		throw mxWin32Exception(_T("LoadMenu"));
	}
	return hMenu;
}
//@}

///////////////////////////////////////////////////////////////////////////////
/// @name パス系メソッド
//@{
/// パス文字列を結合します
inline mxString mxPathCombine(LPCTSTR path1, LPCTSTR path2) {
	TCHAR szPath[MAX_PATH + 1];
	::PathCombine(szPath, path1, path2);
	return mxString(szPath);
}
/// 親フォルダ名を取得します
inline mxString mxPathRemoveFileSpec(LPCTSTR path) {
	TCHAR szPath[MAX_PATH + 1];
	::_tcscpy(szPath, path);
	::PathRemoveFileSpec(szPath);
	return mxString(szPath);
}

/// 自アプリケーションのフルパスを取得します
inline mxString mxGetSelfFilePath() {
	TCHAR szPath[MAX_PATH + 1];
	if (!::GetModuleFileName(NULL, szPath, MAX_PATH)) {
		throw mxWin32Exception(_T("GetModuleFileName"));
	}
	return mxString(szPath);
}
/// 自アプリケーションのフォルダパスを取得します
inline mxString mxGetSelfFolderPath() {
	mxString self = mxGetSelfFilePath();
	return mxPathRemoveFileSpec(self.c_str());
}

/// 一時ファイルのパスを取得します
/// @param lpDir 一時ディレクトリ(NULL可)
/// @param lpName 一時ファイル名(NULL可)
/// @return 一時ファイルパス
inline mxString mxGetTempPath(LPCTSTR lpDir = NULL, LPCTSTR lpName = NULL) {
	TCHAR szTempDir[MAX_PATH + 1];
	if (lpDir) {
		::_tcscpy(szTempDir, lpDir);
	}
	else {
		::GetTempPath(MAX_PATH, szTempDir);
	}

	if (lpName) {
		return mxPathCombine(szTempDir, lpName);
	}
	else {
		TCHAR szTempName[MAX_PATH];
		::GetTempFileName(szTempDir, _T("msyk"), 0, szTempName);

		return mxString(szTempName);
	}
}
/// パスからITEMIDLISTを取得します
///
/// 取得したITEMIDLISTが不要になったら、CoTaskMemFreeで解放します
LPITEMIDLIST mxGetItemIDListFromPath(LPCTSTR lpszPath);

//@}

#endif /* MSYK_GLOBAL_HPP_ */
