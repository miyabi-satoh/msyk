///////////////////////////////////////////////////////////////////////////////
/// @file	folderdialog.hpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	class mxFolderDialog
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_FOLDERDIALOG_HPP_
#define MSYK_FOLDERDIALOG_HPP_

#include "common.hpp"
#include "globals.hpp"
#include "dialog.hpp"

///////////////////////////////////////////////////////////////////////////////
/// フォルダ選択ダイアログボックスクラス
///
class mxFolderDialog : public mxDialog
{
public:
	/// コンストラクタ
	mxFolderDialog(
			mxWindow *pWndOwner,		///< オーナーウィンドウ
			UINT ulFlags,				///< 動作フラグ
			LPCTSTR lpszTitle = NULL,	///< タイトル
			LPCTSTR lpszDefault = NULL,	///< 初期選択フォルダ
			LPCTSTR lpszRoot = NULL		///< ルートフォルダ
			) : mxDialog()
	{
		m_pWndOwner = pWndOwner;
		::ZeroMemory(&m_bi, sizeof(BROWSEINFO));
		m_bi.ulFlags = ulFlags;
		m_bi.lpszTitle = lpszTitle;
		m_lpTemplate = lpszDefault;	// mxDialogメンバを使い回しｗ
		m_lpRoot = lpszRoot;
	}
	/// デストラクタ
	virtual ~mxFolderDialog() {
	}

	/// フォルダ選択ダイアログボックスを表示します
	/// @return IDOKまたはIDCANCEL
	virtual int 	doModal ();

	/// 選択されたフォルダパスを取得します
	LPCTSTR getSelectedPath() const { return m_selectedPath.c_str(); }

protected:
	/// @brief OKボタンを有効または無効にします
	/// @param bEnable TRUE:有効, FALSE:無効
	void enableOK(BOOL bEnable) {
		sendMessage(BFFM_ENABLEOK, 0, bEnable);
	}
	/// OKボタンのテキストを設定します
	/// @param lpString 設定するテキスト
	void setOKText(LPCTSTR lpString) {
		sendMessage(BFFM_SETOKTEXT, 0, reinterpret_cast<LPARAM>(lpString));
	}
	/// 選択項目を設定します
	/// @param lpszPath 選択するアイテムのパス
	void setSelection(LPCTSTR lpszPath) {
		sendMessage(BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(lpszPath));
	}
	/// 選択項目を設定します
	/// @param pidl 選択するアイテムのPIDL
	void setSelection(LPITEMIDLIST pidl) {
		sendMessage(BFFM_SETSELECTION, FALSE, reinterpret_cast<LPARAM>(pidl));
	}
	/// 指定フォルダを展開します
	/// @param lpszPath 展開するフォルダのパス
	void setExpanded(LPCTSTR lpszPath) {
		sendMessage(BFFM_SETEXPANDED, TRUE, reinterpret_cast<LPARAM>(lpszPath));
	}
	/// 指定フォルダを展開します
	/// @param pidl 展開するフォルダのPIDL
	void setExpanded(LPITEMIDLIST pidl) {
		sendMessage(BFFM_SETEXPANDED, FALSE, reinterpret_cast<LPARAM>(pidl));
	}
	/// ステータステキストを設定します
	/// @param lpString 設定するテキスト
	void setStatusText(LPCTSTR lpString) {
		sendMessage(BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(lpString));
	}

	virtual bool 	handleMessage (LRESULT *lRes, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/// @name メッセージハンドラ
	/// 処理した場合はtrue, デフォルトの処理を行う場合はfalseを返します
	//@{
	/// BFFM_INITIALIZEDメッセージへの応答
	virtual bool onInitialized() {
		if (m_lpTemplate) {
			setSelection(m_lpTemplate);
			setExpanded(m_lpTemplate);
		}
		return false;
	}
	/// BFFM_SELCHANGEDメッセージへの応答
	/// @param pidl 選択アイテムのPIDL
	virtual bool onSelChanged(LPITEMIDLIST pidl) {
		return false;
	}
	/// BFFM_VALIDATEFAILEDメッセージへの応答
	/// @param lpszName 不正なフォルダ名
	virtual bool onValidateFailed(LPCTSTR lpszName) {
		return false;
	}
	//@}

private:
	BROWSEINFO m_bi;			///< ダイアログ初期化情報
	LPCTSTR m_lpRoot;			///< ルートフォルダ
	mxString m_selectedPath;	///< 選択されたフォルダのパス
};

#endif /* MSYK_FOLDERDIALOG_HPP_ */
