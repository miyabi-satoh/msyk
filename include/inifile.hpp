///////////////////////////////////////////////////////////////////////////////
/// @file	inifile.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxIniFile
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_INIFILE_HPP_
#define MSYK_INIFILE_HPP_

#include "common.hpp"
#include "globals.hpp"

///////////////////////////////////////////////////////////////////////////////
/// INIファイルの読み書きを行うクラス
///
class mxIniFile
{
public:
	/// コンストラクタ
	/// @param lpFile INIファイル名
	/// @param lpDir ディレクトリ名(デフォルトはNULL=実行ファイルのディレクトリ)
	mxIniFile(LPCTSTR lpFile, LPCTSTR lpDir = NULL) : m_filePath()
	{
		mxString strDir;
		if (!lpDir) {
			strDir = mxGetSelfFolderPath();
		}
		else {
			strDir = lpDir;
		}

		m_filePath = mxPathCombine(strDir.c_str(), lpFile);
	}
	/// デストラクタ
	~mxIniFile() {}

	/// 文字列データの書き出し
	/// @param lpSection セクション名
	/// @param lpKeyName キー名
	/// @param val 書き出す文字列
	/// @throw mxWin32Exception
	void write(LPCTSTR lpSection, LPCTSTR lpKeyName, LPCTSTR val) {
		if (!::WritePrivateProfileString(lpSection, lpKeyName, val, m_filePath.c_str())) {
			throw mxWin32Exception(_T("WritePrivateProfileString"));
		}
	}
	/// 整数データの書き出し
	/// @param lpSection セクション名
	/// @param lpKeyName キー名
	/// @param val 書き出す数値
	/// @throw mxWin32Exception
	void write(LPCTSTR lpSection, LPCTSTR lpKeyName, int val);

	/// 文字列データの読み出し
	/// @param lpSection セクション名
	/// @param lpKeyName キー名
	/// @param defaultVal デフォルト値
	/// @return 読みだした文字列
	mxString read(LPCTSTR lpSection, LPCTSTR lpKeyName, LPCTSTR defaultVal) {
		std::vector<TCHAR> vBuffer;
		for (UINT len = 256; ; len += 256) {
			vBuffer.resize(len);
			DWORD dw = ::GetPrivateProfileString(
					lpSection,
					lpKeyName,
					defaultVal,
					&vBuffer[0],
					len,
					m_filePath.c_str());
			if (dw < len - 2) {
				break;
			}
		}
		return mxString(&vBuffer[0]);
	}
	/// 整数データの読み出し
	/// @param lpSection セクション名
	/// @param lpKeyName キー名
	/// @param defaultVal デフォルト値
	/// @return 読みだした値
	int read(LPCTSTR lpSection, LPCTSTR lpKeyName, int defaultVal) {
		return ::GetPrivateProfileInt(lpSection,lpKeyName, defaultVal, m_filePath.c_str());
	}

private:
	mxString m_filePath;	///< INIファイルパス
};

#endif /* MSYK_INIFILE_HPP_ */
