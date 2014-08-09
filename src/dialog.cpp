///////////////////////////////////////////////////////////////////////////////
/// @file	dialog.cpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	ダイアログボックスクラス
///////////////////////////////////////////////////////////////////////////////

#include "dialog.hpp"
#include "globals.hpp"

int mxDialog::doModal()
{
	m_isModal = true;
	int nRet = ::DialogBoxParam(
			::GetModuleHandle(NULL),
			 m_lpTemplate,
			 static_cast<HWND>(*m_pWndOwner),
			 msykPrivate::mxDialogProc,
			 reinterpret_cast<LPARAM>(this));
	if (nRet == -1) {
		throw mxWin32Exception(_T("DialogBoxParam"));
	}
	return nRet;
}

void mxDialog::create(LPCTSTR lpTemplate, mxWindow *pWndOwner)
{
	m_isModal = false;
	m_pWndOwner = pWndOwner;
	m_lpTemplate = lpTemplate;

	if (!::CreateDialogParam(
			::GetModuleHandle(NULL),
			 m_lpTemplate,
			 static_cast<HWND>(*m_pWndOwner),
			 msykPrivate::mxDialogProc,
			 reinterpret_cast<LPARAM>(this)))
	{
		throw mxWin32Exception(_T("CreateDialogParam"));
	}
}
