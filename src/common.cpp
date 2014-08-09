///////////////////////////////////////////////////////////////////////////////
/// @file	common.cpp
/// @date	2014/07/26
/// @author	Masayuki
/// @brief	共通で使用するマクロやクラス
///////////////////////////////////////////////////////////////////////////////

#include "common.hpp"
#include "stdboost.h"

void mxWin32Exception::init(LPCTSTR lpszApi, DWORD dwError)
{
	m_str = lpszApi;
	m_str+= _T("関数が失敗しました。\r\n");
	m_str+= _T("エラーコード：");
	m_str+= boost::lexical_cast<mxString>(dwError);
	m_str+= _T("\r\n");

	LPVOID lpMsgBuf;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
		reinterpret_cast<LPTSTR>(&lpMsgBuf),
		0,
		NULL
	);
	m_str += reinterpret_cast<LPCTSTR>(lpMsgBuf);
	::LocalFree(lpMsgBuf);
}
