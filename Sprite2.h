#pragma once// Sprite.h
// By Frank Luna
// August 24, 2004.
#ifndef SPRITE_H2
#define SPRITE_H2

#include "main.h"
#include "Vec3.h"
#include "BackBuffer.h"

class Sprite2
{
public:
	Sprite2(int imageID, int maskID);
	Sprite2(const char *szImageFile, const char *szMaskFile);
	Sprite2(const char *szImageFile, COLORREF crTransparentColor);

	virtual ~Sprite2();

	int width() { return mImageBM.bmWidth; }
	int height() { return mImageBM.bmHeight; }
	void update(float dt);

	void setBackBuffer(const BackBuffer *pBackBuffer);
	virtual void draw();

public:
	// Keep these public because they need to be
	// modified externally frequently.
	Vec3 mPosition;
	Vec3 mVelocity;

private:
	// Make copy constructor and assignment operator private
	// so client cannot copy Sprites. We do this because
	// this class is not designed to be copied because it
	// is not efficient--copying bitmaps is slow (lots of memory).
	Sprite2(const Sprite2& rhs);
	Sprite2& operator=(const Sprite2& rhs);

protected:
	HBITMAP mhImage;
	HBITMAP mhMask;
	BITMAP mImageBM;
	BITMAP mMaskBM;

	HDC mhSpriteDC;
	const BackBuffer *mpBackBuffer;

	COLORREF mcTransparentColor;
	void drawTransparent();
	void drawMask();
};



// AnimatedSprite
// by Mihai Popescu
// April 5, 2008
class AnimatedSprite2 : public Sprite2
{
public:
	//NOTE: The animation is on a single row.
	AnimatedSprite2(const char *szImageFile, const char *szMaskFile, const RECT& rcFirstFrame, int iFrameCount);
	virtual ~AnimatedSprite2() { }

public:
	void SetFrame(int iIndex);
	int GetFrameCount() { return miFrameCount; }

	virtual void draw();

protected:
	POINT mptFrameStartCrop;// first point of the frame (upper-left corner)
	POINT mptFrameCrop;		// crop point of frame
	int miFrameWidth;		// width
	int miFrameHeight;		// height
	int miFrameCount;		// number of frames
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LaserSprite2
{
public:
	LaserSprite2(int imageID, int maskID);
	LaserSprite2(const char *szImageFile, const char *szMaskFile);
	LaserSprite2(const char *szImageFile, COLORREF crTransparentColor);

	virtual ~LaserSprite2();

	int width() { return mImageBM.bmWidth; }
	int height() { return mImageBM.bmHeight; }
	void update(float dt);

	void setBackBuffer(const BackBuffer *pBackBuffer);
	virtual void draw();

public:
	// Keep these public because they need to be
	// modified externally frequently.
	Vec3 mPosition;
	Vec3 mVelocity;

private:
	// Make copy constructor and assignment operator private
	// so client cannot copy Sprites. We do this because
	// this class is not designed to be copied because it
	// is not efficient--copying bitmaps is slow (lots of memory).
	LaserSprite2(const LaserSprite2& rhs);
	LaserSprite2& operator=(const LaserSprite2& rhs);

protected:
	HBITMAP mhImage;
	HBITMAP mhMask;
	BITMAP mImageBM;
	BITMAP mMaskBM;

	HDC mhSpriteDC;
	const BackBuffer *mpBackBuffer;

	COLORREF mcTransparentColor;
	void drawTransparent();
	void drawMask();
};






#endif // SPRITE2_H

