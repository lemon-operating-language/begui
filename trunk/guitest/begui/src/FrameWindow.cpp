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

using namespace begui;

// Singleton instance
FrameWindow *FrameWindow::m_pInstance;

FrameWindow::FrameWindow() : 
	m_pMenu(0), 
	m_pModalDialog(0),
	m_style(MULTIPLE),
	m_bOwnDraw(true),
	m_title("untitled"),
	m_clientArea(0,0,0,0),
	m_captionTextYPos(0),
	m_captionBarWidth(100),
	m_captionTextPadLeft(20),
	m_borderSize(0,0,0,0),
	m_state(VISIBLE)
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
	m_state = VISIBLE;

	m_children.clear();

	// load the window style
	ResourceManager::Style style = ResourceManager::inst()->getClassDef("FrameWindow").style("std_owndraw");
	ASSERT(style.hasProp("window_bg"));
	m_windowFace = ResourceManager::inst()->loadImage(style.get_img("window_bg"));
	ASSERT(style.hasProp("caption"));
	m_captionFace = ResourceManager::inst()->loadImage(style.get_img("caption"));
	ASSERT(style.hasProp("window_resizable_area"));
	m_windowResizableArea = style.get_rect("window_resizable_area");
	ASSERT(style.hasProp("caption_resizable_area"));
	m_captionResizableArea = style.get_rect("caption_resizable_area");
	ASSERT(style.hasProp("window_active_area"));
	m_windowActiveArea = style.get_rect("window_active_area");
	ASSERT(style.hasProp("caption_active_area"));
	m_captionActiveArea = style.get_rect("caption_active_area");
	ASSERT(style.hasProp("caption_y_pos"));
	m_captionTextYPos = style.get_i("caption_y_pos");
	ASSERT(style.hasProp("caption_text_color"));
	m_captionTextColor = style.get_c("caption_text_color");
	if (style.hasProp("border_size"))
		m_borderSize = style.get_rect("border_size");
	else
		m_borderSize = Rect<int>(0,0,0,0);
	
	// update client area
	calcClientArea();

	// create the window caption buttons
	if (m_bOwnDraw) {
		m_closeBtn.create(300, 0, "", 101, makeFunctor(*this, &FrameWindow::onCaptionBtn), "std_wnd_close_btn");
		Container::addComponent(&m_closeBtn);
		m_maxBtn.create(300, 0, "", 102, makeFunctor(*this, &FrameWindow::onCaptionBtn), "std_wnd_max_btn");
		Container::addComponent(&m_maxBtn);
		m_minBtn.create(300, 0, "", 103, makeFunctor(*this, &FrameWindow::onCaptionBtn), "std_wnd_min_btn");
		Container::addComponent(&m_minBtn);
	}
	
	// create the main menu
	m_pMenu = new Menu();
	m_pMenu->createMainMenu();
	addComponent(m_pMenu);

	// connect to the internal container
	m_container.setParent(this);

	return true;
}

void FrameWindow::resize(int width, int height)
{
	m_right = m_left + width;
	m_bottom = m_top + height;
	
	// update client area
	calcClientArea();
}

void FrameWindow::resetViewport()
{
	// Setup and orthogonal, pixel-to-pixel projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(m_left, m_right, m_bottom, m_top, 0.0, 1.0);

	// Setup the viewport
	glViewport(m_left, m_top, getWidth(), getHeight());
}

void FrameWindow::calcClientArea()
{
	if (m_bOwnDraw) {
		// compute the client area of our window
		m_clientArea.top = m_captionActiveArea.bottom-m_captionActiveArea.top + m_borderSize.top;
		m_clientArea.left = m_borderSize.left;
		m_clientArea.right = getWidth() - m_windowActiveArea.left 
			- (m_windowFace.m_width - m_windowActiveArea.right)
			- m_borderSize.right + 1;
		m_clientArea.bottom = getHeight() - m_windowActiveArea.top - m_borderSize.top - m_borderSize.bottom;
	}
	else {
		// ISSUE: doesnt take into account the OS's borders, which should be subtracted from the 
		// width and height. Doesnt matter know, might need to be fixed later though.
		m_clientArea = Rect<int>(0,0,getWidth(),getHeight());
	}
}

void FrameWindow::frameUpdate()
{
	Container::frameUpdate();
	
	// update client area
	calcClientArea();

	// update the dummy container
	m_container.setPos(m_clientArea.left, m_clientArea.top);
	m_container.setSize(m_clientArea.getWidth(), m_clientArea.getHeight());
	m_container.frameUpdate();

	// update the size of the caption bar
	if (m_bOwnDraw) {
		m_captionBarWidth = m_captionFace.m_width;
		int title_w = FontManager::getCurFont()->stringLength(m_title);
		if (m_captionBarWidth < title_w + 2*m_captionTextPadLeft)
			m_captionBarWidth = title_w + 2*m_captionTextPadLeft;

		// update the positions of the buttons
		m_closeBtn.setPos(m_captionBarWidth - (m_captionFace.m_width - m_captionActiveArea.right)
						- m_captionActiveArea.left - m_closeBtn.getWidth(), 4);
		m_maxBtn.setPos(m_captionBarWidth - (m_captionFace.m_width - m_captionActiveArea.right)
						- m_captionActiveArea.left - m_closeBtn.getWidth() - m_maxBtn.getWidth() +2, 4);
		m_minBtn.setPos(m_captionBarWidth - (m_captionFace.m_width - m_captionActiveArea.right)
						- m_captionActiveArea.left - m_closeBtn.getWidth() - m_maxBtn.getWidth() 
						- m_minBtn.getWidth() +4, 4);
	}
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
}

void FrameWindow::onRender()
{
}

bool FrameWindow::onMouseDown(int x, int y, int button)
{
	if (m_pModalDialog)
	{
		m_pModalDialog->onMouseDown(x,y, button);
		return true;
	}
	// if contained controls dont handle the mouse event,
	// handle it from this control
	if (!m_container.onMouseDown(x,y,button))
		return Container::onMouseDown(x,y, button);
	return false;
}

bool FrameWindow::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_pModalDialog)
	{
		if (m_pModalDialog->isPtInside(x,y))
			m_pModalDialog->onMouseMove(x,y,prevx,prevy);
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
	if (m_pModalDialog)
	{
		m_pModalDialog->onMouseUp(x,y,button);
		return true;
	}
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

void FrameWindow::onCaptionBtn(int id)
{
	switch (id) {
		case 101:
			// close btn
			m_onClose(CLOSE);
			exit(0);
			break;
		case 102:
			if (m_state == MAXIMIZED) {
				m_state = VISIBLE;
				m_onRestore(RESTORE);
			}
			else {
				m_state = MAXIMIZED;
				m_onMaximize(MAXIMIZE);
			}
			break;
		case 103:
			m_state = MINIMIZED;
			m_onMinimize(MINIMIZE);
			break;
	}
}

void FrameWindow::setEventHook(begui::FrameWindow::Event evt, const Functor1<Event> &fun)
{
	switch (evt) {
		case FrameWindow::MINIMIZE: m_onMinimize = fun; break;
		case FrameWindow::MAXIMIZE: m_onMaximize = fun; break;
		case FrameWindow::RESTORE: m_onRestore = fun; break;
		case FrameWindow::CLOSE: m_onClose = fun; break;
	}
}