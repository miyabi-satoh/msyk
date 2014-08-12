///////////////////////////////////////////////////////////////////////////////
/// @file	window.hpp
/// @date	2014/07/27
/// @author	Masayuki
/// @brief	class mxWindow
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_WINDOW_HPP_
#define MSYK_WINDOW_HPP_

#include "common.hpp"
#include "globals.hpp"

class mxPaintDC;

///////////////////////////////////////////////////////////////////////////////
/// ウィンドウクラス
///
class mxWindow
{
	friend LRESULT CALLBACK
	msykPrivate::mxWindowProc(HWND, UINT, WPARAM, LPARAM);

	friend INT_PTR CALLBACK
	msykPrivate::mxDialogProc(HWND, UINT, WPARAM, LPARAM);

	friend int CALLBACK
	msykPrivate::mxBrowseCallbackProc(HWND, UINT, LPARAM, LPARAM);

	friend UINT_PTR CALLBACK
	msykPrivate::mxOFNHookProc(HWND, UINT, WPARAM, LPARAM);

public:
	/// コンストラクタ
	mxWindow() {
		m_hWnd = NULL;
	}
	/// デストラクタ
	virtual ~mxWindow() {
	}

	/// HWNDからmxWindowオブジェクトを検索します
	static mxWindow* fromHandle(HWND hWnd);

	/// HWNDを取得します
	HWND getHWnd() const { return (this) ? m_hWnd : NULL; }

	////////////////////////////////////////////////////////////////////////////
	/// @name 演算子
	//@{
	/// HWNDへのキャスト
	operator HWND() const { return (this) ? m_hWnd : NULL; }
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name メニュー関係のメソッド
	//@{
	/// メニューバーを再描画します
	/// @throw mxWin32Exception
	void drawMenuBar() {
		if (!::DrawMenuBar(*this)) {
			throw mxWin32Exception(_T("DrawMenuBar"));
		}
	}
	/// ウィンドウに割り当てられているメニューのハンドルを取得します
	/// @return メニューのハンドル
	/// @retval NULL	メニューが割り当てられていない
	HMENU getMenu() const {
		return ::GetMenu(*this);
	}
	/// 新しいメニューを割り当てます
	/// @param hMenu メニューのハンドル
	/// @throw mxWin32Exception
	void setMenu(HMENU hMenu) {
		if (!::SetMenu(*this, hMenu)) {
			throw mxWin32Exception(_T("SetMenu"));
		}
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name ウィンドウ関係のメソッド
	//@{
	/// 指定されたクライアント領域を確保するために必要なウィンドウ座標を計算します
	/// @param lpRect クライアント領域の座標
	/// @throw mxWin32Exception
	void adjustWindowRect(RECT *lpRect) {
		if (!::AdjustWindowRectEx(lpRect, getStyle(), hasMenu(), getExStyle())) {
			throw mxWin32Exception(_T("AdjustWindowRectEx"));
		}
	}
	/// ウィンドウを作成します
	/// @throw mxWin32Exception
	void create(
			LPCTSTR lpClassName,			///< ウィンドウクラス名
			LPCTSTR lpWindowName = NULL,	///< ウィンドウ名
			DWORD style = 0,				///< ウィンドウスタイル
			DWORD exStyle = 0,				///< 拡張ウィンドウスタイル
			int x = CW_USEDEFAULT,			///< X座標
			int y = CW_USEDEFAULT,			///< Y座標
			int cx = CW_USEDEFAULT,			///< 幅
			int cy = CW_USEDEFAULT,			///< 高さ
			mxWindow *pWndParent = NULL,	///< 親またはオーナーウィンドウ
			HMENU hMenu = NULL				///< メニューハンドルまたは子ウィンドウID
			)
	{
		if (!::CreateWindowEx(
				exStyle,
				lpClassName,
				lpWindowName,
				(style) ? style : (WS_OVERLAPPEDWINDOW),
				x, y, cx, cy,
				static_cast<HWND>(*pWndParent),
				hMenu,
				::GetModuleHandle(NULL),
				 reinterpret_cast<LPVOID>(this)))
		{
			throw mxWin32Exception(_T("CreateWindowEx"));
		}
	}
	/// ウィンドウを破棄します
	void destroy() {
		::DestroyWindow(*this);
	}
	/// クライアント領域の左上端と右下端の座標を取得します
	/// @return クライアント領域の座標
	/// @throw mxWin32Exception
	mxRect getClientRect() const {
		mxRect rc;
		if (!::GetClientRect(*this, &rc)) {
			throw mxWin32Exception(_T("GetClientRect"));
		}
		return rc;
	}
	/// 親ウィンドウまたはオーナーウィンドウのハンドルを返します
	HWND getParent() const {
		return ::GetParent(*this);
	}
	/// ウィンドウ領域の左上端と右下端の座標を取得します
	/// @return ウィンドウ領域の座標
	/// @throw mxWin32Exception
	mxRect getRect() const {
		mxRect rc;
		if (!::GetWindowRect(*this, &rc)) {
			throw mxWin32Exception(_T("GetWindowRect"));
		}
		return rc;
	}
	/// ウィンドウの位置とサイズを変更します
	/// @param pt		位置
	/// @param sz		サイズ
	/// @param bRepaint	再描画フラグ
	/// @throw mxWin32Exception
	void move(const mxPoint &pt, const mxSize &sz, BOOL bRepaint = TRUE) {
		if (!::MoveWindow(*this, pt.x, pt.y, sz.cx, sz.cy, bRepaint)) {
			throw mxWin32Exception(_T("MoveWindow"));
		}
	}
	/// 親ウィンドウを変更します
	/// @param hWndNewParent 新しい親ウィンドウ
	/// @return 直前の親ウィンドウのハンドル
	HWND setParent(HWND hWndNewParent) {
		return ::SetParent(*this, hWndNewParent);
	}
	/// タイトルバーのテキスト(コントロールの場合は、コントロールのテキスト)を変更します
	/// @param lpString	タイトルまたはテキスト
	/// @throw mxWin32Exception
	void setText(LPCTSTR lpString) {
		if (!::SetWindowText(*this, lpString)) {
			throw mxWin32Exception(_T("SetWindowText"));
		}
	}
	/// ウィンドウの表示状態を設定します
	/// @param nCmdShow	表示状態
	/// @retval 0以外	以前から表示されていた
	/// @retval 0		以前は非表示だった
	BOOL show(int nCmdShow = SW_SHOWNORMAL) {
		return ::ShowWindow(*this, nCmdShow);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name 描画関係のメソッド
	//@{
	/// 再描画領域を追加します
	/// @param rc		長方形の座標
	/// @param bErase	TRUEを指定すると背景を消去する
	/// @throw mxWin32Exception
	void invalidateRect(const mxRect &rc, BOOL bErase = TRUE) {
		if (!::InvalidateRect(*this, &rc, bErase)) {
			throw mxWin32Exception(_T("InvalidateRect"));
		}
	}
	/// クライアント領域を更新します
	/// @throw mxWin32Exception
	void update() {
		if (!::UpdateWindow(*this)) {
			throw mxWin32Exception(_T("UpdateWindow"));
		}
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name ウィンドウクラス関係のメソッド
	//@{
	/// ウィンドウに関する情報を取得します
	/// @param nIndex	取得する値のオフセット
	/// @return 要求したデータ(32 ビット値)
	LONG getLong(int nIndex) const {
		return ::GetWindowLongA(*this, nIndex);
	}
	/// ウィンドウの属性を変更します
	/// @param nIndex		設定する値のオフセット
	/// @param dwNewLong	新しい値
	/// @return 変更前の値
	LONG setLong(int nIndex, LONG dwNewLong) {
		return ::SetWindowLongA(*this, nIndex, dwNewLong);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name メッセージ関係のメソッド
	//@{
	void postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0) {
		if(!::PostMessage(*this, uMsg, wParam, lParam)) {
			throw mxWin32Exception(_T("PostMessage"));
		}
	}
	LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0) {
		return ::SendMessage(*this, uMsg, wParam, lParam);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name ボタン関係のメソッド
	//@{
	/// ボタンコントロールのチェック状態を変更します
	/// @param nID		ボタンの識別子
	/// @param uCheck	チェックの状態
	///                  - BST_CHECKED
	///                  - BST_INDETERMINATE
	///                  - BST_UNCHECKED
	/// @throw mxWin32Exception
	void checkButton(int nID, UINT uCheck) {
		if (!::CheckDlgButton(*this, nID, uCheck)) {
			throw mxWin32Exception(_T("CheckDlgButton"));
		}
	}
	/// グループの特定のオプションボタンを選択し、その他を選択解除します
	/// @param nIDFirst	グループ内の最初のボタンの識別子
	/// @param nIDLast	グループ内の最後のボタンの識別子
	/// @param nIDCheck	選択したいボタンの識別子
	/// @throw mxWin32Exception
	void checkRadioButton(int nIDFirst, int nIDLast, int nIDCheck) {
		if (!::CheckRadioButton(*this, nIDFirst, nIDLast, nIDCheck)) {
			throw mxWin32Exception(_T("CheckRadioButton"));
		}
	}
	/// ボタンコントロールのチェック状態を取得します
	/// @param nID ボタンの識別子
	/// @return チェック状態
	UINT isButtonChecked(int nID) const {
		return ::IsDlgButtonChecked(*this, nID);
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name タイマー関係のメソッド
	//@{
	/// タイマーを破棄します
	/// @param nID	タイマーの識別子
	/// @throw mxWin32Exception
	void killTimer(UINT_PTR nID) {
		if (!::KillTimer(*this, nID)) {
			throw mxWin32Exception(_T("KillTimer"));
		}
	}
	/// タイマーを作成します
	/// @param nID		タイマーの識別子
	/// @param nElapse	タイムアウト値
	/// @return 新しいタイマの識別子
	/// @throw mxWin32Exception
	UINT_PTR setTimer(UINT_PTR nID, UINT nElapse) {
		UINT_PTR nRet = ::SetTimer(*this, nID, nElapse, NULL);
		if (nRet == 0) {
			throw mxWin32Exception(_T("SetTimer"));
		}
		return nRet;
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name ダイアログボックス関係のメソッド
	//@{
	/// コントロールのハンドルを取得します
	/// @param nID	コントロールの識別子
	/// @return コントロールのウィンドウハンドル
	/// @throw mxWin32Exception
	HWND getItem(int nID) const {
		HWND hWnd = ::GetDlgItem(*this, nID);
		if (!hWnd) {
			throw mxWin32Exception(_T("GetDlgItem"));
		}
		return hWnd;
	}
	/// コントロールのテキストを、整数値へ変換します
	/// @param nID		コントロールの識別子
	/// @param bSigned	値が符号付きか符号なしか
	/// @return コントロールテキストに相当する整数値
	/// @throw mxWin32Exception
	UINT getItemInt(int nID, BOOL bSigned = TRUE) const {
		BOOL bTranslated;
		UINT nRet = ::GetDlgItemInt(*this, nID, &bTranslated, bSigned);
		if (!bTranslated) {
			throw mxWin32Exception(_T("GetDlgItemInt"));
		}
		return nRet;
	}
	/// コントロールに関連付けられているタイトルまたはテキストを取得します
	/// @param nID	コントロールの識別子
	/// @return 取得したテキスト
	/// @throw mxWin32Exception
	mxString getItemText(int nID) const {
		HWND hWnd = getItem(nID);
		int nLen = ::GetWindowTextLength(hWnd);
		if (nLen <= 0) {
			return mxString();
		}
		std::vector<TCHAR> vBuffer(nLen + 1);
		if (!::GetWindowText(hWnd, &vBuffer[0], nLen + 1)) {
			throw mxWin32Exception(_T("GetWindowText"));
		}
		return mxString(&vBuffer[0]);
	}
	/// メッセージがダイアログボックス宛かどうかを判断し、
	/// そうである場合はそのメッセージを処理します
	/// @param lpMsg	チェックするメッセージ
	/// @retval 0以外	メッセージは処理された
	/// @retval 0		メッセージは処理されなかった
	BOOL isDialogMessage(MSG *lpMsg) const {
		return ::IsDialogMessage(*this, lpMsg);
	}
	/// メッセージボックスの作成、表示、操作を行います
	/// @return 選択されたボタンの識別子
	/// @throw mxWin32Exception
	int messageBox(
			LPCTSTR lpText,				///< メッセージボックス内のテキスト
			LPCTSTR lpCaption = NULL,	///< メッセージボックスのタイトル
			UINT uType = MB_OK			///< メッセージボックスのスタイル
			)
	{
		int nRet = ::MessageBox(*this, lpText, lpCaption, uType);
		if (!nRet) {
			throw mxWin32Exception(_T("MessageBox"));
		}
		return nRet;
	}
	/// コントロールへメッセージを送信します
	/// @return メッセージ処理の結果
	LRESULT sendItemMessage(
			int nID,			///< コントロールの識別子
			UINT uMsg,			///< メッセージ
			WPARAM wParam = 0,	///< 最初のパラメータ
			LPARAM lParam = 0	///< 2番目のパラメータ
			)
	{
		return ::SendDlgItemMessage(*this, nID, uMsg, wParam, lParam);
	}
	/// 整数値を文字列へ変換し、ダイアログボックス内のコントロールにテキストとして設定します
	/// @param nID		コントロールの識別子
	/// @param uValue	設定したい値
	/// @param bSigned	値が符号付きか符号なしか
	/// @throw mxWin32Exception
	void setItemInt(int nID, UINT uValue, BOOL bSigned = TRUE) {
		if (!::SetDlgItemInt(*this, nID, uValue, bSigned)) {
			throw mxWin32Exception(_T("GetDlgItemInt"));
		}
	}
	/// コントロールに関連付けられているタイトルまたはテキストを設定します
	/// @param nID		コントロールの識別子
	/// @param lpString	設定したいテキスト
	/// @throw mxWin32Exception
	void setItemText(int nID, LPCTSTR lpString) {
		if (!::SetDlgItemText(*this, nID, lpString)) {
			throw mxWin32Exception(_T("SetDlgItemText"));
		}
	}
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name 独自のメソッド
	//@{
	/// ウィンドウスタイルを取得します
	LONG getStyle() const {
		return getLong(GWL_STYLE);
	}
	/// ウィンドウスタイルを設定します
	LONG setStyle(LONG style) {
		return setLong(GWL_STYLE, style);
	}
	/// 拡張ウィンドウスタイルを取得します
	LONG getExStyle() const {
		return getLong(GWL_EXSTYLE);
	}
	/// 拡張ウィンドウスタイルを設定します
	LONG setExStyle(LONG style) {
		return setLong(GWL_EXSTYLE, style);
	}
	/// メニューが割り当てられているか調べます
	bool hasMenu() const {
		return ::GetMenu(*this) != NULL;
	}
	//@}
protected:
	/// ウィンドウハンドルを設定します
	void setHWnd(HWND hWnd) {
		if (hWnd) {
			m_wndMap[hWnd] = this;
		}
		else {
			m_wndMap.erase(m_hWnd);
		}
		m_hWnd = hWnd;
	}
	/// メッセージを処理します
	/// @retval true  メッセージを処理した
	/// @retval false メッセージを処理しなかった
	virtual bool handleMessage(
			LRESULT *lRes,	///< 処理結果の格納先
			UINT uMsg,		///< メッセージ
			WPARAM wParam,	///< 最初のパラメータ
			LPARAM lParam	///< 2番目のパラメータ
	);

	////////////////////////////////////////////////////////////////////////////
	/// @name メッセージハンドラ
	/// 処理した場合はtrue, デフォルトの処理を行う場合はfalseを返します
	//@{
	/// WM_CLOSEメッセージへの応答
	virtual bool onClose() {
		return false;
	}
	/// WM_COMMANDメッセージへの応答
	/// @param id			コントロールまたはメニューの識別子
	/// @param hwndCtl		コントロールのウィンドウハンドル
	/// @param codeNotify	通知コード
	virtual bool onCommand(int id, HWND hwndCtl, UINT codeNotify) {
		return false;
	}
	/// WM_CREATEメッセージへの応答
	/// @param lRes ウィンドウの作成を中止する場合は-1を設定する
	/// @param lpcs 作成されるウィンドウに関する情報
	virtual bool onCreate(LRESULT *lRes, CREATESTRUCT *lpcs) {
		return false;
	}
	/// WMDESTROYメッセージへの応答
	virtual bool onDestroy();
	/// WM_ERASEBKGNDメッセージへの応答
	virtual bool onEraseBkgnd(LRESULT *lRes, HDC hdc) {
		return false;
	}
	/// WM_LBUTTONDBLCLKおよびWM_LBUTTONDOWNメッセージへの応答
	virtual bool onLButtonDown(BOOL fDoubleClick, const mxPoint &pt, UINT keyFlags) {
		return false;
	}
	/// WM_MOUSEMOVEメッセージへの応答
	virtual bool onMouseMove(const mxPoint &pt, UINT keyFlags) {
		return false;
	}
	/// WM_PAINTメッセージへの応答
	virtual bool onPaint(mxPaintDC *pDC) {
		return false;
	}
	/// WM_SIZEメッセージへの応答
	virtual bool onSize(UINT state, const mxSize &sz) {
		return false;
	}
	/// WM_TIMERメッセージへの応答
	virtual bool onTimer(UINT id) {
		return false;
	}
	/// MM_MCINOTIFYメッセージへの応答
	virtual bool onMciNotify(UINT nFlags, MCIDEVICEID devID) {
		return false;
	}

	//@}
private:
	typedef std::map<HWND, mxWindow*> WindowMap;

	HWND m_hWnd;
	static WindowMap m_wndMap;
};

#endif /* MSYK_WINDOW_HPP_ */
