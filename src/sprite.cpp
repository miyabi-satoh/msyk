///////////////////////////////////////////////////////////////////////////////
/// @file	sprite.cpp
/// @date	2014/07/30	
/// @author	Masayuki
/// @brief	スプライトクラス
///////////////////////////////////////////////////////////////////////////////

#include "sprite.hpp"
#include "dibtransition.hpp"

bool mxSprite::clip(mxDibSection &dstDib, mxRect &rcDst, mxPoint &ptSrc)
{
	// いやいやいや…
	if (this->width() == 0 || this->height() == 0) { return false;}
	if (dstDib.width() == 0 || dstDib.height() == 0) { return false; }

	// 見えてない
	if (this->alpha() == 0) { return false; }

	// 完全に範囲外かどうか
	if (this->x() + this->width() < 0) { return false; }		// 左にハズレている
	if (this->Y() + this->height() < 0) { return false; }	// 上に　〃
	if (dstDib.width() <= this->x()) { return false; }		// 右に　〃
	if (dstDib.height() <= this->Y()) { return false; }		// 下に　〃

	// クリッピング
	rcDst = mxRect(this->getPos(), this->getSize());
	ptSrc = mxPoint();
	if (rcDst.left < 0) {	// 左にはみ出ている
		ptSrc.x = 0 - rcDst.left;
		rcDst.left = 0;
	}
	if (rcDst.top < 0) {	// 上にはみ出ている
		ptSrc.y = 0 - rcDst.top;
		rcDst.top = 0;
	}
	if (dstDib.width() < rcDst.right) { // 右にはみ出ている
		rcDst.right = dstDib.width();
	}
	if (dstDib.height() < rcDst.bottom) { // 下にはみ出ている
		rcDst.bottom = dstDib.height();
	}
	return true;
}

void mxSprite::render(mxDibSection &dstDib)
{
	mxRect rcDst;
	mxPoint ptSrc;
	if (clip(dstDib, rcDst, ptSrc)) {
		fastRender(dstDib, rcDst, ptSrc);
	}
}

void mxSprite::fastRender(
		mxDibSection &dstDib,
		const mxRect &dstRect,
		const mxPoint &srcPoint)
{
	// 転送
	if (alpha() == 0xFF) {
		if (colorKey() == CLR_INVALID) {
			// アルファなし、透過なし
			this->bitBlt(dstDib, dstRect, srcPoint);
		}
		else {
			// アルファなし、透過あり
			mxTrans::Blt<mxPixelOp::TransparentCopy> Trans;
			Trans.getOperator().colorKey = this->colorKey();
			Trans.doTransition(&dstDib, dstRect, this, srcPoint);
		}
	}
	else {
		if (colorKey() == CLR_INVALID) {
			// アルファあり、透過なし
			mxTrans::Blt<mxPixelOp::AlphaBlend> Trans;
			Trans.getOperator().alpha = this->alpha();
			Trans.doTransition(&dstDib, dstRect, this, srcPoint);
		}
		else {
			// アルファあり、透過あり
			mxTrans::Blt<mxPixelOp::TransparentAlphaBlend> Trans;
			Trans.getOperator().alpha = this->alpha();
			Trans.getOperator().colorKey = this->colorKey();
			Trans.doTransition(&dstDib, dstRect, this, srcPoint);
		}
	}
}


