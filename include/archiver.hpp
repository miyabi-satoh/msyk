///////////////////////////////////////////////////////////////////////////////
/// @file	archiver.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxArchiver
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_ARCHIVER_HPP_
#define MSYK_ARCHIVER_HPP_

#include "common.hpp"
#include "file.hpp"
#include "globals.hpp"

///////////////////////////////////////////////////////////////////////////////
/// 簡易アーカイバクラス
///
class mxArchiver
{
	static LPCWSTR Signature;	///< シグネチャー文字列

	/// アーカイブ情報
	struct ArchiveInfo
	{
		WCHAR szSignature[14];	///< シグネチャー
		UINT numFiles;			///< 格納ファイル数
	};

public:
	/// 格納ファイル情報
	struct FileInfo
	{
		UINT nSize;			///< 圧縮前サイズ
		UINT nCompSize;		///< 圧縮後サイズ
		WCHAR szName[124];	///< ファイル名
		/// @name 比較演算子
		//@{
		bool operator==(const FileInfo &rhs) const {
			return (nSize == rhs.nSize && nCompSize == rhs.nCompSize && ::wcscmp(szName, rhs.szName) == 0);
		}
		bool operator!=(const FileInfo &rhs) const {
			return (nSize != rhs.nSize || nCompSize != rhs.nCompSize || ::wcscmp(szName, rhs.szName) != 0);
		}
		//@}
	};

	/// コンストラクタ
	mxArchiver() : m_File() {
	}

	/// デストラクタ
	~mxArchiver() {
	}

	/// アーカイブを開く
	/// @param lpszFile		ファイルパス
	/// @param nOpenFlags	動作フラグ
	void open(LPCTSTR lpszFile, UINT nOpenFlags) {
		m_File.open(lpszFile, nOpenFlags);
	}

	/// アーカイブを閉じる
	void close() {
		m_File.close();
	}

	/// アーカイブの形式をチェックします
	/// @retval true  シグネチャーが一致
	/// @retval false シグネチャーが不一致
	bool checkArchive() {
		ArchiveInfo aInfo;

		m_File.seekToBegin();
		if (m_File.read(&aInfo, sizeof(ArchiveInfo)) != sizeof(ArchiveInfo)) {
			return false;
		}
		return ::wcscmp(aInfo.szSignature, Signature) == 0;
	}

	/// 格納ファイル数を取得します
	/// @return 格納ファイル数
	UINT getFileCount() {
		if (!checkArchive()) {
			return 0;
		}
		ArchiveInfo aInfo;

		m_File.seekToBegin();
		m_File.read(&aInfo, sizeof(aInfo));
		return aInfo.numFiles;
	}

	/// n番目のファイル情報を取得します
	/// @param nPos		ファイル位置(0～)
	/// @param pfInfo	ファイル情報の格納先
	/// @retval true  成功
	/// @retval false 失敗
	bool getFileInfo(UINT nPos, FileInfo *pfInfo) {
		if (nPos >= getFileCount()) {
			return false;
		}
		FileInfo fi;
		m_File.seek(sizeof(ArchiveInfo), FILE_BEGIN);
		for (UINT n = 0; n <= nPos; n++) {
			if (m_File.read(&fi, sizeof(FileInfo)) != sizeof(FileInfo)) {
				return false;
			}
			m_File.seek(fi.nCompSize);
		}
		::CopyMemory(pfInfo, &fi, sizeof(FileInfo));
		return true;
	}

	/// ファイルに展開する
	/// @param pFi			展開するファイルの情報
	/// @param lpszFileName	展開先のファイルパス
	/// @throw mxException
	void extractToFile(const FileInfo *pFi, LPCTSTR lpszFileName);

	/// メモリ上に展開する
	/// @param pFi		展開するファイルの情報
	/// @param pBuffer	展開先のバッファ
	/// @throw mxException
	void extractToMem(const FileInfo *pFi, BYTE *pBuffer) {
		// 一時ファイルに展開する
		mxString strFile = mxGetTempPath();
		extractToFile(pFi, strFile.c_str());

		// バッファへ読み込む
		mxFile oFile(strFile.c_str(), mxFile::modeRead | mxFile::shareRead | mxFile::openExisting);
		oFile.seekToBegin();
		oFile.read(pBuffer, pFi->nSize);
		oFile.close();

		// 一時ファイルを削除する
		::DeleteFile(strFile.c_str());
	}

	/// ファイルの内容を圧縮する
	/// @param lpszFileName 入力ファイルパス
	/// @throw mxException
	void compressFromFile(LPCTSTR lpszFileName) {
		mxFile inFile(lpszFileName, mxFile::modeRead | mxFile::shareRead | mxFile::openExisting);

		mxZero<FileInfo> fi;
		fi->nSize = inFile.getSize();
		::_tcscpy(fi->szName, ::PathFindFileName(lpszFileName));

		std::vector<BYTE> vBuffer(fi->nSize);
		inFile.read(&vBuffer[0], fi->nSize);
		inFile.close();

		return compressFromMem(fi.get(), &vBuffer[0]);
	}

	/// メモリ内容を圧縮する
	/// @param pFi ファイル情報
	/// @param pBuffer 入力バッファ
	/// @throw mxException
	void compressFromMem(FileInfo *pFi, BYTE *pBuffer);

private:
	mxFile m_File;	///< ファイルオブジェクト
};

#endif /* MSYK_ARCHIVER_HPP_ */
