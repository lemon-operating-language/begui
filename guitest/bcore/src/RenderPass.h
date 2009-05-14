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
	Texture		*m_pDepthTex;
	bool		m_bOwnsTexture;
	int			m_width;
	int			m_height;
	PixelFormat	m_format;
	bool		m_bGenMipmaps;

	bool		m_bUsePBuffer;
	
	GLuint		m_FBO;			// frame buffer object for rendering
	GLuint		m_FBODepthBuffer;

	PBuffer		m_pbuffer;	// pbuffer for rendering
	HDC			m_last_hdc;
	HGLRC		m_last_hglrc;

public:
	RenderPass();
	~RenderPass();

	void setMipmapAutoGen(bool bGenMipmaps)		{ m_bGenMipmaps; }	// should be called BEFORE creating the renderpass!!!
	bool setup(PixelFormat pixelFormat, int frameW, int frameH, Texture *pTarget = 0, bool bDepthBuffer=false);
	void free();
	void beginPass();
	void endPass();
	
	Texture*	getFrameData()			{ return m_pFrameTexture; }
	Texture*	getDepthData()			{ return m_pDepthTex; }
	PixelFormat	getPixelFormat() const	{ return m_format; }
	int			getWidth() const		{ return m_width; }
	int			getHeight() const		{ return m_height; }
	bool		isGenMipmapsEnabled() const	{ return m_bGenMipmaps; }
	bool		isUsingPBuffer() const		{ return m_bUsePBuffer; }

private:
	void makePBufferCurrent();
};