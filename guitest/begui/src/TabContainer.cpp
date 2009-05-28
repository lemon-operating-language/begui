/* 
// Copyright 2009 Alexandros Panagopoulos
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

#include "TabContainer.h"

using namespace begui;

TabContainer::TabContainer() :
	m_curTab(0),
	m_bCanCreateTabs(false),
	m_bCanCloseTabs(false),
	m_bCanDragTabs(false)
{
}

TabContainer::~TabContainer()
{
	clear();
}

void TabContainer::clear()
{
	for (size_t i=0; i<m_tabs.size(); ++i)
		SAFE_DELETE(m_tabs[i]);
	m_tabs.clear();
}

void TabContainer::create(int left, int top, int width, int height, const std::string &style_name,
							bool bCanCreateTabs, bool bCanCloseTabs)
{
	setPos(left, top);
	setSize(width, height);

	m_bCanCreateTabs = bCanCreateTabs;
	m_bCanCloseTabs = bCanCloseTabs;

	// load the associated style
}

void TabContainer::addTab(const std::string &title)
{
	Tab *pTab = new Tab;
	pTab->m_title = title;
	pTab->m_icon = 0;
	m_tabs.push_back(pTab);
}

void TabContainer::remTab(int tab)
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	SAFE_DELETE(m_tabs[tab]);
	m_tabs.erase(m_tabs.begin()+tab);
}

void TabContainer::remAllTabs()
{
	for (size_t i=0; i<m_tabs.size(); ++i)
		SAFE_DELETE(m_tabs[i]);
	m_tabs.clear();
}

void TabContainer::addComponent(Component *pC, int tab)
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	ASSERT(pC);
	m_tabs[tab]->addComponent(pC);
}

void TabContainer::remComponent(Component *pC)
{
	ASSERT(pC);

	// find in which tab this component is
	for (size_t i=0; i<m_tabs.size(); ++i) {
		if (tabContains((int)i, pC)) {
			m_tabs[i]->remComponent(pC);
		}
	}
}

void TabContainer::setTabTitle(int tab, const std::string &title)
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	m_tabs[tab]->m_title = title;
}

void TabContainer::setTabIcon(int tab, ResourceManager::ImageRef *icon)
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	m_tabs[tab]->m_icon = icon;
}

std::string TabContainer::getTabTitle(int tab) const
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	return m_tabs[tab]->m_title;
}

bool TabContainer::tabContains(int tab, Component *pC)
{
	ASSERT(tab>=0 && tab<m_tabs.size());
	ASSERT(pC);
	return m_tabs[tab]->contains(pC);
}

bool TabContainer::contains(Component *pC)
{
	for (size_t i=0; i<m_tabs.size(); ++i) {
		if (tabContains((int)i, pC))
			return true;
	}
	return false;
}

void TabContainer::frameUpdate()
{
	for (size_t i=0; i<m_tabs.size(); ++i)
		m_tabs[i]->frameUpdate();
	Container::frameUpdate();
}

void TabContainer::frameRender()
{
	// Render the tab headers
	//TODO

	Container::frameRender();
}

void TabContainer::onRender()
{
	// render the contents of the active tab
	if (m_curTab >= 0 && m_curTab < (int)m_tabs.size())
		m_tabs[m_curTab]->frameRender();
}

bool TabContainer::onMouseDown(int x, int y, int button)
{
	// check for events on the tab headers
	// TODO

	if (m_curTab >= 0 && m_curTab < (int)m_tabs.size())
		return m_tabs[m_curTab]->onMouseDown(x,y,button);
	return Container::onMouseDown(x,y,button);
}

bool TabContainer::onMouseMove(int x, int y, int prevx, int prevy)
{
	// check for events on the tab headers
	// TODO

	if (m_curTab >= 0 && m_curTab < (int)m_tabs.size())
		return m_tabs[m_curTab]->onMouseMove(x,y,prevx,prevy);
	return Container::onMouseMove(x,y,prevx,prevy);
}

bool TabContainer::onMouseUp(int x, int y, int button)
{
	// check for events on the tab headers
	// TODO

	if (m_curTab >= 0 && m_curTab < (int)m_tabs.size())
		return m_tabs[m_curTab]->onMouseUp(x,y,button);
	return Container::onMouseUp(x,y,button);
}

void TabContainer::onKeyDown(int key)
{
	//TODO
}

void TabContainer::onKeyUp(int key)
{
	//TODO
}

void TabContainer::handleTabToFront(Functor1<int> &callback)
{
	m_onTabToFront = callback;
}

void TabContainer::handleTabToBack(Functor1<int> &callback)
{
	m_onTabToBack = callback;
}

void TabContainer::handleTabCreate(Functor1<int> &callback)
{
	m_onTabCreate = callback;
}

void TabContainer::handleTabClose(Functor1<int> &callback)
{
	m_onTabClose = callback;
}

void TabContainer::handleTabDragStart(Functor1<int> &callback)
{
	m_onTabDragStart = callback;
}

void TabContainer::handleTabDragEnd(Functor1<int> &callback)
{
	m_onTabDragEnd = callback;
}