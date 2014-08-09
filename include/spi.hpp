///////////////////////////////////////////////////////////////////////////////
/// @file	spi.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxSPI
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_SPI_HPP_
#define MSYK_SPI_HPP_

#include "common.hpp"
#include "globals.hpp"

typedef int (WINAPI* LP_GETPLUGININFO)(int, LPSTR, int);
typedef int (WINAPI* LP_ISSUPPORTED)(LPSTR, DWORD);
typedef int (WINAPI* LP_GETPICTURE)(LPSTR, long, unsigned int, HANDLE*, HANDLE*, FARPROC, long);

///////////////////////////////////////////////////////////////////////////////
/// Susie Plug-inのラッパークラス
///
class mxSPI : private mxAutoClose<HMODULE>
{
	friend int WINAPI
	msykPrivate::mxSpiProgressCallback(	int, int, long);

	typedef mxAutoClose<HMODULE> BaseClass;

public:
	/// コンストラクタ
	mxSPI() : BaseClass() {
		m_lpGetPluginInfo = NULL;
		m_lpIsSupported = NULL;
		m_lpGetPicture = NULL;
	}
	/// デストラクタ
	virtual ~mxSPI() {
		unload();
	}

	/// プラグインをロードします
	/// @param lpszPath プラグインのパス
	/// @throw mxWin32Exception
	void load(LPCTSTR lpszPath);
	/// プラグインをアンロードします
	void unload() {
		this->safeClose();
		m_lpGetPicture = NULL;
		m_lpGetPluginInfo = NULL;
		m_lpIsSupported = NULL;
	}

	/// @name Susie Plug-in API
	/// @see http://www2f.biglobe.ne.jp/kana/spi_api/index.html
	//@{
	/// Plug-inに関する情報を取得します
	int getPluginInfo(int infono, LPSTR buf, int buflen) {
		if (m_lpGetPluginInfo) {
			return m_lpGetPluginInfo(infono, buf, buflen);
		}
		return 0;
	}
	/// 対応しているファイル形式か調べます
	int isSupported(LPSTR filename, DWORD dw) {
		if (m_lpIsSupported) {
			return m_lpIsSupported(filename, dw);
		}
		return 0;
	}
	/// 画像を展開します
	int getPicture(
			LPSTR buf,
			long len,
			UINT flag,
			HANDLE *pHBInfo,
			HANDLE *pHBm
			)
	{
		if (m_lpGetPicture) {
			return m_lpGetPicture(buf, len, flag, pHBInfo, pHBm,
					reinterpret_cast<FARPROC>(msykPrivate::mxSpiProgressCallback),
					reinterpret_cast<long>(this));
		}
		return -1;
	}
	//@}

protected:
	virtual int onCallback(int nNum, int nDenom) { return 0; }

private:
	LP_GETPLUGININFO m_lpGetPluginInfo;	///< GetPluginInfo関数へのポインタ
	LP_ISSUPPORTED m_lpIsSupported;		///< IsSupported関数へのポインタ
	LP_GETPICTURE m_lpGetPicture;		///< GetPicture関数へのポインタ
};



#endif /* MSYK_SPI_HPP_ */
