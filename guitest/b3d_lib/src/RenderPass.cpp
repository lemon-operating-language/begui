#include "RenderPass.h"
#include <stdlib.h>

//TEMP!
void wglGetLastError() { };

RenderPass::RenderPass() : m_pFrameTexture(0), m_bOwnsTexture(false),
	m_width(-1), m_height(-1),
	m_format(PIXEL_RGBA8),
	m_last_hdc(0),
	m_last_hglrc(0),
	m_pDepthTex(0)
{
}

RenderPass::~RenderPass()
{
	free();
}

bool RenderPass::setup(PixelFormat pixelFormat, int frameW, int frameH, Texture *pTarget, bool bAddDepthTex)
{
	free();

	int texFormat = GL_RGBA8;
	switch (pixelFormat)
	{
	case PIXEL_RGBA8: texFormat = GL_RGBA8; break;
	case PIXEL_RGBA16F: texFormat = GL_RGBA16F_ARB; break;
	case PIXEL_RGBA32F: texFormat = GL_RGBA32F_ARB; break;
	default: ASSERT(0);
	}

	if (pTarget)
	{
		// set an external texture as a render target
		ASSERT(frameW == pTarget->getWidth());
		ASSERT(frameH == pTarget->getHeight());

		m_pFrameTexture = pTarget;
		m_bOwnsTexture = false;
	}
	else
	{
		// Create a texture for the given pixel format
		m_pFrameTexture = new Texture();
		m_pFrameTexture->create(frameW, frameH, texFormat);
		m_bOwnsTexture = true;
	}
	m_format = pixelFormat;
	m_width = frameW;
	m_height = frameH;

	// Create the pbuffer
	if (!m_pbuffer.create(frameW, frameH, texFormat))
	{
		MessageBox(0, "Failed to create pbuffer", "Error", MB_OK);
		exit(-1);
	}

	if (bAddDepthTex)
	{
		m_pDepthTex = new Texture();
		m_pDepthTex->create(frameW, frameH, texFormat);
	}

	return true;
}

void RenderPass::free()
{
	if (m_bOwnsTexture && m_pFrameTexture)
	{
		SAFE_DELETE(m_pFrameTexture);
		SAFE_DELETE(m_pDepthTex);
	}
	m_pbuffer.free();
}

void RenderPass::beginPass()
{
	m_last_hdc = wglGetCurrentDC();
	m_last_hglrc = wglGetCurrentContext();
	wglGetLastError();
	
	// bind the render target texture
    glBindTexture(GL_TEXTURE_2D, m_pFrameTexture->getGLTex());

	// release the pbuffer from the render texture object
	if (wglReleaseTexImageARB(m_pbuffer.getHPBuffer(), WGL_FRONT_LEFT_ARB) == FALSE)
        wglGetLastError();

	if (m_pDepthTex)
	{
		ASSERT(0);
//		wglDrawBuffers(WGL_FRONT_ARB);
		glBindTexture(GL_TEXTURE_2D, m_pDepthTex->getGLTex());
		if (wglReleaseTexImageARB(m_pbuffer.getHPBuffer(), WGL_FRONT_RIGHT_ARB) == FALSE)
			wglGetLastError();
	}

    // make the pbuffer's rendering context current.
	if (wglMakeCurrent( m_pbuffer.getDC(), m_pbuffer.getRenderingContext()) == FALSE)
        wglGetLastError();
}

void RenderPass::endPass()
{
	// make the glut window's rendering context current and draw to the glut window.
    if (wglMakeCurrent(m_last_hdc, m_last_hglrc) == FALSE)
        wglGetLastError();

	// bind the render target texture
    glBindTexture(GL_TEXTURE_2D, m_pFrameTexture->getGLTex());

    // bind the pbuffer to the render texture object
	if (wglBindTexImageARB(m_pbuffer.getHPBuffer(), WGL_FRONT_LEFT_ARB) == FALSE)
        wglGetLastError();
}

void RenderPass::makePBufferCurrent()
{
	m_pbuffer.makeCurrent();
}