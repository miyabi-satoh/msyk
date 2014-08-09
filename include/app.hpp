///////////////////////////////////////////////////////////////////////////////
/// @file	app.hpp
/// @date	2014/07/26
/// @author	Masayuki
/// @brief	class mxApp, getApp
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_APP_HPP_
#define MSYK_APP_HPP_

#include "common.hpp"

// 先行宣言
class mxWindow;

///////////////////////////////////////////////////////////////////////////////
/// アプリケーションクラス
///
class mxApp
{
public:
	/// コンストラクタ
	mxApp() {
		m_pMainWnd = NULL;
	}
	/// デストラクタ
	virtual ~mxApp() {
	}
	/// アプリケーションの実行(エントリポイント)
	/// @return 終了コード
	int run();
	/// メインウィンドウへのポインタを取得します
	mxWindow* getMainWnd() { return m_pMainWnd; }

protected:
	mxWindow* m_pMainWnd;	///< メインウィンドウへのポインタ

	/// 初期化処理
	virtual void initialize() {
	}
	/// 終了処理
	virtual void finalize() {
	}
	/// アイドル時の処理
	virtual void doIdle() {
		::Sleep(1);
	}
	/// アクセラレータキーやダイアログメッセージの処理
	/// @param lpMsg メッセージ情報
	/// @retval true	メッセージを処理した
	/// @retval false	メッセージを処理しなかった
	virtual bool preTranslateMessage(MSG *lpMsg) {
		return false;
	}
private:
};

/// アプリケーションで唯一のmxAppインスタンスを取得します
extern mxApp* getApp();

#endif /* MSYK_APP_HPP_ */
