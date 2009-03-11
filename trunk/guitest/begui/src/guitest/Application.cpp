#include "Application.h"
#include "../timeseries.h"

Application *Application::m_pInst = NULL;

void ChildWnd1::onCreate()
{
	// create a simple push button
	m_button1.create(260, 40, "Button", 1);
	addComponent(&m_button1);

	// create a group of radio buttons
	m_radio1.create(20, 20, "Radio Button 1", 2);
	m_radio2.create(20, 40, "Radio Button 2", 3);
	m_group.create(40, 30, 200, 80, "Radio group");
	m_group.addComponent(&m_radio1);
	m_group.addComponent(&m_radio2);
	addComponent(&m_group);

	// create a slider (also an example of using a live
	// variable)
	m_slider.create(40, 130, 300, 4);
	m_slider.bindValue(&m_sliderPos);
	m_slider.setBounds(0, 10, 2, 20);
	addComponent(&m_slider);

	// create a textbox
	m_textbox.create(250, 170, 130, 180, true, true);
	addComponent(&m_textbox);
	
	m_listbox.create(40, 150, 200, 200, ListBox::MULTI_SELECT_SINGLECLICK);
	m_listbox.addItem("item 1");
	m_listbox.addItem("disabled item 2");
	m_listbox.addItem("item 3");
	m_listbox.addItem("item 4");
	m_listbox.addItem("item 5");
	for (size_t i=0; i<20; ++i)
		m_listbox.addItem("item");
	m_listbox.addItem("last item");
	m_listbox.disableItem(1);
	addComponent(&m_listbox);

	m_combobox.create(250, 150, 80, 200);
	m_combobox.addItem("item 1");
	m_combobox.addItem("item 2");
	m_combobox.addItem("item 3");
	m_combobox.addItem("item 4");
	m_combobox.disableItem(2);
	addComponent(&m_combobox);
}

bool Application::initialize()
{
	// initialize the main window
	if (!BaseApp_Win::initialize())
		return false;

	// Get a pointer to the menu of the main window
	Menu &menu = FrameWindow::inst()->getMenu();

	// Create a new drop down menu
	Menu *pFileMenu = menu.addMenuItem("File", 1, makeFunctor(*this, &Application::onMenu));

	// Add menu items to this menu.
	pFileMenu->addMenuItem("Open image", 101, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Save image", 102, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Save image as..", 103, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addSeparator();
	pFileMenu->addMenuItem("Page Setup", 104, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Print Preview", 105, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addSeparator();
	pFileMenu->addMenuItem("Exit", 106, makeFunctor(*this, &Application::onMenu));
	
	Menu *pMenu2 = menu.addMenuItem("Menu 02", 2, makeFunctor(*this, &Application::onMenu));
	pMenu2->addMenuItem("Item 1", 201, makeFunctor(*this, &Application::onMenu));
	pMenu2->addMenuItem("Item 2", 202, makeFunctor(*this, &Application::onMenu));

	// create a child window
	m_wnd1.create(30, 50, 420, 400, "test");
	FrameWindow::inst()->addComponent(&m_wnd1);

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, // Instance 
				   HINSTANCE hPrevInstance, // Previous Instance 
				   LPSTR lpCmdLine, // Command Line Parameters 
				   int nCmdShow) // Window Show State 
{
	// Uncomment this line to have a console for text output
	Console::create();

	return Application::inst()->run("beGUI test", 800, 600);
}

void Application::onMenu(int id)
{
	switch (id)
	{
	case 106:
		exit(0);
		break;
	}
}