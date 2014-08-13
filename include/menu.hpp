///////////////////////////////////////////////////////////////////////////////
/// @file	menu.hpp
/// @date	2014/07/27	
/// @author	Masayuki
/// @brief	class mxMenu
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_MENU_HPP_
#define MSYK_MENU_HPP_

#include "common.hpp"

///////////////////////////////////////////////////////////////////////////////
/// メニュークラス
///
class mxMenu
{
public:
	/// コンストラクタ
	mxMenu() {
		m_hMenu = NULL;
	}
	/// コンストラクタ
	mxMenu(HMENU hMenu) {
		m_hMenu = hMenu;
	}
	/// デストラクタ
	virtual ~mxMenu() {
	}

	/// HMENUへのキャスト
	operator HMENU() const { return m_hMenu; }

	/// グループ内の1つのメニュー項目をチェックし、その他のチェックを外します
	/// @param idFirst	最初の項目の位置または識別子
	/// @param idLast	最後の項目の位置または識別子
	/// @param idCheck	チェックする項目の位置または識別子
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @throw mxWin32Exception
	void checkRadioItem(UINT idFirst, UINT idLast, UINT idCheck, UINT nPosOrCmd = MF_BYCOMMAND)
	{
		if (!::CheckMenuRadioItem(*this, idFirst, idLast, idCheck, nPosOrCmd)) {
			throw mxWin32Exception(_T("CheckMenuRadioItem"));
		}
	}
	/// メニュー項目を削除します
	/// @param nItem		項目の位置または識別子
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @throw mxWin32Exception
	void deleteItem(UINT nItem, UINT nPosOrCmd = MF_BYCOMMAND) {
		if (!::DeleteMenu(*this, nItem, nPosOrCmd)) {
			throw mxWin32Exception(_T("DeleteMenu"));
		}
	}
	/// メニュー項目を有効化、無効化、または淡色表示にします
	/// @param nItem		項目の位置または識別子
	/// @param nEnable		MF_ENABLE / MF_DISABLED / MF_GRAYED
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @return メニュー項目の以前の状態
	/// @throw mxWin32Exception
	int enableItem(UINT nItem, UINT nEnable, UINT nPosOrCmd = MF_BYCOMMAND)
	{
		int nRet = ::EnableMenuItem(*this, nItem, nEnable | nPosOrCmd);
		if (nRet == - 1) {
			throw mxWin32Exception(_T("EnableMenuItem"));
		}
		return nRet;
	}
	/// メニューの項目数を調べます
	/// @throw mxWin32Exception
	int getItemCount() const {
		int nRet = ::GetMenuItemCount(*this);
		if (nRet == - 1) {
			throw mxWin32Exception(_T("GetMenuItemCount"));
		}
		return nRet;
	}
	/// メニュー項目に関する情報を取得します
	/// @param lpmii		メニュー項目の情報
	/// @param nItem		項目の位置または識別子
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @throw mxWin32Exception
	void getItemInfo(MENUITEMINFO *lpmii, UINT nItem, UINT nPosOrCmd = MF_BYCOMMAND) {
		if (!::GetMenuItemInfo(*this, nItem,
				(nPosOrCmd == MF_BYPOSITION) ? TRUE : FALSE, lpmii))
		{
			mxTrace(_T("nItem=") << nItem);
			throw mxWin32Exception(_T("GetMenuItemInfo"));
		}
	}
	/// サブメニューのハンドルを取得します
	/// @param nPos	メニュー項目の位置
	HMENU getSubMenu(int nPos) {
		return ::GetSubMenu(*this, nPos);
	}
	/// 新しいメニュー項目を挿入します
	/// @param nItem		項目の位置または識別子
	/// @param lpmii		メニュー項目の情報
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @throw mxWin32Exception
	void insertItem(UINT nItem, MENUITEMINFO *lpmii, UINT nPosOrCmd = MF_BYCOMMAND)
	{
		if (!::InsertMenuItem(*this, nItem,
				(nPosOrCmd == MF_BYPOSITION) ? TRUE : FALSE, lpmii))
		{
			throw mxWin32Exception(_T("InsertMenuItem"));
		}
	}
	/// メニュー項目に関する情報を変更します
	/// @param lpmii		メニュー項目の情報
	/// @param nItem		項目の位置または識別子
	/// @param nPosOrCmd	MF_BYPOSITION / MF_BYCOMMAND
	/// @throw mxWin32Exception
	void setItemInfo(MENUITEMINFO *lpmii, UINT nItem, UINT nPosOrCmd = MF_BYCOMMAND) {
		if (!::SetMenuItemInfo(*this, nItem,
				(nPosOrCmd == MF_BYPOSITION) ? TRUE : FALSE, lpmii))
		{
			throw mxWin32Exception(_T("GetMenuItemInfo"));
		}
	}
private:
	HMENU m_hMenu;
};

#endif /* MSYK_MENU_HPP_ */
