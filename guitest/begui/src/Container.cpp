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

#include "Container.h"

using namespace begui;

Container::Container() : m_pActiveComponent(0)
{
}

Container::~Container()
{
}

void Container::frameUpdate()
{
	// update children too
	for (size_t i=0; i<m_children.size(); ++i)
		m_children[i]->frameUpdate();
}

void Container::frameRender()
{
	// Change the coordinate system to the local one
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_left, m_top, 0);
	
	// Render the container itself
	onRender();
	
	// render children
	std::vector<Component*> ontop;
	for (size_t i=0; i<m_children.size(); ++i)
	{
		if (m_children[i]->isVisible())
		{
			m_children[i]->frameRender();
		}
	}

	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Container::onMouseDown(int x, int y, int button)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_left, m_top, 0);
	
	// Transform coordinates.
	Vector2 lP = /*toLocal*/(Vector2(x,y));

	// select active component
	// traversal is done in z-order (starting from the end of the
	// list where the top child is) in order to eliminate problems
	// due to overlapping
	int activeComponent = -1;
	for (int i=(int)m_children.size()-1; i>=0; --i)
	{
		if (m_children[i]->isVisible())
		{
			if (m_children[i]->isPtInside(lP.x, lP.y))
			{
				activeComponent = i;
				break;
			}
		}
	}

	// Change the current active component
	Component *pOldActive = m_pActiveComponent;
	m_pActiveComponent = (activeComponent>=0)?m_children[activeComponent]:0;

	// Deactive the previous active component
	if (pOldActive && pOldActive != m_pActiveComponent)
	{
		pOldActive->releaseFocus();
		pOldActive->onDeactivate();
	}

	if (m_pActiveComponent)
	{
		// Bring the new active component on top (method respects
		// fixedZOrder flag)
		bringChildToFront(activeComponent);

		// Activate the new component
		m_pActiveComponent->onActivate();

		// switch focus from this component to the child
		releaseFocus();
		m_pActiveComponent->getFocus();

		// get the local coordinates inside the child component:
		Vector2 llP = m_pActiveComponent->parentToLocal(lP);
		m_pActiveComponent->onMouseDown((int)llP.x, (int)llP.y, button);
	}
	else
	{
		// Custom mouse handling
		onMouseDownEx((int)lP.x, (int)lP.y);
	}
	
	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Container::onMouseMove(int x, int y, int prevx, int prevy)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_left, m_top, 0);

	if ((m_pActiveComponent && !m_pActiveComponent->hasMouseFocus()) || !m_pActiveComponent)
	{
		for (int i=(int)m_children.size()-1; i>=0; --i)
		{
			// Transform coordinates.
			Vector2 lP = m_children[i]->parentToLocal(Vector2(x,y));
			Vector2 lpP = m_children[i]->parentToLocal(Vector2(prevx,prevy));

			m_children[i]->onMouseMove((int)lP.x, (int)lP.y, (int)lpP.x, (int)lpP.y);
		}
	}

	// call mouseMove of active component
	// (without updating the active component)
	if (m_pActiveComponent)
	{
		// Transform coordinates.
		Vector2 lP = m_pActiveComponent->parentToLocal(Vector2(x,y));
		Vector2 lpP = m_pActiveComponent->parentToLocal(Vector2(prevx,prevy));

		if (m_pActiveComponent->hasMouseFocus())
		{
			m_pActiveComponent->onMouseMove((int)lP.x, (int)lP.y, (int)lpP.x, (int)lpP.y);
		}
	}
	else
	{
		// custom handling
		onMouseMoveEx(x, y, prevx, prevy);
	}
	
	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Container::onMouseUp(int x, int y, int button)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_left, m_top, 0);

	// call mouseUp of active component
	// (without updating the active component)
	if (m_pActiveComponent)
	{
		// Transform coordinates.
		Vector2 lP = m_pActiveComponent->parentToLocal(Vector2(x,y));

		m_pActiveComponent->onMouseUp((int)lP.x, (int)lP.y, button);
	}
	else
	{
		// custom handling
		onMouseUpEx(x, y);
	}
	
	// Reset the coordinate system
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Container::onKeyDown(int key)
{
	// if key is tab, change the active component
	// to the next in the tab order
	//TODO

	if (m_pActiveComponent)
	{
		m_pActiveComponent->onKeyDown(key);
	}
}

void Container::onKeyUp(int key)
{
	if (m_pActiveComponent)
	{
		m_pActiveComponent->onKeyUp(key);
	}
}

void Container::bringChildToFront(int id)
{
	ASSERT(id >= 0 && id < (int)m_children.size());

	Component *comp = m_children[id];
	if (comp->hasFixedZOrder())
		return;

	// Change the order in the children array to bring the child to the top
	// (top child is the last in the array)
	// respect alwaysOnTop components.
	int sz = (int)m_children.size();
	int i=-1;
	for (i=id; i<sz-1; ++i)
	{
		if (m_children[i+1]->isAlwaysOnTop())
			break;
		m_children[i] = m_children[i+1];
	}
	m_children[i] = comp;
}

void Container::addComponent(Component *pC)
{
	ASSERT(pC);

	// top child is the last in the array. Respect alwaysOnTop
	// flags when adding the child
	if (m_children.size() == 0)
		m_children.push_back(pC);
	else
	{
		int i = (int)m_children.size()-1;
		if (!pC->isAlwaysOnTop())
		{
			for (; i>=0; --i)
			{
				if (!m_children[i]->isAlwaysOnTop())
					break;
			}
		}
		m_children.insert(m_children.begin()+(i+1), pC);
	}
	pC->setParent(this);
}