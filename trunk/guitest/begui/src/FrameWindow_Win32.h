#pragma once

#include "Window.h"
#include "FrameWindow.h"
#include "../../bcore/src/RenderPass.h"

namespace begui {

/**
 * FrameWindow_Win32: 
 *
 */
class FrameWindow_Win32 : public FrameWindow
{
	friend class FrameWindow;

private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	HDC			m_hDC;
	HGLRC		m_hRC;
	HDC			m_hMemDC;

	RenderPass m_frameRenderPass;

public:
	virtual ~FrameWindow_Win32();

	virtual void frameRender();
	virtual void setPos(int x, int y);
	virtual void setSize(int w, int h);
	virtual void minimize();
	virtual void maximize();
	virtual void restore();
	
	LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	FrameWindow_Win32();

	virtual void createGLWindow(int left, int top, int width, int height, const std::string &title, const Options &opt);
	virtual void initializeSubsystems();
	virtual void freeGLWindow();
};

};