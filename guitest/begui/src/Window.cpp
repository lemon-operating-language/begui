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

#include "Window.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Menu.h"
#include "util.h"

using namespace begui;

Window::Window() :
	m_bResizing(false),
	m_bMoving(false),
	m_bResizable(true),
	m_bMovable(true),
	m_pMenu(0),
	m_title("untitled"),
	m_style(SINGLE),
	m_clientArea(0,0,0,0),
	m_captionTextYPos(0),
	m_captionBarWidth(100),
	m_captionTextPadLeft(20),
	m_borderSize(0,0,0,0),
	m_state(VISIBLE),
	m_captionTextColor(0,0,0),
	m_bHasBorders(true),
	m_bHasCaption(true),
	m_bCanMinimize(false),
	m_bCanMaximize(false),
	m_bCanClose(true)
{
	setFixedZOrder(false);
}

Window::~Window()
{
	SAFE_DELETE(m_pMenu);
}

void Window::create(int left, int top, int width, int height, const std::string &title, 
					const std::string &style_name)
{
	m_left = left;
	m_top = top;
	m_right = m_left + width;
	m_bottom = m_top + height;
	m_title = title;
	m_state = VISIBLE;

	// load the window style
	ResourceManager::Style style = ResourceManager::inst()->getClassDef("Window").style(style_name);
	ASSERT(style.hasProp("window_bg"));
	m_windowFace = ResourceManager::inst()->loadImage(style.get_img("window_bg"));
	ASSERT(style.hasProp("caption"));
	m_captionFace = ResourceManager::inst()->loadImage(style.get_img("caption"));
	ASSERT(style.hasProp("window_resizable_area"));
	m_windowResizableArea = style.get_rect("window_resizable_area");
	ASSERT(style.hasProp("caption_resizable_area"));
	m_captionResizableArea = style.get_rect("caption_resizable_area");
	if (style.hasProp("window_active_area"))
		m_windowActiveArea = style.get_rect("window_active_area");
	else
		m_windowActiveArea = Rect<int>(0,0,m_windowFace.m_width, m_windowFace.m_height);
	if (style.hasProp("caption_active_area"))
		m_captionActiveArea = style.get_rect("caption_active_area");
	else
		m_captionActiveArea = Rect<int>(0,0,m_captionFace.m_width, m_captionFace.m_height);
	ASSERT(style.hasProp("caption_y_pos"));
	m_captionTextYPos = style.get_i("caption_y_pos");
	if (style.hasProp("caption_text_color"))
		m_captionTextColor = style.get_c("caption_text_color");
	else
		m_captionTextColor = Color(0,0,0);
	if (style.hasProp("border_size"))
		m_borderSize = style.get_rect("border_size");
	else
		m_borderSize = Rect<int>(0,0,0,0);
	
	// update client area
	calcClientArea();
	
	// create the window caption buttons
	if (m_bHasCaption) {
		if (m_bCanClose) {
			m_closeBtn.create(300, 0, "", 101, makeFunctor(*this, &Window::onCaptionBtn), "std_wnd_close_btn");
			Container::addComponent(&m_closeBtn);
		}
		if (m_bResizable) {
			if (m_bCanMaximize) {
				m_maxBtn.create(300, 0, "", 102, makeFunctor(*this, &Window::onCaptionBtn), "std_wnd_max_btn");
				Container::addComponent(&m_maxBtn);
			}
			if (m_bCanMinimize) {
				m_minBtn.create(300, 0, "", 103, makeFunctor(*this, &Window::onCaptionBtn), "std_wnd_min_btn");
				Container::addComponent(&m_minBtn);
			}
		}
	}
	
	// connect to the internal container
	m_contents.setParent(this);

	// custom initialization
	onCreate();
}

void Window::calcClientArea()
{
	// compute the client area of our window
	m_clientArea.top = (m_bHasCaption ? m_captionActiveArea.getHeight() : 0) + (m_bHasBorders ? m_borderSize.top:0);
	m_clientArea.left = m_bHasBorders ? m_borderSize.left : 0;
	m_clientArea.right = getWidth() - m_windowActiveArea.left 
		- (m_windowFace.m_width - m_windowActiveArea.right)
		- ((m_bHasBorders)? (m_borderSize.right) : 0) + 1;
	m_clientArea.bottom = getHeight() - m_windowActiveArea.top - (m_bHasBorders ? (m_borderSize.top + m_borderSize.bottom) : 0)
		- (m_bHasCaption ? m_captionActiveArea.getHeight() : 0);
}

void Window::setClientAreaSize(int cw, int ch)
{
	int new_r = cw + m_windowActiveArea.left + (m_windowFace.m_width - m_windowActiveArea.right) 
				+ (m_bHasBorders ? (m_borderSize.right) : 0) - 1;
	int new_b = ch + m_windowActiveArea.top 
				+ (m_bHasBorders ? (m_borderSize.top + m_borderSize.bottom) : 0) 
				+ (m_bHasCaption ? m_captionActiveArea.getHeight() : 0);
	setSize(new_r+m_clientArea.left, new_b+m_clientArea.top);
}

Menu* Window::createMainMenu()
{
	SAFE_DELETE(m_pMenu);
	m_pMenu = new Menu;
	m_pMenu->createMainMenu();
	addComponent(m_pMenu);
	return m_pMenu;
}

void Window::frameUpdate()
{
	Container::frameUpdate();
	
	// update client area
	calcClientArea();

	// update the dummy container
	m_contents.setPos(m_clientArea.left, m_clientArea.top);
	m_contents.setSize(m_clientArea.getWidth(), m_clientArea.getHeight());
	m_contents.frameUpdate();

	// update the size of the caption bar
	m_captionBarWidth = 3*getWidth()/4;
	int title_w = FontManager::getCurFont()->stringLength(m_title);
	if (m_captionBarWidth < title_w + 2*m_captionTextPadLeft)
		m_captionBarWidth = title_w + 2*m_captionTextPadLeft;
	if (m_captionBarWidth > m_windowActiveArea.getWidth())
		m_captionBarWidth = m_windowActiveArea.getWidth();

	// update the positions of the buttons
	int cw = m_closeBtn.getWidth();
	m_closeBtn.setPos(m_captionBarWidth - 3*cw/2, 4);
	m_maxBtn.setPos(m_captionBarWidth - 3*cw/2 - int(1.2*m_maxBtn.getWidth()), 4);
	m_minBtn.setPos(m_captionBarWidth - 3*cw/2 - int(1.2*m_maxBtn.getWidth()) 
		- int(1.2*m_minBtn.getWidth()), 4);
}

void Window::frameRender()
{
	int wnd_top = getTop() + ((m_bHasCaption)? m_captionActiveArea.getHeight() : 0);

	glEnable(GL_BLEND);

	// render the window caption
	glColor4f(1,1,1,1);
	if (m_bHasCaption) {
		Component::drawImageWtBorders(m_captionFace, getLeft() - m_captionActiveArea.left,
			getTop() - m_captionActiveArea.top,
			m_captionBarWidth + m_captionActiveArea.left + (m_captionFace.m_width - m_captionActiveArea.right),
			0,
			m_captionResizableArea);
		
		// render the caption title
		glColor4f(m_captionTextColor.r, m_captionTextColor.g, m_captionTextColor.b, 1);
		FontManager::getCurFont()->renderString(getLeft() + m_captionTextPadLeft, getTop() + m_captionTextYPos, m_title);
	}

	// render the window main area
	float alpha = (m_bMoving)?0.5f:1.0f;
	if (m_style == Window::MULTIPLE)
		glColor4f(0.5f,0.5f,0.5f,alpha);
	else
		glColor4f(1,1,1,alpha);
	if (m_bHasBorders) {
		Component::drawImageWtBorders(m_windowFace, getLeft() - m_windowActiveArea.left,
			wnd_top - m_windowActiveArea.top,
			getWidth(),
			getHeight() - ((m_bHasCaption)? m_captionActiveArea.getHeight() : 0),
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
	glTranslatef((float)getLeft(), (float)getTop(), 0.0f);

// TEMP: render the client area frame
/*glColor4f(1,0,0,1);
glBegin(GL_LINES);
	glVertex2f(m_clientArea.left, m_clientArea.top);
	glVertex2f(m_clientArea.right, m_clientArea.top);
	
	glVertex2f(m_clientArea.right, m_clientArea.top);
	glVertex2f(m_clientArea.right, m_clientArea.bottom);
	
	glVertex2f(m_clientArea.right, m_clientArea.bottom);
	glVertex2f(m_clientArea.left, m_clientArea.bottom);
	
	glVertex2f(m_clientArea.left, m_clientArea.bottom);
	glVertex2f(m_clientArea.left, m_clientArea.top);
glEnd();*/

	// render the contents of the sub-container
	m_contents.frameRender();

	// Render the modal dialog, if any
/*	if (m_pModalDialog)
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
	}*/
	
	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Window::onRender()
{
}

bool Window::onMouseDown(int x, int y, int button)
{
	Vector2i lP = parentToLocal(Vector2i(x,y));

	if (m_contents.onMouseDown(lP.x,lP.y,button))
		return true;
	if (Container::onMouseDown(x,y,button))
		return true;
	
	// if we got here, the internal components did not handle the event

	// Check if click was on the title bar
	// Check if click was on the border and the window is resizable
	//TODO

	// Click was on the empty space, and window is movable.
	// Start moving.
	if (lP.x>=0 && lP.x<m_captionBarWidth && lP.y>=0 && lP.y<m_captionActiveArea.getHeight())
		m_bMoving = true;
	return true;
}

bool Window::onMouseMove(int x, int y, int prevx, int prevy)
{
	// If click was on the title bar, drag window
	// If resizing, resize window
	
	if (m_bMoving && input::isMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		// Drag window around:
		setPos(m_left + (x-prevx), m_top + (y-prevy));
		return true;
	}

	Vector2i lP = parentToLocal(Vector2i(x,y));
	Vector2i lPrevP = parentToLocal(Vector2i(prevx,prevy));
	if (m_contents.onMouseMove(lP.x,lP.y,lPrevP.x,lPrevP.y))
		return true;
	if (Container::onMouseMove(x,y,prevx,prevy))
		return true;

	return false;
}

bool Window::onMouseUp(int x, int y, int button)
{
	// stop dragging or
	// ..stop resizing
	m_bResizing = false;
	m_bMoving = false;

	Vector2i lP = parentToLocal(Vector2i(x,y));

	Container::onMouseUp(x,y,button);
	return m_contents.onMouseUp(lP.x,lP.y,button);
}

bool Window::isPtInside(int x, int y)
{
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_left || x > m_right)
		return false;
	if (y < m_top || y > m_bottom)
		return false;
	if (y < m_captionActiveArea.getHeight() && x > m_captionBarWidth)
		return false;
	return true;
}


void Window::onKeyDown(int key)
{
	m_contents.onKeyDown(key);
}

void Window::onKeyUp(int key)
{
	m_contents.onKeyUp(key);
}

void Window::onCaptionBtn(int id)
{
	switch (id) {
		case 102:
			if (m_state == MAXIMIZED) {
				restore();
			}
			else {
				maximize();
			}
			break;
		case 103:
			minimize();
			break;
	}
}

void Window::minimize()
{
	m_state = MINIMIZED;
}

void Window::maximize()
{
	m_state = MAXIMIZED;
}

void Window::restore()
{
	m_state = VISIBLE;
}