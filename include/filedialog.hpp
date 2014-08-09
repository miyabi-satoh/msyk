///////////////////////////////////////////////////////////////////////////////
/// @file	filedialog.hpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	class mxFileDialog
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_FILEDIALOG_HPP_
#define MSYK_FILEDIALOG_HPP_

#include "common.hpp"
#include "dialog.hpp"

///////////////////////////////////////////////////////////////////////////////
/// ファイル選択ダイアログボックスクラス
///
class mxFileDialog : public mxDialog
{
public:
	/// コンストラクタ
	mxFileDialog(
		bool bOpenFileDialog,		///< 開く場合はtrue、保存する場合はfalse
		DWORD dwFlags,				///< 動作フラグ
		LPCTSTR lpszFilter = NULL,	///< フィルタ文字列(ex. "Text|*.txt||")
		mxWindow* pWndOwner = NULL,	///< オーナーウィンドウ
		LPCTSTR lpszDefExt = NULL,	///< デフォルトの拡張子
		LPCTSTR lpszFileName = NULL	///< 初期ファイル名
	): mxDialog(), m_strFilter(), m_strResult(4096, _T('\0'))
	{
		m_isModal = true;
		m_pWndOwner = pWndOwner;

		m_bOpenFileDialog = bOpenFileDialog;
		m_strFilter = (lpszFilter) ? lpszFilter : _T("すべてのファイル(*.*)|*.*||");

		::ZeroMemory(&m_ofn, sizeof(OPENFILENAME));
		m_ofn.Flags = dwFlags | OFN_EXPLORER;
		m_ofn.lpstrDefExt = lpszDefExt;
		if (lpszFileName) {
			::_tcscpy(m_ofn.lpstrFile, lpszFileName);
		}
	}
	/// デストラクタ
	virtual ~mxFileDialog() {
	}

	/// ファイル選択ダイアログボックスを表示します
	/// @return IDOKまたはIDCANCEL
	virtual int 	doModal ();

	/// 選択されたファイルのパスを取得します
	mxString getPathName() const { return mxString(&m_strResult[0]); }

	/// 複数選択されたファイルのn番目のパスを取得します
	/// @retval 空文字列 nが無効な番号
	mxString getPathName(int n) const {
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) != OFN_ALLOWMULTISELECT) { // 単一選択
			return (n == 0) ? getPathName() : mxString();
		}
		else {	// 複数選択
			//"Dir\file1
			// または
			//"Dir\0file1\0file2\0...\0fileX\0\0"

			// ディレクトリ名を取得しておく
			LPCTSTR lpDir = &m_strResult[0];
			// n番目のファイル名まで移動する
			LPCTSTR lp = &m_strResult[0];
			for (int pos = 0; pos <= n; pos++) {
				while (*lp != _T('\0')) {
					lp++;
				}
				lp++;
			}
			if (*lp == _T('\0')) {
				return (n == 0) ? mxString(lpDir) : mxString();
			}
			return mxPathCombine(lpDir, lp);
		}
	}

protected:
	bool m_bOpenFileDialog;			///< 開く or 保存
	OPENFILENAME m_ofn;				///< ダイアログパラメータ
	mxString m_strFilter;			///< フィルタ文字列
	std::vector<TCHAR> m_strResult;	///< 選択結果
};

#endif /* MSYK_FILEDIALOG_HPP_ */
