///////////////////////////////////////////////////////////////////////////////
/// @file	dibtransition.hpp
/// @date	2014/07/28	
/// @author	Masayuki
/// @brief	DIBトランジションに関するクラス
///////////////////////////////////////////////////////////////////////////////

#ifndef MSYK_DIBTRANSITION_HPP_
#define MSYK_DIBTRANSITION_HPP_

#include "dibsection.hpp"

#define TRANSITION_END (0x00FFFFFF)	///< トランジションの終了を表す定数

///////////////////////////////////////////////////////////////////////////////
/// トランジションを行う抽象基底クラス
///
template <class T>
class mxTransition
{
public:
	/// コンストラクタ
	mxTransition() : m_Operator() {
	}
	/// デストラクタ
	virtual ~mxTransition() {
	}

	/// ピクセルオペレータを取得します
	T& getOperator() { return m_Operator;}
	/// ピクセルオペレータを取得します(const版)
	const T& getOperator() const { return m_Operator;}

protected:
	T m_Operator;	///< ピクセルオペレータ
};

namespace mxPixelOp {

///////////////////////////////////////////////////////////////////////////////
/// 単色で塗りつぶすオペレータ
struct Fill
{
	mxRGBA rgbColor;	///< 塗りつぶす色

	void operator()(mxRGBA &X) {
		X = rgbColor;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// コピーを行うオペレータ
struct Copy
{
	void operator()(mxRGBA &X, const mxRGBA &Y) {
		X = Y;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// 透過コピーを行うオペレータ
struct TransparentCopy
{
	mxRGBA colorKey;	///< 透過色

	void operator()(mxRGBA &X, const mxRGBA &Y) {
		if (Y != colorKey) {
			X = Y;
		}
	}
	void operator()(mxRGBA &X, const mxRGBA &Y, const mxRGBA &Z) {
		if (Z != colorKey) {
			X = Y;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// アルファブレンドを行うオペレータ
struct AlphaBlend
{
	BYTE alpha;	///< アルファ値

	void operator()(mxRGBA &X, const mxRGBA &Y) {
		if (alpha == 0xFF) {
			X = Y;
		}
		else {
			X.r = X.r + (((Y.r - X.r) * alpha) >> 8);
			X.g = X.g + (((Y.g - X.g) * alpha) >> 8);
			X.b = X.b + (((Y.b - X.b) * alpha) >> 8);
		}
	}
	void operator()(mxRGBA &X, const mxRGBA &Y, const mxRGBA &Z) {
		if (alpha == 0) {
			X = Y;
		}
		else if (alpha == 0xFF) {
			X = Z;
		}
		else {
			X.r = Y.r + (((Z.r - Y.r) * alpha) >> 8);
			X.g = Y.g + (((Z.g - Y.g) * alpha) >> 8);
			X.b = Y.b + (((Z.b - Y.b) * alpha) >> 8);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// 透過＋アルファブレンドを行うオペレータ
struct TransparentAlphaBlend
{
	mxRGBA colorKey;	///< 透過色
	BYTE alpha;			///< アルファ値

	void operator()(mxRGBA &X, const mxRGBA &Y) {
		if (Y == colorKey) {
			return;
		}
		if (alpha == 0xFF) {
			X = Y;
		}
		else {
			X.r = X.r + (((Y.r - X.r) * alpha) >> 8);
			X.g = X.g + (((Y.g - X.g) * alpha) >> 8);
			X.b = X.b + (((Y.b - X.b) * alpha) >> 8);
		}
	}
	void operator()(mxRGBA &X, const mxRGBA &Y, const mxRGBA &Z) {
		if (Z == colorKey) {
			return;
		}
		if (alpha == 0) {
			X = Y;
		}
		else if (alpha == 0xFF) {
			X = Z;
		}
		else {
			X.r = Y.r + (((Z.r - Y.r) * alpha) >> 8);
			X.g = Y.g + (((Z.g - Y.g) * alpha) >> 8);
			X.b = Y.b + (((Z.b - Y.b) * alpha) >> 8);
		}
	}
};

}  // namespace mxPixelOp

namespace mxTrans {

///////////////////////////////////////////////////////////////////////////////
/// 単純転送を行う
///
template <class T>
class Blt : public mxTransition<T>
{
public:
	/// コンストラクタ
	Blt() : mxTransition<T>() {
	}
	/// デストラクタ
	virtual ~Blt() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		mxRGBA *pBitsA = pA->bits();
		int aX, aY, aPos;
		for (aY = rcA.top; aY < rcA.bottom; aY++) {
			aX = rcA.left;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			for (; aX < rcA.right; aX++) {
				mxTransition<T>::m_Operator(pBitsA[aPos++]);
			}
		}
		return TRANSITION_END;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		mxRGBA *pBitsA = pA->bits();
		const mxRGBA *pBitsB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos;
		bY = ptB.y;
		for (aY = rcA.top; aY < rcA.bottom; aY++, bY++) {
			aX = rcA.left;
			bX = ptB.x;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			bPos = pB->getPixelPos(mxPoint(bX, bY));
			for (; aX < rcA.right; aX++) {
				mxTransition<T>::m_Operator(pBitsA[aPos++], pBitsB[bPos++]);
			}
		}
		return TRANSITION_END;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		mxRGBA *pBitsA = pA->bits();
		const mxRGBA *pBitsB = pB->bits();
		const mxRGBA *pBitsC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos;
		bY = ptB.y;
		cY = ptC.y;
		for (aY = rcA.top; aY < rcA.bottom; aY++, bY++, cY++) {
			aX = rcA.left;
			bX = ptB.x;
			cX = ptC.x;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			bPos = pB->getPixelPos(mxPoint(bX, bY));
			cPos = pC->getPixelPos(mxPoint(cX, cY));
			for (; aX < rcA.right; aX++) {
				mxTransition<T>::m_Operator(pBitsA[aPos++], pBitsB[bPos++], pBitsC[cPos++]);
			}
		}
		return TRANSITION_END;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// 4x4の領域に1ピクセルずつ打つ
///
template <class T>
class Dot4x4 : public mxTransition<T>
{
public:
	/// コンストラクタ
	Dot4x4(int nStep):mxTransition<T>() {
		m_nStep = nStep;
	}
	/// デストラクタ
	virtual ~Dot4x4() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		int dx, dy;
		if (!getDistance(dx, dy)) {
			return TRANSITION_END;
		}
		mxRGBA *pPixelA = pA->bits();
		int aX, aY, aPos;
		for (aY = rcA.top + dy; aY < rcA.bottom; aY += 4) {
			aX = rcA.left + dx;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			for (; aX < rcA.right; aX += 4) {
				mxTransition<T>::m_Operator(pPixelA[aPos]);
				aPos += 4;
			}
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		int dx, dy;
		if (!getDistance(dx, dy)) {
			return TRANSITION_END;
		}
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos;
		bY = ptB.y + dy;
		for (aY = rcA.top + dy; aY < rcA.bottom; aY += 4) {
			aX = rcA.left + dx;
			bX = ptB.x + dx;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			bPos = pB->getPixelPos(mxPoint(bX, bY));
			for (; aX < rcA.right; aX += 4) {
				mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos]);
				aPos += 4;
				bPos += 4;
			}
			bY += 4;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		int dx, dy;
		if (!getDistance(dx, dy)) {
			return TRANSITION_END;
		}
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		const mxRGBA *pPixelC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos;
		bY = ptB.y + dy;
		cY = ptC.y + dy;
		for (aY = rcA.top + dy; aY < rcA.bottom; aY += 4) {
			aX = rcA.left + dx;
			bX = ptB.x + dx;
			cX = ptC.x + dx;
			aPos = pA->getPixelPos(mxPoint(aX, aY));
			bPos = pB->getPixelPos(mxPoint(bX, bY));
			cPos = pC->getPixelPos(mxPoint(cX, cY));
			for (; aX < rcA.right; aX += 4) {
				mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos], pPixelC[cPos]);
				aPos += 4;
				bPos += 4;
				cPos += 4;
			}
			bY += 4;
			cY += 4;
		}
		return m_nStep + 1;
	}

private:
	int m_nStep;

	bool getDistance(int &dx, int &dy)
	{
		// 4x4の領域を次の順番で塗りつぶす
		//     0   1   2   3
		// 0 [ 1][13][ 4][16]
		// 1 [ 9][ 5][12][ 8]
		// 2 [ 3][15][ 2][14]
		// 3 [11][ 7][10][ 6]
		switch (m_nStep) {
		case  1: dx = 0; dy = 0; break;
		case  2: dx = 2; dy = 2; break;
		case  3: dx = 0; dy = 2; break;
		case  4: dx = 2; dy = 0; break;
		case  5: dx = 1; dy = 1; break;
		case  6: dx = 3; dy = 3; break;
		case  7: dx = 1; dy = 3; break;
		case  8: dx = 3; dy = 1; break;
		case  9: dx = 0; dy = 1; break;
		case 10: dx = 2; dy = 3; break;
		case 11: dx = 0; dy = 3; break;
		case 12: dx = 2; dy = 1; break;
		case 13: dx = 1; dy = 0; break;
		case 14: dx = 3; dy = 2; break;
		case 15: dx = 1; dy = 2; break;
		case 16: dx = 3; dy = 0; break;
		default:
			return false;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// アコーディオン(上から下)
///
template <class T>
class AccordionT2B : public mxTransition<T>
{
public:
	/// コンストラクタ
	AccordionT2B(int nStep, int bandWidth = 14) : mxTransition<T>() {
		m_nStep = nStep;
		m_nBandW = bandWidth;
	}
	/// デストラクタ
	virtual ~AccordionT2B() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		mxRGBA *pPixelA = pA->bits();
		int aX, aY, aPos, bandWidth = 0;
		for (aY = rcA.top + (m_nBandW - 1) + (m_nBandW * m_nStep); aY >= rcA.top; aY -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY < rcA.bottom) {
				aX = rcA.left;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++]);
				}
			}
		}

		if (aY >= rcA.bottom) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos, bandWidth = 0;
		bY = ptB.y + (m_nBandW - 1) + (m_nBandW * m_nStep);
		for (aY = rcA.top + (m_nBandW - 1) + (m_nBandW * m_nStep); aY >= rcA.top; aY -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY < rcA.bottom) {
				aX = rcA.left;
				bX = ptB.x;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				bPos = pB->getPixelPos(mxPoint(bX, bY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++], pPixelB[bPos++]);
				}
			}
			bY -= (m_nBandW + 1);
		}

		if (aY >= rcA.bottom) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		const mxRGBA *pPixelC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos, bandWidth = 0;
		bY = ptB.y + (m_nBandW - 1) + (m_nBandW * m_nStep);
		cY = ptC.y + (m_nBandW - 1) + (m_nBandW * m_nStep);
		for (aY = rcA.top + (m_nBandW - 1) + (m_nBandW * m_nStep); aY >= rcA.top; aY -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY < rcA.bottom) {
				aX = rcA.left;
				bX = ptB.x;
				cX = ptC.x;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				bPos = pB->getPixelPos(mxPoint(bX, bY));
				cPos = pC->getPixelPos(mxPoint(cX, cY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++], pPixelB[bPos++], pPixelC[cPos++]);
				}
			}
			bY -= (m_nBandW + 1);
			cY -= (m_nBandW + 1);
		}

		if (aY >= rcA.bottom) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

private:
	int m_nStep;
	int m_nBandW;
};

///////////////////////////////////////////////////////////////////////////////
/// アコーディオン(下から上)
///
template <class T>
class AccordionB2T : public mxTransition<T>
{
public:
	/// コンストラクタ
	AccordionB2T(int nStep, int bandWidth = 14): mxTransition<T>() {
		m_nStep = nStep;
		m_nBandW = bandWidth;
	}
	/// デストラクタ
	virtual ~AccordionB2T() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		mxRGBA *pPixelA = pA->bits();
		int aX, aY, aPos, bandWidth = 0;
		for (aY = rcA.bottom - (m_nBandW - 1) - (m_nBandW * m_nStep); aY < rcA.bottom; aY += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY >= rcA.top) {
				aX = rcA.left;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++]);
				}
			}
		}

		if (aY < rcA.top) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos, bandWidth = 0;
		bY = (ptB.y + rcA.height()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		for (aY = rcA.bottom - (m_nBandW - 1) - (m_nBandW * m_nStep); aY < rcA.bottom; aY += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY >= rcA.top) {
				aX = rcA.left;
				bX = ptB.x;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				bPos = pB->getPixelPos(mxPoint(bX, bY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++], pPixelB[bPos++]);
				}
			}
			bY += (m_nBandW + 1);
		}

		if (aY < rcA.top) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		const mxRGBA *pPixelC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos, bandWidth = 0;
		bY = (ptB.y + rcA.height()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		cY = (ptC.y + rcA.height()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		for (aY = rcA.bottom - (m_nBandW - 1) - (m_nBandW * m_nStep); aY < rcA.bottom; aY += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aY >= rcA.top) {
				aX = rcA.left;
				bX = ptB.x;
				cX = ptC.x;
				aPos = pA->getPixelPos(mxPoint(aX, aY));
				bPos = pB->getPixelPos(mxPoint(bX, bY));
				cPos = pC->getPixelPos(mxPoint(cX, cY));
				for (; aX < rcA.right; aX++) {
					mxTransition<T>::m_Operator(pPixelA[aPos++], pPixelB[bPos++], pPixelC[cPos++]);
				}
			}
			bY += (m_nBandW + 1);
			cY += (m_nBandW + 1);
		}

		if (aY < rcA.top) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

private:
	int m_nStep;
	int m_nBandW;
};

///////////////////////////////////////////////////////////////////////////////
/// アコーディオン(左から右)
///
template <class T>
class AccordionL2R : public mxTransition<T>
{
public:
	/// コンストラクタ
	AccordionL2R(int nStep, int bandWidth = 14) : mxTransition<T>() {
		m_nStep = nStep;
		m_nBandW = bandWidth;
	}
	/// デストラクタ
	virtual ~AccordionL2R() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		mxRGBA *pPixelA = pA->bits();
		int aX, aY, aPos, bandWidth = 0;
		for (aX = rcA.left + (m_nBandW - 1) + (m_nBandW * m_nStep); aX >= rcA.left; aX -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX < rcA.right) {
				aY = rcA.top;
				for (; aY < rcA.bottom; aY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					mxTransition<T>::m_Operator(pPixelA[aPos]);
				}
			}
		}

		if (aX >= rcA.right) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos, bandWidth = 0;
		bX = ptB.x + (m_nBandW - 1) + (m_nBandW * m_nStep);
		for (aX = rcA.left + (m_nBandW - 1) + (m_nBandW * m_nStep); aX >= rcA.left; aX -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX < rcA.right) {
				aY = rcA.top;
				bY = ptB.y;
				for (; aY < rcA.bottom; aY++, bY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					bPos = pB->getPixelPos(mxPoint(bX, bY));
					mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos]);
				}
			}
			bX -= (m_nBandW + 1);
		}

		if (aX >= rcA.right) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		const mxRGBA *pPixelC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos, bandWidth = 0;
		bX = ptB.x + (m_nBandW - 1) + (m_nBandW * m_nStep);
		cX = ptC.x + (m_nBandW - 1) + (m_nBandW * m_nStep);
		for (aX = rcA.left + (m_nBandW - 1) + (m_nBandW * m_nStep); aX >= rcA.left; aX -= (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX < rcA.right) {
				aY = rcA.top;
				bY = ptB.y;
				cY = ptC.y;
				for (; aY < rcA.bottom; aY++, bY++, cY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					bPos = pB->getPixelPos(mxPoint(bX, bY));
					cPos = pC->getPixelPos(mxPoint(cX, cY));
					mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos], pPixelC[cPos]);
				}
			}
			bX -= (m_nBandW + 1);
			cX -= (m_nBandW + 1);
		}

		if (aX >= rcA.right) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

private:
	int m_nStep;
	int m_nBandW;
};

///////////////////////////////////////////////////////////////////////////////
/// アコーディオン(右から左)
///
template <class T>
class AccordionR2L : public mxTransition<T>
{
public:
	/// コンストラクタ
	AccordionR2L(int nStep, int bandWidth = 14) : mxTransition<T>() {
		m_nStep = nStep;
		m_nBandW = bandWidth;
	}
	/// デストラクタ
	virtual ~AccordionR2L() {
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA
			)
	{
		mxRGBA *pPixelA = pA->bits();
		int aX, aY, aPos, bandWidth = 0;
		for (aX = rcA.right - (m_nBandW - 1) - (m_nBandW * m_nStep); aX < rcA.right; aX += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX >= rcA.left) {
				aY = rcA.top;
				for (; aY < rcA.bottom; aY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					mxTransition<T>::m_Operator(pPixelA[aPos]);
				}
			}
		}

		if (aX < rcA.left) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		int aX, aY, aPos, bX, bY, bPos, bandWidth = 0;
		bX = (ptB.x + rcA.width()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		for (aX = rcA.right - (m_nBandW - 1) - (m_nBandW * m_nStep); aX < rcA.right; aX += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX >= rcA.left) {
				aY = rcA.top;
				bY = ptB.y;
				for (; aY < rcA.bottom; aY++, bY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					bPos = pB->getPixelPos(mxPoint(bX, bY));
					mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos]);
				}
			}
			bX += (m_nBandW + 1);
		}

		if (aX < rcA.left) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

	int doTransition(
			mxDibSection *pA,
			const mxRect &rcA,
			const mxDibSection *pB,
			const mxPoint &ptB,
			const mxDibSection *pC,
			const mxPoint &ptC
			)
	{
		mxRGBA *pPixelA = pA->bits();
		const mxRGBA *pPixelB = pB->bits();
		const mxRGBA *pPixelC = pC->bits();
		int aX, aY, aPos, bX, bY, bPos, cX, cY, cPos, bandWidth = 0;
		bX = (ptB.x + rcA.width()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		cX = (ptC.x + rcA.width()) - (m_nBandW - 1) - (m_nBandW * m_nStep);
		for (aX = rcA.right - (m_nBandW - 1) - (m_nBandW * m_nStep); aX < rcA.right; aX += (m_nBandW + 1)) {
			if (bandWidth++ > m_nBandW) {
				break;
			}
			if (aX >= rcA.left) {
				aY = rcA.top;
				bY = ptB.y;
				cY = ptC.y;
				for (; aY < rcA.bottom; aY++, bY++, cY++) {
					aPos = pA->getPixelPos(mxPoint(aX, aY));
					bPos = pB->getPixelPos(mxPoint(bX, bY));
					cPos = pC->getPixelPos(mxPoint(cX, cY));
					mxTransition<T>::m_Operator(pPixelA[aPos], pPixelB[bPos], pPixelC[cPos]);
				}
			}
			bX += (m_nBandW + 1);
			cX += (m_nBandW + 1);
		}

		if (aX < rcA.left) {
			return TRANSITION_END;
		}
		return m_nStep + 1;
	}

private:
	int m_nStep;
	int m_nBandW;
};

}  // namespace mxTrans

#endif /* MSYK_DIBTRANSITION_HPP_ */
