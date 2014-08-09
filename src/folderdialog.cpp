///////////////////////////////////////////////////////////////////////////////
/// @file	folderdialog.cpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	フォルダ選択ダイアログクラス
///////////////////////////////////////////////////////////////////////////////

#include "folderdialog.hpp"

int mxFolderDialog::doModal () {
	LPITEMIDLIST pidlRoot = mxGetItemIDListFromPath(m_lpRoot);
	m_bi.pidlRoot = pidlRoot;
	m_bi.lpfn = msykPrivate::mxBrowseCallbackProc;
	m_bi.lParam = reinterpret_cast<LPARAM>(this);
	m_bi.hwndOwner = static_cast<HWND>(*m_pWndOwner);

	int nRet = IDCANCEL;
	LPITEMIDLIST pidl = ::SHBrowseForFolder(&m_bi);
	if (pidl) {
		TCHAR szPath[MAX_PATH];
		::SHGetPathFromIDList(pidl, szPath);
		::CoTaskMemFree(pidl);

		m_selectedPath = szPath;
		nRet = IDOK;
	}

	::CoTaskMemFree(pidlRoot);

	return nRet;
}

bool mxFolderDialog::handleMessage(
		LRESULT *lRes,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		return onInitialized();

	case BFFM_SELCHANGED:
		return onSelChanged(reinterpret_cast<LPITEMIDLIST>(lParam));

	case BFFM_VALIDATEFAILED:
		return onValidateFailed(reinterpret_cast<LPCTSTR>(lParam));
	}

	return false;
}


