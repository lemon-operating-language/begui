/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of BeGUI library.
//
//    BeGUI is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    BeGUI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with BeGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FrameWindow.h"
#include "ResourceManager.h"
#include "Font.h"
#include "util.h"
#ifdef _WIN32
	#include "FrameWindow_Win32.h"
#endif

using namespace begui;

// static singleton instance
FrameWindow *FrameWindow::m_pInst = 0;

int g_lastXPos = 0;
int g_lastYPos = 0;

FrameWindow::FrameWindow()
{
	// default style for a frame window
	setStyle(Window::MULTIPLE);
}

FrameWindow::~FrameWindow()
{
	free();
}

FrameWindow* FrameWindow::inst()
{
	if (m_pInst)
		return m_pInst;
#ifdef _WIN32
	m_pInst = new FrameWindow_Win32;
#endif
	ASSERT(m_pInst);
	return m_pInst;
}

void FrameWindow::create(int left, int top, int width, int height, const std::string &title, const Options *opt,
						 const std::string &style_name)
{
	// set the various options for the created window
	if (opt) {
		m_options = *opt;
	}
	else {
		m_options.bOwnDraw = true;
		m_options.bFullScreen = false;
		m_options.nColorBits = 16;
		m_options.nDepthBits = 16;
		m_options.nStencilBits = 0;
	}

	// create the OpenGL window
	try {
		createGLWindow(left, top, width, height, title, m_options);
	}
	catch (std::exception e) {
		Console::error("Failed to create OpenGL window: " + (std::string)e.what() + "\n");
		return;
	}

	// only if the window is own-drawn does it have borders and caption
	m_bHasBorders = m_bHasCaption = m_options.bOwnDraw;
	m_bMovable = m_options.bOwnDraw;

	m_bCanMinimize = true;
	m_bCanMaximize = true;

	Window::create(left, top, width, height, title, style_name);
}

void FrameWindow::free()
{
	// release the OpenGL window
	freeGLWindow();
}

void FrameWindow::frameRender()
{
	resetViewport();

	Window::frameRender();
}

void FrameWindow::setPos(int x, int y)
{
	Window::setPos(x,y);
}

void FrameWindow::setSize(int w, int h)
{
	Window::setSize(w,h);
}

void FrameWindow::resetViewport()
{
	// Setup and orthogonal, pixel-to-pixel projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(m_left, m_right, m_bottom, m_top, 0.0, 1.0);

	// Setup the viewport
	if (m_options.bOwnDraw)
		glViewport(0,0,getWidth(),getHeight());
	else
		glViewport(m_left, -m_top, getWidth(), getHeight());
}

void FrameWindow::showModalDialog(begui::Dialog *dlg)
{
}

void FrameWindow::closeModalDialog()
{
}

bool FrameWindow::onMouseDown(int x, int y, int button)
{
	if (m_options.bOwnDraw) {
		x += m_left;
		y += m_top;
		g_lastXPos = x;
		g_lastYPos = y;
	}
	return Window::onMouseDown(x,y,button);
}

bool FrameWindow::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_options.bOwnDraw) {
		x += m_left;
		y += m_top;
		prevx = g_lastXPos;
		prevy = g_lastYPos;
		g_lastXPos = x;
		g_lastYPos = y;
	}
	return Window::onMouseMove(x,y,prevx,prevy);
}

bool FrameWindow::onMouseUp(int x, int y, int button)
{
	if (m_options.bOwnDraw) {
		x += m_left;
		y += m_top;
	}
	return Window::onMouseUp(x,y,button);
}

void FrameWindow::calcClientArea()
{
	if (m_options.bOwnDraw) {
		Window::calcClientArea();
	}
	else {
		// ISSUE: doesnt take into account the OS's borders, which should be subtracted from the 
		// width and height. Doesnt matter know, might need to be fixed later though.
		m_clientArea = Rect<int>(0,0,getWidth(),getHeight());
	}
}


void FrameWindow::onCaptionBtn(int id)
{
	switch (id) {
		case 101:
			// close btn
			//m_onClose(CLOSE);
			exit(0);
			break;
		default:
			Window::onCaptionBtn(id);
	}
}

/*
// Singleton instance
FrameWindow *FrameWindow::m_pInstance;

FrameWindow::FrameWindow() : 
	m_pMenu(0), 
	m_pModalDialog(0),
	m_bOwnDraw(true),
{
}

FrameWindow::~FrameWindow()
{
	SAFE_DELETE(m_pMenu);
}

bool FrameWindow::create(int width, int height, const std::string &title, Style wnd_style, bool bOwnDraw)
{
	m_left = 0;
	m_top = 0;
	m_right = width;
	m_bottom = height;
	m_title = title;
	m_style = wnd_style;
	m_bOwnDraw = bOwnDraw;

	m_children.clear();

	

	
	
	// create the main menu
	m_pMenu = new Menu();
	m_pMenu->createMainMenu();
	addComponent(m_pMenu);


	return true;
}

void FrameWindow::resize(int width, int height)
{
	m_right = m_left + width;
	m_bottom = m_top + height;
	
	// update client area
	calcClientArea();
}


void FrameWindow::frameUpdate()
{
	
}

void FrameWindow::frameRender()
{
	resetViewport();

	int wnd_top = getTop();

	if (m_bOwnDraw) {
		wnd_top += m_captionActiveArea.bottom - m_captionActiveArea.top;

		// render the window caption
		glColor4f(1,1,1,1);
		Component::drawImageWtBorders(m_captionFace, getLeft() - m_captionActiveArea.left,
			getTop() - m_captionActiveArea.top,
			m_captionBarWidth,
			0,
			m_captionResizableArea);
		
		// render the caption title
		glColor4f(m_captionTextColor.r, m_captionTextColor.g, m_captionTextColor.b, 1);
		FontManager::getCurFont()->renderString(getLeft() + m_captionTextPadLeft, getTop() + m_captionTextYPos, m_title);
	}

	// render the window main area
	if (m_style == FrameWindow::MULTIPLE)
		glColor4f(0.5f,0.5f,0.5f,1);
	else
		glColor4f(1,1,1,1);
	if (m_bOwnDraw) {
		Component::drawImageWtBorders(m_windowFace, getLeft() - m_windowActiveArea.left,
			wnd_top - m_windowActiveArea.top,
			getWidth(),// + (m_windowActiveArea.left + m_windowFace.m_width - m_windowActiveArea.right),
			getHeight() // + (m_windowActiveArea.top + m_windowFace.m_height - m_windowActiveArea.bottom)
						- (m_captionActiveArea.bottom - m_captionActiveArea.top),
			m_windowResizableArea);
	}
	else {
		Component::drawImage(m_windowFace, getLeft()-m_windowResizableArea.left,
			wnd_top - m_windowResizableArea.top,
			getWidth() + (m_windowResizableArea.left + m_windowFace.m_width - m_windowResizableArea.right),
			getHeight() + (m_windowResizableArea.top + m_windowFace.m_height - m_windowResizableArea.bottom));
	}

	// render the contents of the window
	Container::frameRender();

	// setup the translation
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(getLeft(), getTop(), 0.0f);

	// render the contents of the sub-container
	m_container.frameRender();

	// Render the modal dialog, if any
	if (m_pModalDialog)
	{
		// Render a blended fs quad to darken the bg
		glEnable(GL_BLEND);
		glColor4f(0.3f,0.3f,0.31f,0.6f);
		glBegin(GL_QUADS);
			glVertex3f((GLfloat)m_left, (GLfloat)m_top, 0);
			glVertex3f((GLfloat)m_right, (GLfloat)m_top, 0);
			glVertex3f((GLfloat)m_right, (GLfloat)m_bottom, 0);
			glVertex3f((GLfloat)m_left, (GLfloat)m_bottom, 0);
		glEnd();
		glDisable(GL_BLEND);

		m_pModalDialog->frameRender();
	}
	
	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void FrameWindow::onRender()
{
}

bool FrameWindow::onMouseDown(int x, int y, int button)
{
	Vector2i lP = parentToLocal(Vector2i(x,y));
	x = lP.x;
	y = lP.y;

	if (m_pModalDialog)
	{
		m_pModalDialog->onMouseDown(x,y, button);
		return true;
	}
	
	// if contained controls dont handle the mouse event,
	// handle it from this control
	if (!m_container.onMouseDown(x,y,button))
		if (!Container::onMouseDown(x,y, button)) {
			// check if the caption was clicked
			if (m_bOwnDraw && button == MOUSE_BUTTON_LEFT && m_captionActiveArea.contains(lP.x,lP.y)) {
				m_bDragging = true;
			}
			return false;
		}
	return true;
}

bool FrameWindow::onMouseMove(int x, int y, int prevx, int prevy)
{
	Vector2i lP = parentToLocal(Vector2i(x,y));
	Vector2i lPp = parentToLocal(Vector2i(prevx,prevy));
	x = lP.x;
	y = lP.y;
	prevx = lPp.x;
	prevy = lPp.y;

	if (m_pModalDialog)
	{
		if (m_pModalDialog->isPtInside(x,y))
			m_pModalDialog->onMouseMove(x,y,prevx,prevy);
		return true;
	}
	
	// handle dragging of the window
	if (m_bDragging && input::isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		setPos(getLeft()+(x-prevx), getTop()+(y-prevy));
		Console::print("new pos = %d, %d\n", getLeft(), getTop());
		return true;
	}

	// if contained controls dont handle the mouse event,
	// handle it from this control
	if (!m_container.onMouseMove(x,y,prevx,prevy))
		return Container::onMouseMove(x,y,prevx,prevy);
	return false;
}

bool FrameWindow::onMouseUp(int x, int y, int button)
{
	Vector2i lP = parentToLocal(Vector2i(x,y));
	x = lP.x;
	y = lP.y;

	if (m_pModalDialog)
	{
		m_pModalDialog->onMouseUp(x,y,button);
		return true;
	}
	
	m_bDragging = false;

	// if contained controls dont handle the mouse event,
	// handle it from this control
	if (!m_container.onMouseUp(x,y,button))
		return Container::onMouseUp(x,y,button);
	return false;
}

void FrameWindow::onKeyDown(int key)
{
	if (m_pModalDialog)
	{
		m_pModalDialog->onKeyDown(key);
		return;
	}
	Container::onKeyDown(key);
}

void FrameWindow::onKeyUp(int key)
{
	if (m_pModalDialog)
	{
		m_pModalDialog->onKeyUp(key);
		return;
	}
	Container::onKeyUp(key);
}

void FrameWindow::showModalDialog(Dialog *dlg)
{
	ASSERT(dlg);
	m_pModalDialog = dlg;
}

void FrameWindow::closeModalDialog()
{
	m_pModalDialog = 0;
}

void FrameWindow::setEventHook(begui::FrameWindow::Event evt, const Functor1<Event> &fun)
{
	switch (evt) {
		case FrameWindow::MINIMIZE: m_onMinimize = fun; break;
		case FrameWindow::MAXIMIZE: m_onMaximize = fun; break;
		case FrameWindow::RESTORE: m_onRestore = fun; break;
		case FrameWindow::CLOSE: m_onClose = fun; break;
	}
}*/