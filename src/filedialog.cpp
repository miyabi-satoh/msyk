///////////////////////////////////////////////////////////////////////////////
/// @file	filedialog.cpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	ファイル選択ダイアログボックスクラス
///////////////////////////////////////////////////////////////////////////////

#include "filedialog.hpp"
#include "globals.hpp"
#include "stdboost.h"

int mxFileDialog::doModal()
{
//	mxString::iterator it;
//	for (it = m_strFilter.begin(); it != m_strFilter.end(); it++) {
//		if (*it == _T('|')) {
//			*it = _T('\0');
//		}
//	}

	BOOST_FOREACH(TCHAR &c, m_strFilter) {
		if (c == _T('|')) {
			c = _T('\0');
		}
	}

	m_ofn.lStructSize = sizeof(OPENFILENAME);
	m_ofn.hInstance = ::GetModuleHandle(NULL);
	m_ofn.lpstrFilter = m_strFilter.c_str();
	m_ofn.lpstrFile = &m_strResult[0];
	m_ofn.nMaxFile = m_strResult.size();
	m_ofn.lCustData = reinterpret_cast<LPARAM>(this);
	m_ofn.lpfnHook = msykPrivate::mxOFNHookProc;
	m_ofn.hwndOwner = static_cast<HWND>(*m_pWndOwner);

	BOOL bRet;
	if (m_bOpenFileDialog) {
		bRet = ::GetOpenFileName(&m_ofn);
	}
	else {
		bRet = ::GetSaveFileName(&m_ofn);
	}

	return (bRet) ? IDOK : IDCANCEL;
}



