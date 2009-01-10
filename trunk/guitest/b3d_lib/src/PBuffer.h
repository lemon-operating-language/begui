#pragma once

#include "common.h"

class PBuffer
{
private:
	HPBUFFERARB	m_hpbuffer;      // Handle to a pbuffer.
    HDC			m_hdc;           // Handle to a device context.
    HGLRC		m_hglrc;         // Handle to a GL rendering context.
    int			m_width;         // Width of the pbuffer
    int			m_height;        // Height of the pbuffer
	bool		m_bShareContext;
	bool		m_bShareObjects;

public:
	PBuffer();
	virtual ~PBuffer();

	bool create(int w, int h, int pixel_format, bool bShareContext=false, bool bShareObjects=true);
	void free();

	HPBUFFERARB	getHPBuffer() const	{ return m_hpbuffer; }
	HDC			getDC()	const		{ return m_hdc; }
	HGLRC		getRenderingContext() const	{ return m_hglrc; }
	int			getWidth() const	{ return m_width; }
	int			getHeight() const	{ return m_height; }

	void makeCurrent();
};