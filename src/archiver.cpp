///////////////////////////////////////////////////////////////////////////////
/// @file	archiver.cpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	簡易アーカイバクラス
///////////////////////////////////////////////////////////////////////////////

#include "archiver.hpp"
#include <zlib.h>
LPCWSTR mxArchiver::Signature = L"MsykArc";

void mxArchiver::extractToFile(const FileInfo *pFi, LPCTSTR lpszFileName)
{
	FileInfo fi;
	m_File.seek(sizeof(ArchiveInfo), FILE_BEGIN);
	for (UINT n = 0; ; n++) {
		if (m_File.read(&fi, sizeof(FileInfo)) != sizeof(FileInfo)) {
			throw mxException(_T("ファイル読み込み中にエラー(1)が発生しました。"));
		}
		if (*pFi == fi) {
			break;
		}
		m_File.seek(fi.nCompSize);
	}

	std::vector<BYTE> vBuffer(fi.nCompSize);
	if (m_File.read(&vBuffer[0], fi.nCompSize) != fi.nCompSize) {
		throw mxException(_T("ファイル読み込み中にエラー(2)が発生しました。"));
	}

	// データを展開する
	const int BUF_SIZE = 256;
	BYTE oBuf[BUF_SIZE];

	z_stream z;
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	if (inflateInit(&z) != Z_OK) {
		throw mxException(_T("展開処理中にエラーが発生しました。"));
	}

	z.next_in = &vBuffer[0];
	z.avail_in = fi.nCompSize;
	z.next_out = oBuf;
	z.avail_out = BUF_SIZE;

	mxFile oFile(
			lpszFileName,
			mxFile::modeReadWrite | mxFile::shareRead | mxFile::createAlways);

	while (1) {
		int status = inflate(&z, Z_NO_FLUSH);
		if (status == Z_STREAM_END) {
			break;
		}
		if (status != Z_OK) {
			throw mxException(_T("展開処理中にエラーが発生しました。"));
		}
		if (z.avail_out == 0) {
			if (oFile.write(oBuf, BUF_SIZE) != BUF_SIZE) {
				throw mxException(_T("ファイル書き込み中にエラーが発生しました。"));
			}
			z.next_out = oBuf;
			z.avail_out = BUF_SIZE;
		}
	}

	UINT nAvail = BUF_SIZE - z.avail_out;
	if (nAvail > 0) {
		if (oFile.write(oBuf, nAvail) != nAvail) {
			throw mxException(_T("ファイル書き込み中にエラーが発生しました。"));
		}
	}

	if (inflateEnd(&z) != Z_OK) {
		throw mxException(_T("展開処理中にエラーが発生しました。"));
	}

	oFile.close();
}

void mxArchiver::compressFromMem(FileInfo *pFi, BYTE *pBuffer)
{
	if (!checkArchive()) {
		mxZero<ArchiveInfo> aInfo;
		_tcscpy(aInfo->szSignature, Signature);

		m_File.seekToBegin();
		m_File.write(aInfo.get(), sizeof(ArchiveInfo));
	}
	m_File.seekToEnd();

	//  ファイル情報を書き込む
	m_File.write(pFi, sizeof(FileInfo));

	// データを圧縮して書き込む
	const int BUF_SIZE = 256;
	BYTE oBuf[BUF_SIZE];
	UINT nCompSize = 0;

	z_stream z;
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	if (deflateInit(&z, 1) != Z_OK) {
		throw mxException(_T("圧縮処理中にエラーが発生しました。"));
	}

	z.next_in = pBuffer;
	z.avail_in = pFi->nSize;
	z.next_out = oBuf;
	z.avail_out = BUF_SIZE;

	while (1) {
		int status = deflate(&z, Z_FINISH);
		if (status == Z_STREAM_END) {
			break;
		}
		if (status != Z_OK) {
			throw mxException(_T("圧縮処理中にエラーが発生しました。"));
		}
		if (z.avail_out == 0) {
			if (m_File.write(oBuf, BUF_SIZE) != BUF_SIZE) {
				throw mxException(_T("ファイル書き込み中にエラーが発生しました。"));
			}
			m_File.flush();
			nCompSize += BUF_SIZE;
			z.next_out = oBuf;
			z.avail_out = BUF_SIZE;
		}
	}

	UINT nAvail = BUF_SIZE - z.avail_out;
	if (nAvail > 0) {
		if (m_File.write(oBuf, nAvail) != nAvail) {
			throw mxException(_T("ファイル書き込み中にエラーが発生しました。"));
		}
		m_File.flush();
		nCompSize += nAvail;
	}

	if(deflateEnd(&z) != Z_OK) {
		throw mxException(_T("圧縮処理中にエラーが発生しました。"));
	}

	// 圧縮後のサイズを書き込む
	pFi->nCompSize = nCompSize;
	m_File.setEOF();
	m_File.seek(-static_cast<int>(sizeof(FileInfo) + nCompSize));
	m_File.write(pFi, sizeof(FileInfo));

	// 格納ファイル数を+1する
	ArchiveInfo aInfo;
	m_File.seekToBegin();
	m_File.read(&aInfo, sizeof(ArchiveInfo));
	aInfo.numFiles++;
	m_File.seekToBegin();
	m_File.write(&aInfo, sizeof(ArchiveInfo));
}
