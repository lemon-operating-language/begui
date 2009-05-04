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
#include "ResourceManager.h"
#include "util.h"

using namespace begui;

ScrollBar::ScrollBar() : m_scrollDir(SCROLL_VERTICAL),
	m_minPos(0), m_maxPos(1), m_curPos(0),
	m_sliderDragStart(-1),
	m_percVisible(-1),
	m_sliderLen(20),
	m_barSize(18),
	m_buttonWidth(18),
	m_buttonHeight(18),
	m_sliderWidth(18),
	m_sliderOffs1(0),
	m_sliderOffs2(0)
{
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::create(int x, int y, int length, ScrollDir dir, double minPos, double maxPos,
						const std::string &style_name)
{
	// set the dimensions of the component
	m_left = x;
	m_top = y;
	if (dir == ScrollBar::SCROLL_HORIZONTAL)
	{
		m_right = x+length;
		m_bottom = y+m_barSize;
	}
	else
	{
		m_right = x+m_barSize;
		m_bottom = y+length;
	}

	// set properties
	m_scrollDir = dir;
	m_minPos = minPos;
	m_maxPos = maxPos;
	m_curPos = minPos;

	// get the style of the scrollbar
	ResourceManager::Style barstyle = ResourceManager::inst()->getClassDef("ScrollBar").style(style_name);
	m_barBg = ResourceManager::inst()->loadImage(barstyle.get_img("bg"));

	// create the buttons
	if (dir == ScrollBar::SCROLL_HORIZONTAL) {
		ResourceManager::Style bstyle = ResourceManager::inst()->getClassDef("Button").style("scroller_btn_left");
		m_incBtn.create(-bstyle.get_i("padding_left"), -bstyle.get_i("padding_top"), 
						"", 101, makeFunctor(*this, &ScrollBar::handleClick), "scroller_btn_left");

		bstyle = ResourceManager::inst()->getClassDef("Button").style("scroller_btn_right");
		m_decBtn.create(m_right+bstyle.get_i("padding_right")-bstyle.get_i("default_width"), 
						-bstyle.get_i("padding_top"), 
						"", 101, makeFunctor(*this, &ScrollBar::handleClick), "scroller_btn_right");

		m_slider.create(0, 0, "", 103, Functor1<int>(), "scroller_slider");
	}
	else {
		int xCenter = 0;
		ResourceManager::Style bstyle = ResourceManager::inst()->getClassDef("Button").style("scroller_btn_up");
		m_sliderOffs1 = bstyle.get_i("padding_bottom");
		m_incBtn.create(-bstyle.get_i("padding_left"), -bstyle.get_i("padding_top"), 
						"", 101, makeFunctor(*this, &ScrollBar::handleClick), "scroller_btn_up");
		
		// use the top button to define the center of the scroll bar
		xCenter = bstyle.get_i("default_width")/2 - bstyle.get_i("padding_left");

		bstyle = ResourceManager::inst()->getClassDef("Button").style("scroller_btn_down");
		m_sliderOffs2 = -bstyle.get_i("padding_top");
		m_decBtn.create(-bstyle.get_i("padding_left"), 
						getHeight()+bstyle.get_i("padding_bottom")-bstyle.get_i("default_height"), 
						"", 102, makeFunctor(*this, &ScrollBar::handleClick), "scroller_btn_down");
		
		bstyle = ResourceManager::inst()->getClassDef("Button").style("scroller_slider");
		m_sliderOffs1 += bstyle.get_i("padding_top");
		m_sliderOffs2 += -bstyle.get_i("padding_bottom");
		m_slider.create(xCenter-(bstyle.get_i("default_width")/2-bstyle.get_i("padding_left")), 
						-m_sliderOffs1, 
						"", 
						103, 
						Functor1<int>(), "scroller_slider");
	}
	addComponent(&m_incBtn);
	addComponent(&m_decBtn);
	addComponent(&m_slider);
/*	if (dir == ScrollBar::SCROLL_HORIZONTAL)
		m_decBtn.create(m_right,y, "", 101, Functor1<int>(), "std");
	else
		m_decBtn.create(x,m_bottom, "", 101, Functor1<int>(), "std");
	m_slider.create(x,y, "", 101, Functor1<int>(), "std");*/
}

void ScrollBar::onUpdate()
{
	// update the position and size of the slider
	int runArea = 0;
	if (m_scrollDir == ScrollBar::SCROLL_VERTICAL) {
		runArea = m_decBtn.getBottom() - m_incBtn.getTop();
	}
	else {
		runArea = m_decBtn.getRight() - m_incBtn.getLeft();
	}

	if (m_percVisible > 1)
		m_percVisible = 1;
	if (m_percVisible > 0)
		m_sliderLen = runArea * m_percVisible;
	else
		m_sliderLen = (runArea > 20) ? 20 : 0.8*runArea;

	m_sliderLen += m_sliderOffs1-m_sliderOffs2;

	if (m_scrollDir == ScrollBar::SCROLL_VERTICAL) {
		m_slider.setPos(m_slider.getLeft(), m_incBtn.getBottom() + (runArea - m_sliderLen/2)*m_curPos - m_sliderOffs1);
		m_slider.setSize(m_slider.getWidth(), m_sliderLen);
	}
	else {
	}

	Container::onUpdate();
}

void ScrollBar::onRender()
{
	Component::drawImage(m_barBg, (m_incBtn.getWidth() - m_barBg.m_width)/2 + m_incBtn.getLeft()+1 , 
					 m_incBtn.getBottom() - m_incBtn.getHeight()/2, 
					 m_barBg.m_width, getHeight());
}
	
void ScrollBar::onMouseDown(int x, int y, int button)
{
/*	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;

	x-=m_left;
	y-=m_top;

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
		m_curPos = m_maxPos;*/

	Container::onMouseDown(x,y,button);
}

void ScrollBar::onMouseMove(int x, int y, int prevx, int prevy)
{
/*	int w = SCROLL_WIDTH-2;
	int h = SCROLL_WIDTH-2;
	
	x-=m_left;
	y-=m_top;
	prevx-=m_left;
	prevy-=m_top;
	
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
	}*/
	Container::onMouseMove(x,y,prevx,prevy);
}

void ScrollBar::onMouseUp(int x, int y, int button)
{
	m_sliderDragStart = -1;

	Container::onMouseUp(x,y,button);
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

	Container::onKeyDown(key);
}

void ScrollBar::onKeyUp(int key)
{
	Container::onKeyUp(key);
}

void ScrollBar::setBounds(double minPos, double maxPos, double percVisible)
{
	if (m_curPos < minPos) m_curPos = minPos;
	if (m_curPos > maxPos) m_curPos = maxPos;
	m_minPos = minPos;
	m_maxPos = maxPos;
	m_percVisible = percVisible;
}

void ScrollBar::handleClick(int id)
{
	double pageStep = (m_maxPos - m_minPos)/5;
	if (m_percVisible > 0)
		pageStep = (m_maxPos - m_minPos)*m_percVisible;

	switch (id) {
		case 101:
			m_curPos -= pageStep/STEPS_PER_PAGE;
			if (m_curPos < m_minPos)
				m_curPos = m_minPos;
			break;
		case 102:
			m_curPos += pageStep/STEPS_PER_PAGE;
			if (m_curPos > m_maxPos)
				m_curPos = m_maxPos;
			break;
	}
}