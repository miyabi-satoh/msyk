///////////////////////////////////////////////////////////////////////////////
/// @file	findfile.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxFindFile
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_FINDFILE_HPP_
#define MSYK_FINDFILE_HPP_

#include "common.hpp"
#include "globals.hpp"

///////////////////////////////////////////////////////////////////////////////
/// FindFirstFile/FindNextFileのラッパークラス
///
class mxFindFile : private mxAutoFind
{
public:
	mxFindFile(LPCTSTR lpszName) : mxAutoFind(), m_strFileName(lpszName) {
		::ZeroMemory(&m_wfd, sizeof(WIN32_FIND_DATA));
	}
	~mxFindFile() {
	}

	bool next() {
		if (m_handle == NULL) {
			m_handle = ::FindFirstFile(m_strFileName.c_str(), &m_wfd);
		}
		else if (m_handle != INVALID_HANDLE_VALUE) {
			return ::FindNextFile(m_handle, &m_wfd);
		}
		return m_handle != INVALID_HANDLE_VALUE;
	}

	mxString getFileName() const { return mxString(m_wfd.cFileName); }
	mxString getFilePath() const {
		mxString strFolder = mxPathRemoveFileSpec(m_strFileName.c_str());
		return mxPathCombine(strFolder.c_str(), m_wfd.cFileName);
	}

private:
	mxString m_strFileName;	///< 検索ファイル名
	WIN32_FIND_DATA m_wfd;	///< 検索結果
};

#endif /* MSYK_FINDFILE_HPP_ */
