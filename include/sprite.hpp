///////////////////////////////////////////////////////////////////////////////
/// @file	sprite.hpp
/// @date	2014/08/02
/// @author	Masayuki
/// @brief	class mxSprite
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_SPRITE_HPP_
#define MSYK_SPRITE_HPP_

#include "dibsection.hpp"

class mxSprite : public mxDibSection
{
public:
	/// コンストラクタ
	mxSprite() : mxDibSection(), m_pos() {
		m_z = 0;
		m_ColorKey = CLR_INVALID;
		m_alpha = 0xFF;
	}
	/// デストラクタ
	~mxSprite() {
	}

	/// クリッピングを行う
	/// @param dstDib 描画先DIB
	/// @param rcDst 描画先領域の格納先
	/// @param ptSrc 描画元原点の格納先
	/// @return 描画領域が空になる場合はfalseを返します
	bool clip(mxDibSection &dstDib, mxRect &rcDst, mxPoint &ptSrc);

	/// クリッピング＆レンダリングを行う
	/// @param dstDib 描画先DIB
	void render(mxDibSection &dstDib);

	/// レンダリングのみを行う
	virtual void fastRender(
			mxDibSection &dstDib,	///< 描画先DIB
			const mxRect &dstRect,	///< 描画先領域
			const mxPoint &srcPoint	///< 描画元原点
			);


	/// @name 値の取得
	//@{
	int x() const { return m_pos.x; }
	int Y() const { return m_pos.y; }
	int z() const { return m_z; }
	BYTE alpha() const { return m_alpha; }
	COLORREF colorKey() const { return m_ColorKey; }
	const mxPoint& getPos() const { return m_pos; }
	//@}

	/// @name 値の設定
	//@{
	void setPos(const mxPoint &pt) { m_pos = pt; }
	void setZ(int z) { m_z = z; }
	void setAlpha(BYTE a) { m_alpha = a; }
	void setColorKey(COLORREF c) { m_ColorKey = c; }
	//@}

protected:
	mxPoint m_pos;			///< (x,y)座標
	int m_z;				///< z座標
	BYTE m_alpha;			///< アルファ値
	COLORREF m_ColorKey;	///< 透過色

private:
};

#endif /* MSYK_SPRITE_HPP_ */
