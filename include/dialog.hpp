///////////////////////////////////////////////////////////////////////////////
/// @file	dialog.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	class mxDialog
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_DIALOG_HPP_
#define MSYK_DIALOG_HPP_

#include "window.hpp"

///////////////////////////////////////////////////////////////////////////////
/// ダイアログボックスクラス
///
class mxDialog : public mxWindow
{
public:
	/// コンストラクタ
	mxDialog()
		: mxWindow()
	{
		m_pWndOwner = NULL;
		m_lpTemplate = NULL;
		m_isModal = true;
	}
	/// コンストラクタ
	/// @param lpTemplate	リソーステンプレート名
	/// @param pWndOwner	親ウィンドウ
	mxDialog(LPCTSTR lpTemplate, mxWindow *pWndOwner)
		: mxWindow()
	{
		m_pWndOwner = pWndOwner;
		m_lpTemplate = lpTemplate;
		m_isModal = true;
	}
	/// コンストラクタ
	/// @param nIDTemplate	リソーステンプレートID
	/// @param pWndOwner	親ウィンドウ
	mxDialog(UINT nIDTemplate, mxWindow *pWndOwner)
		: mxWindow()
	{
		m_pWndOwner = pWndOwner;
		m_lpTemplate = MAKEINTRESOURCE(nIDTemplate);
		m_isModal = true;
	}
	/// デストラクタ
	virtual ~mxDialog() {
	}

	/// モーダルダイアログボックスを表示します
	/// @return EndDialog 関数の nResult パラメータの値
	/// @throw mxWin32Exception
	virtual int doModal();

	/// モーダルダイアログボックスを終了します
	/// @param nResult	返したい値
	/// @throw mxWin32Exception
	void endDialog(int nResult) {
		if (!::EndDialog(*this, nResult)) {
			throw mxWin32Exception(_T("EndDialog"));
		}
	}

	/// モードレスダイアログボックスを表示します
	/// @param lpTemplate	リソーステンプレート名
	/// @param pWndOwner	親ウィンドウ
	/// @throw mxWin32Exception
	virtual void create(LPCTSTR lpTemplate, mxWindow *pWndOwner);
	/// モードレスダイアログボックスを表示します
	/// @param nIDTemplate	リソーステンプレートID
	/// @param pWndOwner	親ウィンドウ
	/// @throw mxWin32Exception
	virtual void create(int nIDTemplate, mxWindow *pWndOwner) {
		create(MAKEINTRESOURCE(nIDTemplate), pWndOwner);
	}

protected:
	mxWindow *m_pWndOwner;	///< 親ウィンドウ
	LPCTSTR m_lpTemplate;	///< リソーステンプレート
	bool m_isModal;			///< モーダルかモードレスか

	virtual bool handleMessage(
			LRESULT *lRes, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_INITDIALOG:
			return onInitDialog(reinterpret_cast<HWND>(wParam));
		}
		return mxWindow::handleMessage(lRes, uMsg, wParam, lParam);
	}

	////////////////////////////////////////////////////////////////////////////
	/// @name メッセージハンドラ
	/// 処理した場合はtrue, デフォルトの処理を行う場合はfalseを返します
	//@{
	/// WM_INITDIALOGメッセージへの応答
	/// @param hWndFocus デフォルトでフォーカスを持つコントロール
	virtual bool onInitDialog(HWND hWndFocus) {
		return false;
	}
	virtual bool onCommand(int id, HWND hwndCtl, UINT codeNotify) {
		switch (id) {
		case IDOK:
			return onOK();

		case IDCANCEL:
			return onCancel();
		}
		return mxWindow::onCommand(id, hwndCtl, codeNotify);
	}
	/// OKボタンが押された場合の処理
	///
	/// モーダル：EndDialog(IDOK)を呼び出します @n
	/// モードレス：Destroy()を呼び出します
	virtual bool onOK() {
		if (m_isModal) {
			endDialog(IDOK);
		}
		else {
			destroy();
		}
		return true;
	}
	/// CANCELボタンが押された場合の処理
	///
	/// モーダル：EndDialog(IDCANCEL)を呼び出します @n
	/// モードレス：Destroy()を呼び出します
	virtual bool onCancel() {
		if (m_isModal) {
			endDialog(IDCANCEL);
		}
		else {
			destroy();
		}
		return true;
	}
};

#endif /* MSYK_DIALOG_HPP_ */
