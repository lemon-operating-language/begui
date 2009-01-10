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

#include "Component.h"
#include "util.h"

using namespace begui;

Component::Component() : m_left(0), m_right(0), m_top(0), m_bottom(0),
	m_pParent(0),
	m_bAlwaysOnTop(false),
	m_bVisible(true),
	m_bHasMouseFocus(false),
	m_bHasKeybFocus(false),
	m_bActive(false),
	m_bFixedZOrder(true),
	m_bEnabled(true)
{
}

// Coordinate transformation
Vector2	Component::worldToLocal(const Vector2& v) const
{
	Component *parent = m_pParent;
	Vector2 pos(v.x-m_left, v.y-m_top);
	while (parent) {
		pos.x -= parent->m_left;
		pos.y -= parent->m_top;
		parent = parent->m_pParent;
	}
	return pos;
}

Vector2	Component::parentToLocal(const Vector2& v) const
{
	return Vector2(v.x-m_left, v.y-m_top);
}

Vector2 Component::localToWorld(const Vector2& v) const
{
	Component *parent = m_pParent;
	Vector2 pos(v.x+m_left, v.y+m_top);
	while (parent) {
		pos.x += parent->m_left;
		pos.y += parent->m_top;
		parent = parent->m_pParent;
	}
	return pos;
}

Vector2 Component::localToParent(const Vector2& v) const
{
	return Vector2(v.x+m_left, v.y+m_top);
}

void Component::drawBorderedQuad(int l, int t, int r, int b,		// position to appear on screen
								 int lB, int tB, int rB, int bB,				// border positions
						double texL, double texLB, double texR, double texRB,	// left to right positions in tex
						double texT, double texTB, double texB, double texBB)	// top to bottom
{	
	glBegin(GL_QUADS);
		// top-left corner
		glTexCoord2f(texL, texT);	glVertex3f(l, t, 0);
		glTexCoord2f(texLB, texT);	glVertex3f(lB, t, 0);
		glTexCoord2f(texLB, texTB);	glVertex3f(lB, tB, 0);
		glTexCoord2f(texL, texTB);	glVertex3f(l, tB, 0);
		
		// top border
		glTexCoord2f(texLB, texT);	glVertex3f(lB, t, 0);
		glTexCoord2f(texRB, texT);	glVertex3f(rB, t, 0);
		glTexCoord2f(texRB, texTB);	glVertex3f(rB, tB, 0);
		glTexCoord2f(texLB, texTB);	glVertex3f(lB, tB, 0);

		// top-right corner
		glTexCoord2f(texRB, texT);	glVertex3f(rB, t, 0);
		glTexCoord2f(texR, texT);	glVertex3f(r, t, 0);
		glTexCoord2f(texR, texTB);	glVertex3f(r, tB, 0);
		glTexCoord2f(texRB, texTB);	glVertex3f(rB, tB, 0);

		// left border
		glTexCoord2f(texL, texTB);	glVertex3f(l, tB, 0);
		glTexCoord2f(texLB, texTB);	glVertex3f(lB, tB, 0);
		glTexCoord2f(texLB, texBB);	glVertex3f(lB, bB, 0);
		glTexCoord2f(texL, texBB);	glVertex3f(l, bB, 0);

		// center area
		glTexCoord2f(texLB, texTB);	glVertex3f(lB, tB, 0);
		glTexCoord2f(texRB, texTB);	glVertex3f(rB, tB, 0);
		glTexCoord2f(texRB, texBB);	glVertex3f(rB, bB, 0);
		glTexCoord2f(texLB, texBB);	glVertex3f(lB, bB, 0);

		// right border
		glTexCoord2f(texRB, texTB);	glVertex3f(rB, tB, 0);
		glTexCoord2f(texR, texTB);	glVertex3f(r, tB, 0);
		glTexCoord2f(texR, texBB);	glVertex3f(r, bB, 0);
		glTexCoord2f(texRB, texBB);	glVertex3f(rB, bB, 0);

		// bottom-left corner
		glTexCoord2f(texL, texBB);	glVertex3f(l, bB, 0);
		glTexCoord2f(texLB, texBB);	glVertex3f(lB, bB, 0);
		glTexCoord2f(texLB, texB);	glVertex3f(lB, b, 0);
		glTexCoord2f(texL, texB);	glVertex3f(l, b, 0);

		// bottom border
		glTexCoord2f(texLB, texBB);	glVertex3f(lB, bB, 0);
		glTexCoord2f(texRB, texBB);	glVertex3f(rB, bB, 0);
		glTexCoord2f(texRB, texB);	glVertex3f(rB, b, 0);
		glTexCoord2f(texLB, texB);	glVertex3f(lB, b, 0);

		// bottom-right corner
		glTexCoord2f(texRB, texBB);	glVertex3f(rB, bB, 0);
		glTexCoord2f(texR, texBB);	glVertex3f(r, bB, 0);
		glTexCoord2f(texR, texB);	glVertex3f(r, b, 0);
		glTexCoord2f(texRB, texB);	glVertex3f(rB, b, 0);
	glEnd();
}

void Component::getFocus()
{
	getMouseFocus();
	getKeybFocus();
}

void Component::releaseFocus()
{
	releaseMouseFocus();
	releaseKeybFocus();
}

void Component::getMouseFocus()
{
	m_bHasMouseFocus = true;
}

void Component::getKeybFocus()
{
	m_bHasKeybFocus = true;
}

void Component::releaseMouseFocus()
{
	m_bHasMouseFocus = false;
}

void Component::releaseKeybFocus()
{
	m_bHasKeybFocus = false;
}

void Component::onMouseDown(int x, int y, int button)
{
}

void Component::onMouseMove(int x, int y, int prevx, int prevy)
{
}

void Component::onMouseUp(int x, int y, int button)
{
}

void Component::onKeyDown(int key)
{
}

void Component::onKeyUp(int key)
{
}

bool Component::isPtInside(int x, int y)
{
	if (x<m_left || x>m_right || y<m_top || y>m_bottom)
		return false;
	return true;
}
