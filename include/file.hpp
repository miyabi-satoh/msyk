///////////////////////////////////////////////////////////////////////////////
/// @file	file.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	ファイルクラス
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_FILE_HPP_
#define MSYK_FILE_HPP_

#include "common.hpp"

///////////////////////////////////////////////////////////////////////////////
/// ファイルオープン例外クラス
///
class mxFileOpenException : public mxWin32Exception
{
public:
	/// コンストラクタ
	mxFileOpenException() : mxWin32Exception(_T("CreateFile")) {}
	/// デストラクタ
	virtual ~mxFileOpenException() {}
};

///////////////////////////////////////////////////////////////////////////////
/// ファイル読み込み例外クラス
///
class mxFileReadException : public mxWin32Exception
{
public:
	/// コンストラクタ
	mxFileReadException() : mxWin32Exception(_T("ReadFile")) {}
	/// デストラクタ
	virtual ~mxFileReadException() {}
};

///////////////////////////////////////////////////////////////////////////////
/// ファイル書き込み例外クラス
///
class mxFileWriteException : public mxWin32Exception
{
public:
	/// コンストラクタ
	mxFileWriteException() : mxWin32Exception(_T("WriteFile")) {}
	/// デストラクタ
	virtual ~mxFileWriteException() {}
};

///////////////////////////////////////////////////////////////////////////////
/// ファイルクラス
///
class mxFile : private mxAutoHandle
{
public:
	/// 動作フラグ
	enum {
		modeRead		= 0x00000001,			///< GENERIC_READ
		modeWrite		= 0x00000002,			///< GENERIC_WRITE
		modeReadWrite	= modeRead | modeWrite,	///< GENERIC_READ | GENERIC_WRITE

		shareDelete		= 0x00000010,			///< FILE_SHARE_DELETE
		shareRead		= 0x00000020,			///< FILE_SHARE_READ
		shareWrite		= 0x00000040,			///< FILE_SHARE_WRITE

		createNew		= 0x00000100,			///< CREATE_NEW
		createAlways	= 0x00000200,			///< CREATE_ALWAYS
		openExisting	= 0x00000400,			///< OPEN_EXISTING
		openAlways		= 0x00000800,			///< OPEN_ALWAYS
	};

	/// コンストラクタ
	mxFile() : mxAutoHandle() {
	}
	/// コンストラクタ
	/// @param lpszFileName ファイルパス
	/// @param nOpenFlags 動作フラグ
	mxFile(LPCTSTR lpszFileName, UINT nOpenFlags) : mxAutoHandle() {
		open(lpszFileName, nOpenFlags);
	}
	/// デストラクタ
	virtual ~mxFile() {
		close();
	}

	/// ファイルを閉じる
	void close() {
		this->safeClose();
	}

	/// バッファ内のデータをファイルに書き込み、バッファをクリアする
	/// @throw mxWin32Exception
	void flush() {
		if (!::FlushFileBuffers(*this)) {
			throw mxWin32Exception(_T("FlushFileBuffers"));
		}
	}

	/// ファイルサイズを取得する
	/// @return ファイルサイズ
	/// @retval -1 失敗
	DWORD getSize() const { return ::GetFileSize(*this, NULL); }

	/// 有効なファイルハンドルか調べる
	/// @retval true  有効
	/// @retval false 無効
	bool isValid() const {
		return m_handle && (m_handle != INVALID_HANDLE_VALUE);
	}

	/// ファイルを開く
	/// @param lpszFileName ファイルパス
	/// @param nOpenFlags 動作フラグ
	/// @throw mxFileOpenException
	void open(LPCTSTR lpszFileName, UINT nOpenFlags) {
		close();

		DWORD dwDesiredAccess = 0;
		if ((nOpenFlags & modeRead) == modeRead) { dwDesiredAccess |= GENERIC_READ; }
		if ((nOpenFlags & modeWrite) == modeWrite) { dwDesiredAccess |= GENERIC_WRITE; }

		DWORD dwShareMode = 0;
		if ((nOpenFlags & shareDelete) == shareDelete) { dwShareMode |= FILE_SHARE_DELETE; }
		if ((nOpenFlags & shareRead) == shareRead) { dwShareMode |= FILE_SHARE_READ; }
		if ((nOpenFlags & shareWrite) == shareWrite) { dwShareMode |= FILE_SHARE_WRITE; }

		DWORD dwCreationDisposition = 0;
		if ((nOpenFlags & createNew) == createNew) { dwCreationDisposition |= CREATE_NEW; }
		if ((nOpenFlags & createAlways) == createAlways) { dwCreationDisposition |= CREATE_ALWAYS; }
		if ((nOpenFlags & openExisting) == openExisting) { dwCreationDisposition |= OPEN_EXISTING; }
		if ((nOpenFlags & openAlways) == openAlways) { dwCreationDisposition |= OPEN_ALWAYS; }

		m_handle = ::CreateFile(
			lpszFileName,
			dwDesiredAccess,
			dwShareMode,
			NULL,
			dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (m_handle == INVALID_HANDLE_VALUE) {
			throw mxFileOpenException();
		}
	}


	/// ファイルからデータを読み取る
	/// @param lpBuf バッファ
	/// @param nCount 読み取るバイト数
	/// @return 実際に読み取ったバイト数
	/// @throw mxFileReadException
	UINT read(void *lpBuf, UINT nCount) {
		DWORD dwReaded = 0;
		if (!::ReadFile(*this, lpBuf, nCount, &dwReaded, NULL)) {
			throw mxFileReadException();
		}
		return dwReaded;
	}

	/// ファイルポインタを移動する
	/// @param lOff	移動するバイト数
	/// @param dwFrom 開始位置
	///				 - FILE_BEGIN : 先頭から
	///				 - FILE_CURRENT : 現在位置から
	///				 - FILE_END : 終端から
	/// @return 移動後のファイルポインタの位置
	UINT seek(LONG lOff, DWORD dwFrom = FILE_CURRENT) {
		return ::SetFilePointer(*this, lOff, NULL, dwFrom);
	}
	/// ファイルポインタを先頭に移動する
	/// @return 移動後のファイルポインタの位置
	UINT seekToBegin() { return seek(0, FILE_BEGIN); }
	/// ファイルポインタを終端に移動する
	/// @return 移動後のファイルポインタの位置
	UINT seekToEnd() { return seek(0, FILE_END); }

	/// 現在のファイルポインタの位置にEOFを設定する
	/// @throw mxWin32Exception
	void setEOF() {
		if (!::SetEndOfFile(*this)) {
			throw mxWin32Exception(_T("SetEndOfFile"));
		}
	}

	/// ファイルへデータを書き込む
	/// @param lpBuf バッファ
	/// @param nCount 書き込むバイト数
	/// @return 実際に書き込んだバイト数
	/// @throw mxFileWriteException
	UINT write(const void *lpBuf, UINT nCount) {
		DWORD dwWritten = 0;
		if (!::WriteFile(*this, lpBuf, nCount, &dwWritten, NULL)) {
			throw mxFileWriteException();
		}
		return dwWritten;
	}
};

#endif /* MSYK_FILE_HPP_ */
