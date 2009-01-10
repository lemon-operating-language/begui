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

#include "ScrollBar.h"
#include "WindowResourceManager.h"
#include "util.h"

using namespace begui;

ScrollBar::ScrollBar() : m_scrollDir(SCROLL_VERTICAL),
	m_minPos(0), m_maxPos(1), m_curPos(0),
	m_sliderDragStart(-1),
	m_percVisible(-1),
	m_sliderLen(20)
{
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::create(int x, int y, int length, ScrollDir dir, double minPos, double maxPos)
{
	// set the dimensions of the component
	m_left = x;
	m_top = y;
	if (dir == ScrollBar::SCROLL_HORIZONTAL)
	{
		m_right = x+length;
		m_bottom = y+SCROLL_WIDTH;
	}
	else
	{
		m_right = x+SCROLL_WIDTH;
		m_bottom = y+length;
	}

	// set properties
	m_scrollDir = dir;
	m_minPos = minPos;
	m_maxPos = maxPos;
	m_curPos = minPos;
}

void ScrollBar::frameUpdate()
{
	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;

	// update the size of the slider
	int runArea = 0;
	if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		runArea = getHeight()-2*h-4;
	else
		runArea = getWidth()-2*w-4;
	if (m_percVisible > 1)
		m_percVisible = 1;
	if (m_percVisible > 0)
		m_sliderLen = runArea * m_percVisible;
	else
		m_sliderLen = (runArea > 20) ? 20 : 0.8*runArea;
}

void ScrollBar::frameRender()
{
	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;
	float tX = 332.0/512;
	float tY = 37.0/512;
	float tX2 = tX + 19.0/512;
	float tY2 = tY + 19.0/512;

	// render the background
	glColor4f(0,0.05,0.1,0.3);
	glBegin(GL_QUADS);
		glVertex2f(m_left, m_top);
		glVertex2f(m_right, m_top);
		glVertex2f(m_right, m_bottom);
		glVertex2f(m_left, m_bottom);
	glEnd();
	
	// set the texture of a window
	Texture *pTex = WindowResourceManager::inst()->getChildWindowImage();
	pTex->set();

	// render the buttons
	glBegin(GL_QUADS);
		if (m_curPos > m_minPos)
			glColor4f(1,1,1,1);
		else
			glColor4f(1,1,1,0.3);
		
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			glTexCoord2f(tX, tY2); glVertex2f(m_left+1, m_top+1);
			glTexCoord2f(tX2, tY2); glVertex2f(m_left+1+w, m_top+1);
			glTexCoord2f(tX2, tY); glVertex2f(m_left+1+w, m_top+1+h);
			glTexCoord2f(tX, tY); glVertex2f(m_left+1, m_top+1+h);
		}
		else
		{
			glTexCoord2f(tX, tY2); glVertex2f(m_left+1, m_top+1);
			glTexCoord2f(tX, tY); glVertex2f(m_left+1+w, m_top+1);
			glTexCoord2f(tX2, tY); glVertex2f(m_left+1+w, m_top+1+h);
			glTexCoord2f(tX2, tY2); glVertex2f(m_left+1, m_top+1+h);
		}
		
		if (m_curPos < m_maxPos)
			glColor4f(1,1,1,1);
		else
			glColor4f(1,1,1,0.3);
		
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			glTexCoord2f(tX, tY); glVertex2f(m_left+1, m_bottom-h-1);
			glTexCoord2f(tX2, tY); glVertex2f(m_left+1+w, m_bottom-h-1);
			glTexCoord2f(tX2, tY2); glVertex2f(m_left+1+w, m_bottom-1);
			glTexCoord2f(tX, tY2); glVertex2f(m_left+1, m_bottom-1);
		}
		else
		{
			glTexCoord2f(tX2, tY); glVertex2f(m_right-w-1, m_top+1);
			glTexCoord2f(tX2, tY2); glVertex2f(m_right-1, m_top+1);
			glTexCoord2f(tX, tY2); glVertex2f(m_right-1, m_top+h+1);
			glTexCoord2f(tX, tY); glVertex2f(m_right-w-1, m_top+h+1);
		}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	// render the slider
	float sX = 355.0/512;
	float sY = 33.0/512;
	float sX2 = sX + 26.0/512;
	float sY2 = sY + 26.0/512;

	double sliderPos = (m_curPos-m_minPos)/(m_maxPos - m_minPos);
	glColor4f(1,1,1,0.5);
	glBegin(GL_QUADS);
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			glTexCoord2f(sX, sY2); glVertex2f(m_left+1, m_top+h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos);
			glTexCoord2f(sX2, sY2); glVertex2f(m_left+1+w, m_top+h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos);
			glTexCoord2f(sX2, sY); glVertex2f(m_left+1+w, m_top+h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos + m_sliderLen);
			glTexCoord2f(sX, sY); glVertex2f(m_left+1, m_top+h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos + m_sliderLen);
		}
		else
		{
			glTexCoord2f(sX, sY2); glVertex2f(m_left+w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos, m_top+1);
			glTexCoord2f(sX2, sY2); glVertex2f(m_left+w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos, m_top+h+1);
			glTexCoord2f(sX2, sY); glVertex2f(m_left+w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos + m_sliderLen, m_top+h+1);
			glTexCoord2f(sX, sY); glVertex2f(m_left+w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos + m_sliderLen, m_top+1);
		}
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}
	
void ScrollBar::onMouseDown(int x, int y, int button)
{
	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;

	double sliderPos = (m_curPos-m_minPos)/(m_maxPos - m_minPos);
	
	// adding (m_maxPos - m_minPos)*m_percVisible to m_curPos would
	// take us to the next page of content. Use that.
	double pageStep = (m_maxPos - m_minPos)/5;
	if (m_percVisible > 0)
		pageStep = (m_maxPos - m_minPos)*m_percVisible;

	if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
	{
		if (x <= 0 || x > w)
			return;

		if (y > 0 && y < h)
		{
			// up arrow clicked
			m_curPos -= pageStep/STEPS_PER_PAGE;
		}
		else if (y > getHeight()-h-2 && y < getHeight()-1)
		{
			// down arrow clicked
			m_curPos += pageStep/STEPS_PER_PAGE;
		}
		else if (y >= h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos &&
			y <= h+2 + (getHeight()-2*h-4-m_sliderLen)*sliderPos + m_sliderLen)
		{
			m_sliderDragStart = y;
			m_sliderDragStartVal = m_curPos;
		}
		else
		{
			if (y > h+1 && y < getHeight()-h-1)
			{
				if (y < h+1+(getHeight()-2*h-2)*sliderPos)
					m_curPos -= pageStep;
				else
					m_curPos += pageStep;
			}
		}
	}
	else if (m_scrollDir == ScrollBar::SCROLL_HORIZONTAL)
	{
		if (y <= 0 || y > h)
			return;

		if (x > 0 && x < w)
		{
			// left arrow clicked
			m_curPos -= pageStep/STEPS_PER_PAGE;
		}
		else if (x > getWidth()-w-2 && x < getWidth()-1)
		{
			// right arrow clicked
			m_curPos += pageStep/STEPS_PER_PAGE;
		}
		else if (x >= w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos &&
			x <= w+2 + (getWidth()-2*w-4-m_sliderLen)*sliderPos + m_sliderLen)
		{
			m_sliderDragStart = x;
			m_sliderDragStartVal = m_curPos;
		}
		else
		{
			if (x > w+1 && x < getWidth()-w-1)
			{
				if (x < w+1+(getWidth()-2*w-2)*sliderPos)
					m_curPos -= pageStep;
				else
					m_curPos += pageStep;
			}
		}
	}
	
	if (m_curPos < m_minPos)
		m_curPos = m_minPos;
	if (m_curPos > m_maxPos)
		m_curPos = m_maxPos;
}

void ScrollBar::onMouseMove(int x, int y, int prevx, int prevy)
{
	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;
	
	if (m_sliderDragStart != -1)
	{
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			if (x <= -20 || x > w+20) {
				m_sliderDragStart = -1;
				m_curPos = m_sliderDragStartVal;
				return;
			}

			m_curPos = m_sliderDragStartVal + (m_maxPos - m_minPos)*(y - m_sliderDragStart)/(getHeight() - 2*h - 4 - m_sliderLen);
			if (m_curPos < m_minPos)
				m_curPos = m_minPos;
			if (m_curPos > m_maxPos)
				m_curPos = m_maxPos;
		}
		else
		{
			if (y <= -20 || y > h+20) {
				m_sliderDragStart = -1;
				m_curPos = m_sliderDragStartVal;
				return;
			}

			m_curPos = m_sliderDragStartVal + (m_maxPos - m_minPos)*(x - m_sliderDragStart)/(getWidth() - 2*w - 4 - m_sliderLen);
			if (m_curPos < m_minPos)
				m_curPos = m_minPos;
			if (m_curPos > m_maxPos)
				m_curPos = m_maxPos;
		}
	}
}

void ScrollBar::onMouseUp(int x, int y, int button)
{
	m_sliderDragStart = -1;
}

void ScrollBar::onKeyDown(int key)
{
	// adding (m_maxPos - m_minPos)*m_percVisible to m_curPos would
	// take us to the next page of content. Use that.
	double pageStep = (m_maxPos - m_minPos)/5;
	if (m_percVisible > 0)
		pageStep = (m_maxPos - m_minPos)*m_percVisible;

	switch (key) {
	case KEY_UP:
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			m_curPos -= pageStep/STEPS_PER_PAGE;
			if (m_curPos < m_minPos)
				m_curPos = m_minPos;
		}
		break;
	case KEY_DOWN:
		if (m_scrollDir == ScrollBar::SCROLL_VERTICAL)
		{
			m_curPos += pageStep/STEPS_PER_PAGE;
			if (m_curPos > m_maxPos)
				m_curPos = m_maxPos;
		}
		break;
	case KEY_LEFT:
		if (m_scrollDir == ScrollBar::SCROLL_HORIZONTAL)
		{
			m_curPos -= pageStep/STEPS_PER_PAGE;
			if (m_curPos < m_minPos)
				m_curPos = m_minPos;
		}
		break;
	case KEY_RIGHT:
		if (m_scrollDir == ScrollBar::SCROLL_HORIZONTAL)
		{
			m_curPos += pageStep/STEPS_PER_PAGE;
			if (m_curPos > m_maxPos)
				m_curPos = m_maxPos;
		}
		break;
	case KEY_PAGEUP:
		m_curPos -= pageStep;
		if (m_curPos < m_minPos)
			m_curPos = m_minPos;
		break;
	case KEY_PAGEDOWN:
		m_curPos += pageStep;
		if (m_curPos > m_maxPos)
			m_curPos = m_maxPos;
		break;
	case KEY_HOME:
		m_curPos = m_minPos;
		break;
	case KEY_END:
		m_curPos = m_maxPos;
		break;
	}
}

void ScrollBar::onKeyUp(int key)
{
}

bool ScrollBar::isPtInside(int x, int y)
{
	if (x < m_left || x > m_right)
		return false;
	if (y < m_top || y > m_bottom)
		return false;
	return true;
}
