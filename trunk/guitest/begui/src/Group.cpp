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

#include "Group.h"
#include "Font.h"
#include "ResourceManager.h"

using namespace begui;

Group::Group() : m_frameStyle(Group::ROUNDED),
	m_frameColor(0.3, 0.3, 0.3)
{
}

void Group::create(int left, int top, int width, int height, const std::string &title)
{
	m_left = left;
	m_top = top;
	m_right = m_left+width;
	m_bottom = m_top + height;
	m_title = title;
}

void Group::onRender()
{
	// set the texture of a window
	Texture *pTex = ResourceManager::inst()->getStockMap(ResourceManager::STD_CONTROLS);
	pTex->set();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double ul=382;
	double ut=34;
	double ur=406;
	double ub=58;
	double tw=512;
	double th=512;
	int left = 0, top=0;	//NOTE: rendering is done in LOCAL coordinate system!!
	int right = m_right-m_left, bottom=m_bottom-m_top;
	glColor4f(0,0,0,0.17);
	Component::drawBorderedQuad(left, top, right, bottom,
						left+4, top+4, right-4, bottom-4,
						ul/tw, (ul+4)/tw, ur/tw, (ur-4)/tw,
						ut/th, (ut+4)/th, ub/th, (ub-4)/th);

	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	int center = left + getWidth()/2;
	glColor4f(0.2,0.2,0.2,0.5f);
	Font::renderString(center - Font::stringLength(m_title)/2, top + 11, m_title);
	
	glDisable(GL_BLEND);
}


bool Group::isPtInside(int x, int y)
{
	if (x<m_left || x>m_right)
		return false;
	if (y<m_top || y>m_bottom)
		return false;
	return true;
}