///////////////////////////////////////////////////////////////////////////////
/// @file	spi.cpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	Susie Plug-inのラッパークラス
///////////////////////////////////////////////////////////////////////////////

#include "spi.hpp"

void mxSPI::load(LPCTSTR lpszPath)
{
	unload();

	m_handle = ::LoadLibrary(lpszPath);
	if (!m_handle) {
		throw mxWin32Exception(_T("LoadLibrary"));
	}

	// GetPluginInfo関数のアドレスを取得する
	m_lpGetPluginInfo = reinterpret_cast<LP_GETPLUGININFO>(
			::GetProcAddress(m_handle, "GetPluginInfo"));
	// IsSupported関数のアドレスを取得する
	m_lpIsSupported = reinterpret_cast<LP_ISSUPPORTED>(
			::GetProcAddress(m_handle, "IsSupported"));
	// GetPicture関数のアドレスを取得する
	m_lpGetPicture = reinterpret_cast<LP_GETPICTURE>(
			::GetProcAddress(m_handle, "GetPicture"));
}


