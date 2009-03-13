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

using namespace begui;

Window::Window() :
	m_bResizing(false),
	m_bMoving(false),
	m_bResizable(true),
	m_bMovable(true),
	m_bLButtonDown(false)
{
	setFixedZOrder(false);
}

Window::~Window()
{
}

void Window::create(int left, int top, int width, int height, const std::string &title)
{
	m_left = left;
	m_top = top;
	m_right = m_left + width;
	m_bottom = m_top + height;
	m_title = title;

	// custom initialization
	onCreate();
}

void Window::onRender()
{
	// set the texture of a window
	Texture *pTex = ResourceManager::inst()->getStockMap(ResourceManager::WINDOW_RES);
	pTex->set();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render the window
	if (m_bMoving || m_bResizing)
		glColor4f(1,1,1,0.5);
	else
		glColor4f(1,1,1,1);
	double tW = 512;	// texture width;
	double tH = 512;	// texture width;
	int wtL = 0;		// window left in pixels in texture
	int wtR = 511;		// window right
	int wtT = 76;		// top
	int wtB = 504;		// bottom
	int offs = 32;	// the size of the border
	Component::drawBorderedQuad(0, 0, getWidth(), getHeight(),
								offs, offs, getWidth()-offs, getHeight()-offs,
								wtL/tW, (wtL+offs)/tW, wtR/tW, (wtR-offs)/tW,
								wtT/tH, (wtT+offs)/tH, wtB/tH, (wtB-offs)/tH);

	// render the window title
	int len = Font::stringLength(m_title) + 40;
	glColor4f(1,1,1, 0.95);
	int left = 12;
	int right = left + len;	//TODO: resize
	int top = -11;
	int bottom = top + 21;
	wtL = 1;		// window left in pixels in texture
	wtR = 140;		// window right
	wtT = 1;		// top
	wtB = 23;		// bottom
	offs = 6;	// the size of the border
	Component::drawBorderedQuad(left, top, right, bottom,
								left+offs, top+offs, right-offs, bottom-offs,
								wtL/tW, (wtL+offs)/tW, wtR/tW, (wtR-offs)/tW,
								wtT/tH, (wtT+offs)/tH, wtB/tH, (wtB-offs)/tH);

	// reset texture
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// render the title text
	glColor4f(1,1,1,1);
	Font::renderString(32, 3, m_title);
}

void Window::onMouseDownEx(int x, int y)
{
	// Check if click was on the title bar
	// Check if click was on the border and the window is resizable
	//TODO

	// Click was on the empty space, and window is movable.
	// Start moving.
	m_bLButtonDown = true;
}

void Window::onMouseMoveEx(int x, int y, int prevx, int prevy)
{
	// If click was on the title bar, drag window
	// If resizing, resize window
	
	if (m_bMovable && m_bLButtonDown)
	{
		if (x-prevx!=0 || y-prevy!=0)
			m_bMoving = true;

		// Drag window around:
		m_left = m_left + (x - prevx);
		m_right = m_right + (x - prevx);
		m_top = m_top + (y - prevy);
		m_bottom = m_bottom + (y - prevy);
	}
}

void Window::onMouseUpEx(int x, int y)
{
	// stop dragging or
	// ..stop resizing
	m_bResizing = false;
	m_bMoving = false;
	m_bLButtonDown = false;
}

bool Window::isPtInside(int x, int y)
{
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_left+12 || x > m_right-14)
		return false;
	if (y < m_top+12 || y > m_bottom-14)
		return false;
	return true;
}