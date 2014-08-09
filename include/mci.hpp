///////////////////////////////////////////////////////////////////////////////
/// @file	mci.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	class mxMCI
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_MCI_HPP_
#define MSYK_MCI_HPP_

#include "common.hpp"

///////////////////////////////////////////////////////////////////////////////
/// MCI例外クラス
///
class mxMCIException : public mxException
{
public:
	/// コンストラクタ
	/// @param lpszCmd	MCIコマンド名
	/// @param e		MCIエラーコード
	mxMCIException(LPCTSTR lpszCmd, MCIERROR e) {
		TCHAR buf[256];
		::mciGetErrorString(e, buf, 256);

		m_str = lpszCmd;
		m_str+= _T("が失敗。\r\n");
		m_str+= buf;
	}
	/// デストラクタ
	~mxMCIException() {
	}
};

///////////////////////////////////////////////////////////////////////////////
/// MCIラッパークラス
///
class mxMCI
{
public:
	/// コンストラクタ
	mxMCI() {
		m_DeviceId = INVALIDE_DEVICE_ID;
	}

	/// デストラクタ
	~mxMCI() {
		stop();
	}

	/// デバイスIDを取得します
	MCIDEVICEID getDeviceId() const { return m_DeviceId; }

	/// ファイルの再生
	/// @param szFilePath ファイルパス
	/// @throw mxMCIExceptiion
	void play(LPCTSTR szFilePath);

	/// CDトラックの再生
	/// @param cDrive ドライブ番号('Q'等)
	/// @param nTrackNo トラック番号
	/// @throw mxMCIException
	void play(TCHAR cDrive, UINT nTrackNo);

	/// 停止(デバイスも閉じる)
	void stop() {
		if (m_DeviceId != INVALIDE_DEVICE_ID) {
			::mciSendCommand(m_DeviceId, MCI_STOP, 0, 0);
		}
		close();
	}

	/// 各種状態の取得
	/// @param dwItem 取得する情報の種類
	/// @return 取得結果
	/// @throw mxMCIException
	DWORD getStatus(DWORD dwItem) {
		MCI_STATUS_PARMS mciStatus;
		mciStatus.dwItem = dwItem;

		MCIERROR mciError = ::mciSendCommand(
			m_DeviceId,
			MCI_STATUS,
			MCI_STATUS_ITEM,
			reinterpret_cast<DWORD>(&mciStatus));
		if (mciError != 0) {
			throw mxMCIException(_T("MCI_STATUS"), mciError);
		}
		return mciStatus.dwReturn;
	}

private:
	MCIDEVICEID m_DeviceId;	///< デバイスID
	static const MCIDEVICEID INVALIDE_DEVICE_ID = -1;	///< 未初期化のデバイスID

	/// デバイスを閉じる
	void close() {
		if (m_DeviceId != INVALIDE_DEVICE_ID) {
			::mciSendCommand(m_DeviceId, MCI_CLOSE, 0, 0);
			m_DeviceId = INVALIDE_DEVICE_ID;
		}
	}
};

#endif /* MSYK_MCI_HPP_ */
