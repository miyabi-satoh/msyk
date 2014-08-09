///////////////////////////////////////////////////////////////////////////////
/// @file	inifile.cpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	INIファイルの読み書きを行うクラス
///////////////////////////////////////////////////////////////////////////////

#include "inifile.hpp"
#include "stdboost.h"

void mxIniFile::write(LPCTSTR lpSection, LPCTSTR lpKeyName, int val) {
	mxString str = boost::lexical_cast<mxString>(val);
	write(lpSection, lpKeyName, str.c_str());
}



