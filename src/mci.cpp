///////////////////////////////////////////////////////////////////////////////
/// @file	mci.cpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	MCIラッパークラス
///////////////////////////////////////////////////////////////////////////////

#include "mci.hpp"
#include "stdboost.h"

void mxMCI::play(LPCTSTR szFilePath) {
	// GetTickCount()を使ってユニークなエイリアス名を生成する
	mxString szAlias = boost::lexical_cast<mxString>(::GetTickCount());

	// ファイルオープン
	MCIERROR mciError;
	mxString strCmd;
	strCmd = _T("open \"");
	strCmd += szFilePath;
	strCmd += _T("\" alias ");
	strCmd += szAlias;
	mciError = ::mciSendString(strCmd.c_str(), NULL, 0, NULL);
	if (mciError != 0) {
		throw mxMCIException(strCmd.c_str(), mciError);
	}

	m_DeviceId = ::mciGetDeviceID(szAlias.c_str());

	// 再生
	MCI_OPEN_PARMS mciOpen;
	mciError = ::mciSendCommand(m_DeviceId, MCI_PLAY, 0, reinterpret_cast<DWORD>(&mciOpen));
	if (mciError != 0) {
		close();
		throw mxMCIException(_T("MCI_PLAY"), mciError);
	}
}

void mxMCI::play(TCHAR cDrive, UINT nTrackNo) {
	// ドライブレターを設定する
	TCHAR szDrive[4];
	szDrive[0] = cDrive;
	szDrive[1] = ':';
	szDrive[2] = '\0';

	MCIERROR mciError;
	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrDeviceType = reinterpret_cast<LPCTSTR>(MCI_DEVTYPE_CD_AUDIO);
	mciOpen.lpstrElementName = szDrive;
	mciError = ::mciSendCommand(
		0,
		MCI_OPEN,
		MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT | MCI_OPEN_SHAREABLE | MCI_WAIT,
		reinterpret_cast<DWORD>(&mciOpen));
	if (mciError != 0) {
		throw mxMCIException(_T("MCI_OPEN"), mciError);
	}

	m_DeviceId = mciOpen.wDeviceID;

	try {
		// トラック数の取得
		MCI_STATUS_PARMS mciStatus;
		mciStatus.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
		mciError = ::mciSendCommand(
			m_DeviceId,
			MCI_STATUS,
			MCI_STATUS_ITEM,
			reinterpret_cast<DWORD>(&mciStatus));
		if (mciError != 0) {
			throw mxMCIException(_T("MCI_STATUS"), mciError);
		}

		DWORD dwTrackCount = mciStatus.dwReturn;

		// 時間フォーマットの設定
		MCI_SET_PARMS mciSet;
		mciSet.dwTimeFormat = MCI_FORMAT_TMSF;
		mciError = ::mciSendCommand(
			m_DeviceId,
			MCI_SET,
			MCI_WAIT | MCI_SET_TIME_FORMAT,
			reinterpret_cast<DWORD>(&mciSet));
		if (mciError != 0) {
			throw mxMCIException(_T("MCI_SET"), mciError);
		}

		// 指定トラックの再生
		MCI_PLAY_PARMS mciPlay;
		mciPlay.dwFrom = MCI_MAKE_TMSF(nTrackNo, 0, 0, 0);
		DWORD dwTo = 0;
		if (nTrackNo != dwTrackCount) {
			mciPlay.dwTo = MCI_MAKE_TMSF(nTrackNo + 1, 0, 0, 0);
			dwTo = MCI_TO;
		}
		mciError = ::mciSendCommand(
			m_DeviceId,
			MCI_PLAY,
			MCI_NOTIFY | MCI_FROM | dwTo,
			reinterpret_cast<DWORD>(&mciPlay));
		if (mciError != 0) {
			throw mxMCIException(_T("MCI_PLAY"), mciError);
		}
	}
	catch (...) {
		close();
		throw;
	}
}


