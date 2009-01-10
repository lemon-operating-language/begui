#pragma once

#include "common.h"
#include "PBuffer.h"
#include "Texture.h"

class RenderPass
{
public:
	enum PixelFormat {
		PIXEL_RGBA8 = 0,
		PIXEL_RGBA16F,
		PIXEL_RGBA32F,
		PIXEL_FORMATS_NUM
	};

private:
	Texture		*m_pFrameTexture;
	bool		m_bOwnsTexture;
	int			m_width;
	int			m_height;
	PixelFormat	m_format;
	PBuffer		m_pbuffer;

	HDC			m_last_hdc;
	HGLRC		m_last_hglrc;

	Texture *m_pDepthTex;

public:
	RenderPass();
	~RenderPass();
	bool setup(PixelFormat pixelFormat, int frameW, int frameH, Texture *pTarget = 0, bool bAddDepthTex=false);
	void free();
	void beginPass();
	void endPass();
	void makePBufferCurrent();
	Texture* getFrameData()		{ return m_pFrameTexture; }
};