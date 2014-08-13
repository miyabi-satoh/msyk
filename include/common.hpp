///////////////////////////////////////////////////////////////////////////////
/// @file	common.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	共通で使用するマクロやクラス
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_COMMON_HPP_
#define MSYK_COMMON_HPP_

#include "stdwin.h"

/// TCHAR文字列クラス
typedef std::basic_string<TCHAR> mxString;

/// ライブラリバージョン(数値)
#define MSYK_VER		0x100000
/// ライブラリバージョン(文字列)
#define MSYK_VER_STR	_T("1.0.0")

/// AppClassのグローバル変数宣言マクロ
#define DECLARE_APP(AppClass)	\
	class AppClass;				\
	extern AppClass theApp;

/// AppClassのグローバル変数およびmain関数定義マクロ
#define IMPLEMENT_APP(AppClass)			\
	AppClass theApp;						\
	mxApp* getApp() { return &theApp; }		\
	int main(int, char**) {				\
		return theApp.run();				\
	}

/// Eclipse用デバッグマクロ
#ifdef _DEBUG
	#if defined(UNICODE) || defined(_UNICODE)
		#define mxTrace(x)		std::wcout << x << std::endl; std::wcout << std::flush;
		#define mxTraceR(x)	std::wcout << x << std::endl; std::wcout << std::flush;
	#else
		#define mxTrace(x)		std::cout << x << std::endl;
		#define mxTraceR(x)	std::cout << x << std::endl;
	#endif
#else
	#if defined(UNICODE) || defined(_UNICODE)
		#define mxTrace(x)
		#define mxTraceR(x)	std::wcout << x << std::endl; std::wcout << std::flush;
	#else
		#define mxTrace(x)
		#define mxTraceR(x)	std::cout << x << std::endl;
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
/// POINT構造体の拡張クラス
///
class mxPoint : public POINT
{
public:
	/// コンストラクタ
	mxPoint() {
		x = 0;
		y = 0;
	}
	/// コンストラクタ
	/// @param x X座標
	/// @param y Y座標
	mxPoint(int x, int y) {
		this->x = x;
		this->y = y;
	}
	/// コンストラクタ
	/// @param dw 下位ワードにX座標、上位ワードにY座標
	explicit mxPoint(DWORD dw) {
		x = LOWORD(dw);
		y = HIWORD(dw);
	}
	/// コピーコンストラクタ
	mxPoint(const mxPoint &rhs) {
		x = rhs.x;
		y = rhs.y;
	}
	/// デストラクタ
	~mxPoint() {
	}

	////////////////////////////////////////////////////////////////////////////
	/// @name 演算子
	//@{
	bool operator==(const mxPoint &rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const mxPoint &rhs) const {
		return x != rhs.x || y != rhs.y;
	}
	mxPoint operator+(const mxPoint &rhs) const {
		mxPoint pt(*this);
		pt.x += rhs.x;
		pt.y += rhs.y;
		return pt;
	}
	mxPoint& operator+=(const mxPoint &rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	mxPoint operator-(const mxPoint &rhs) const {
		mxPoint pt(*this);
		pt.x -= rhs.x;
		pt.y -= rhs.y;
		return pt;
	}
	mxPoint& operator-=(const mxPoint &rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	template <class NUM>
	mxPoint operator*(const NUM &n) const {
		mxPoint pt(*this);
		pt.x = static_cast<int>(1.0 * pt.x * n);
		pt.y = static_cast<int>(1.0 * pt.y * n);
		return pt;
	}
	template <class NUM>
	mxPoint& operator*=(const NUM &n) {
		x = static_cast<int>(1.0 * x * n);
		y = static_cast<int>(1.0 * y * n);
		return *this;
	}
	template <class NUM>
	mxPoint operator/(const NUM &n) const {
		mxPoint pt(*this);
		pt.x = static_cast<int>(1.0 * pt.x / n);
		pt.y = static_cast<int>(1.0 * pt.y / n);
		return pt;
	}
	template <class NUM>
	mxPoint& operator/=(const NUM &n) {
		x = static_cast<int>(1.0 * x / n);
		y = static_cast<int>(1.0 * y / n);
		return *this;
	}
	//@}
};

///////////////////////////////////////////////////////////////////////////////
/// SIZE構造体の拡張クラス
///
class mxSize : public SIZE
{
public:
	/// コンストラクタ
	mxSize() {
		cx = 0;
		cy = 0;
	}
	/// コンストラクタ
	/// @param cx 幅
	/// @param cy 高さ
	mxSize(int cx, int cy) {
		this->cx = cx;
		this->cy = cy;
	}
	/// コンストラクタ
	/// @param dw 下位ワードに幅、上位ワードに高さ
	explicit mxSize(DWORD dw) {
		cx = LOWORD(dw);
		cy = HIWORD(dw);
	}
	/// デストラクタ
	~mxSize() {
	}

	////////////////////////////////////////////////////////////////////////////
	/// @name 演算子
	//@{
	bool operator==(const mxSize &rhs) const {
		return cx == rhs.cx && cy == rhs.cy;
	}
	bool operator!=(const mxSize &rhs) const {
		return cx != rhs.cx || cy != rhs.cy;
	}
	//@}
};

///////////////////////////////////////////////////////////////////////////////
/// RECT構造体の拡張クラス
///
class mxRect : public RECT
{
public:
	/// コンストラクタ
	mxRect() {
		::SetRectEmpty(this);
	}
	/// コンストラクタ
	mxRect(int left, int top, int right, int bottom) {
		::SetRect(this, left, top, right, bottom);
	}
	/// コンストラクタ
	mxRect(const mxPoint &pt, const mxSize &sz) {
		left = pt.x;
		top = pt.y;
		right = pt.x + sz.cx;
		bottom = pt.y + sz.cy;
	}
	/// コンストラクタ
	mxRect(const RECT &rc) {
		::CopyRect(this, &rc);
	}
	/// デストラクタ
	~mxRect() {
	}

	////////////////////////////////////////////////////////////////////////////
	/// @name 属性の取得
	//@{
	/// 幅を取得します
	int width() const { return right - left; }
	/// 高さを取得します
	int height() const { return bottom - top; }
	/// サイズを取得します
	mxSize size() const { return mxSize(width(), height()); }
	/// 左上座標を取得します
	mxPoint topLeft() const { return mxPoint(left, top); }
	/// 右下座標を取得します
	mxPoint bottomRight() const { return mxPoint(right, bottom); }
	//@}

	////////////////////////////////////////////////////////////////////////////
	/// @name 演算子
	//@{
	/// 位置の加算(offset)
	mxRect operator+(const mxPoint &pt) const {
		mxRect rc(*this);
		::OffsetRect(&rc, pt.x, pt.y);
		return rc;
	}
	/// 位置の減算(offset)
	mxRect operator-(const mxPoint &pt) const {
		mxRect rc(*this);
		::OffsetRect(&rc, -pt.x, -pt.y);
		return rc;
	}
	/// 位置の加算(offset)と代入
	mxRect& operator+=(const mxPoint &pt) {
		::OffsetRect(this, pt.x, pt.y);
		return *this;
	}
	/// 位置の減算(offset)と代入
	mxRect& operator-=(const mxPoint &pt) {
		::OffsetRect(this, -pt.x, -pt.y);
		return *this;
	}

	/// サイズの加算(inflate)
	mxRect operator+(const mxSize &sz) const {
		mxRect rc(*this);
		::InflateRect(&rc, sz.cx, sz.cy);
		return rc;
	}
	/// サイズの減算(deflate)
	mxRect operator-(const mxSize &sz) const {
		mxRect rc(*this);
		::InflateRect(&rc, -sz.cx, -sz.cy);
		return rc;
	}
	/// サイズの加算(inflate)と代入
	mxRect& operator+=(const mxSize &sz) {
		::InflateRect(this, sz.cx, sz.cy);
		return *this;
	}
	/// サイズの減算(deflate)
	mxRect& operator-=(const mxSize &sz) {
		::InflateRect(this, -sz.cx, -sz.cy);
		return *this;
	}

	template <class NUM>
	mxRect operator*(const NUM &n) const {
		mxRect rc(*this);
		rc.left = static_cast<int>(1.0 * rc.left * n);
		rc.top = static_cast<int>(1.0 * rc.top * n);
		rc.right = static_cast<int>(1.0 * rc.right * n);
		rc.bottom = static_cast<int>(1.0 * rc.bottom * n);
		return rc;
	}
	template <class NUM>
	mxRect& operator*=(const NUM &n) {
		left = static_cast<int>(1.0 * left * n);
		top = static_cast<int>(1.0 * top * n);
		right = static_cast<int>(1.0 * right * n);
		bottom = static_cast<int>(1.0 * bottom * n);
		return *this;
	}
	template <class NUM>
	mxRect operator/(const NUM &n) const {
		mxRect rc(*this);
		rc.left = static_cast<int>(1.0 * rc.left / n);
		rc.top = static_cast<int>(1.0 * rc.top / n);
		rc.right = static_cast<int>(1.0 * rc.right / n);
		rc.bottom = static_cast<int>(1.0 * rc.bottom / n);
		return rc;
	}
	template <class NUM>
	mxRect& operator/=(const NUM &n) {
		left = static_cast<int>(1.0 * left / n);
		top = static_cast<int>(1.0 * top / n);
		right = static_cast<int>(1.0 * right / n);
		bottom = static_cast<int>(1.0 * bottom / n);
		return *this;
	}

	//@}
};

///////////////////////////////////////////////////////////////////////////////
/// シングルトンパターンの基底クラス
/// @see http://cflat-inc.hatenablog.com/entry/2014/03/04/214608
template <class T>
class mxSingleton
{
public:
	/// インスタンスを取得する
	/// @retval オブジェクトへの参照
	static T& singleton()
	{
		static typename T::pointer_type s_singleton(T::createInstance());

		return getReference(s_singleton) ;
	}

protected:
	/// コンストラクタ
	mxSingleton() {}

private:
	typedef std::auto_ptr<T> pointer_type;

	static T *createInstance() { return new T(); }
	static T &getReference(const pointer_type &ptr) { return *ptr; }

	mxSingleton(const mxSingleton &);
	mxSingleton& operator=(const mxSingleton &);
};

///////////////////////////////////////////////////////////////////////////////
// クローザーテンプレート
template <class T>
struct mxCloser
{
	void operator()(T &handle);
};
// HMODULEバージョン
template <>
struct mxCloser<HMODULE>
{
	void operator()(HMODULE &module) {
		if (module) {
			::FreeLibrary(module);
			module = NULL;
		}
	}
};
// HFONTバージョン
template <>
struct mxCloser<HFONT>
{
	void operator()(HFONT &obj) {
		if (obj) {
			::DeleteObject(obj);
			obj = NULL;
		}
	}
};
// HBITMAPバージョン
template <>
struct mxCloser<HBITMAP>
{
	void operator()(HBITMAP &obj) {
		if (obj) {
			::DeleteObject(obj);
			obj = NULL;
		}
	}
};
// HBRUSHバージョン
template <>
struct mxCloser<HBRUSH>
{
	void operator()(HBRUSH &obj) {
		if (obj) {
			::DeleteObject(obj);
			obj = NULL;
		}
	}
};
//HDCバージョン
template <>
struct mxCloser<HDC>
{
	void operator()(HDC &hdc) {
		if (hdc) {
			::DeleteDC(hdc);
			hdc = NULL;
		}
	}
};
// HMIXERバージョン
template <>
struct mxCloser<HMIXER>
{
	void operator()(HMIXER &hMixer) {
		if (hMixer) {
			::mixerClose(hMixer);
			hMixer = NULL;
		}
	}
};

//HANDLEバージョン
struct mxHandleCloser
{
	void operator()(HANDLE &handle) {
		if (handle && handle != INVALID_HANDLE_VALUE) {
			::CloseHandle(handle);
			handle = NULL;
		}
	}
};
// FindCloseバージョン
struct mxFindCloser
{
	void operator()(HANDLE &handle) {
		if (handle && handle != INVALID_HANDLE_VALUE) {
			::FindClose(handle);
			handle = NULL;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// デストラクタで解放を行うオブジェクトの基底クラス
///
template<class H, class U = mxCloser<H> >
class mxAutoClose
{
public:
	/// コンストラクタ
	mxAutoClose() : m_closer() {
		m_handle = NULL;
	}
	/// コンストラクタ
	mxAutoClose(const H& handle) : m_closer() {
		m_handle = handle;
	}
	/// デストラクタ
	virtual ~mxAutoClose() {
		m_closer(m_handle);
	}
	/// 格納されているハンドルを取得します
	H& get() { return m_handle; }

	/// キャスト演算子
	operator H() const { return m_handle; }
	/// 代入演算子
	H& operator=(const H& rhs) {
		safeClose();
		m_handle = rhs;
		return m_handle;
	}
	/// アドレス演算子
	H* operator*() { return &m_handle; }
	/// アドレス演算子(const版)
	const H* operator*() const { return &m_handle; }

	/// 安全な解放
	void safeClose() {
		m_closer(m_handle);
	}

protected:
	H m_handle;
	U m_closer;
};

typedef mxAutoClose<HANDLE, mxHandleCloser> mxAutoHandle;
typedef mxAutoClose<HANDLE, mxFindCloser> mxAutoFind;

///////////////////////////////////////////////////////////////////////////////
/// 構造体の初期化用クラス
///
template <class T>
class mxZero
{
public:
	mxZero() {
		::ZeroMemory(&m_obj, sizeof(T));
	}
	virtual ~mxZero() {
	}

	T* operator->() { return &m_obj; }
	const T* operator->() const { return &m_obj; }

	T* get() { return &m_obj; }
	const T* get() const { return &m_obj; }


private:
	T m_obj;
};

///////////////////////////////////////////////////////////////////////////////
/// 例外の基底クラス
///
class mxException
{
public:
	/// コンストラクタ
	/// @param lpString 例外文字列
	mxException(LPCTSTR lpString) : m_str(lpString) {
	}
	/// デストラクタ
	virtual ~mxException() {
	}
	/// 例外文字列を取得します
	LPCTSTR what() const {
		return m_str.c_str();
	}

protected:
	mxString m_str;	///< 例外文字列

	/// コンストラクタ
	mxException() : m_str() {
	}
};

///////////////////////////////////////////////////////////////////////////////
/// Win32APIの例外クラス
///
class mxWin32Exception : public mxException
{
public:
	/// コンストラクタ
	/// @param lpszApi API名称
	mxWin32Exception(LPCTSTR lpszApi) : mxException() {
		init(lpszApi, ::GetLastError());
	}
	/// コンストラクタ
	/// @param lpszApi API名称
	/// @param dwError エラーコード
	mxWin32Exception(LPCTSTR lpszApi, DWORD dwError) : mxException() {
		init(lpszApi, dwError);
	}
	/// デストラクタ
	virtual ~mxWin32Exception() {
	}

private:
	/// 例外文字列を初期化します
	/// @param lpszApi API名称
	/// @param dwError エラーコード
	void init(LPCTSTR lpszApi, DWORD dwError);
};
#endif /* MSYK_COMMON_HPP_ */
