# Introduction #

It is very easy to use individual components of beGUI in an OpenGL application which was not written with beGUI in mind. In this article, we will describe how to add a window with a couple of controls in a program using GLUT.

# The GLUT program #

Let's assume that the GLUT program we are going to modify is the following:

```
#include <stdlib.h>
#include <GL/glut.h>

int width = 0;
int height = 0;

void renderScene(void)
{
	static float angle = 0;

	glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(height == 0)
		height = 1;

	float ratio = 1.0* width / height;

	// Set the viewport to be the entire window
	glViewport(0, 0, width, height);

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,ratio,1,1000);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0, 
		      0.0,0.0,-1.0,
			  0.0f,1.0f,0.0f);
	glPushMatrix();
	glRotatef(angle,0.0,1.0,0.0);

	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(0.5,0.0,0.0);
		glVertex3f(0.0,0.5,0.0);
	glEnd();

	glPopMatrix();
	
	glutSwapBuffers();
	
	angle++;
}

void changeSize(int w, int h) {

	width = w;
	height = h;
}

void processMouse(int button, int state, int x, int y)
{
}

void processMouseActiveMotion(int x, int y)
{
}

void processMousePassiveMotion(int x, int y)
{
}

int main(int argc, char *argv[])
{
	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create the main window
	glutInitWindowPosition(30,30);
	glutInitWindowSize(800,600);
	glutCreateWindow("beGUI GLUT Example");

	// set the display function
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	// resize function
	glutReshapeFunc(changeSize);

	// handle mouse events
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);

	// start the main loop
	glutMainLoop();

	return 0;
}
```

This program simply creates a new GLUT window, and displays a rotating triangle.

# Adding beGUI support #

The first step to use beGUI in your application is to initialize all basic objects. This means, load all resources and fonts. First, we add the following include statement (before including GLUT):

```
#include <bgui.h>
```

Next, create a new function to initialize beGUI, like this:

```
void initBeGUI()
{
	// Get windows directory
	char win_dir[MAX_PATH+1];
	GetWindowsDirectory(win_dir, MAX_PATH);

	// Initialize font subsystem
	if (!FontManager::initialize())
		throw std::exception("failed to initialize font subsystem");
	if (!FontManager::setFont(strcat(win_dir, "\\Fonts\\tahoma.ttf"), 11))
		throw std::exception("failed to set default font");
	
	// Initialize the window manager and load resources
	ResourceManager::inst()->setResourceDir("..\\..\\..\\resources\\"); //CHANGE THIS to the folder where begui\resources is located
	ResourceManager::inst()->loadResources();
}
```

**Remember to change `"..\\..\\..\\resources\\"` to the path where the `begui\resources\` folder is located.** Alternatively, you can copy `begui\resources` to a new directory in your project's folder and use that path. This way, you can also use an alternative GUI skin by modifying the contents of the copied directory (like `style.txt`).

We have to call this function after the GLUT window is created, in our `main` function.

Don't forget to link your program with the library files for beGUI, GLEW and Freetype, which are all necessary for beGUI.

# Creating a window #

Now, let's create a window and a couple of controls. First, create the object for the window, a button and a listbox, as globals in our program:

```
Window myWindow;
Button myBtn1;
ListBox myList1;
```

Now, we have to initialize the window and the two controls. This is done by calling the `create` methods of each object, and adding the two controls to the window using the method `Window::addComponent(...)`, as follows:

```
int main(int argc, char *argv[])
{
        .........

	// initialize beGUI subsystems
	initBeGUI();

	// create the beGUI window
	myWindow.create(20, 20, 200, 200, "test");
	myBtn1.create(20, 20, "Button 1", 101);
	myWindow.addComponent(&myBtn1);
	myList1.create(20, 50, 160, 100, ListBox::SINGLE_SELECT);
	myList1.addItem("item 1");
	myList1.addItem("item 2");
	myList1.addItem("item 3");
	myWindow.addComponent(&myList1);

	// start the main loop
	glutMainLoop();

        .......
```

Finally, we have to render the beGUI controls in our application. To do so, we have to setup an orthographic projection, which will not scale our pixels, and for each frame, update and render our controls. To do so, we have to call the `frameUpdate` and `frameRender` methods of the controls on the top of the control hierarchy, in our case `myWindow`. The `renderFrame` method in our application is modified in the following way:

```
void renderScene(void)
{
        ....code....

	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(0.5,0.0,0.0);
		glVertex3f(0.0,0.5,0.0);
	glEnd();

	glPopMatrix();

        // Set some states: depth testing should be DISABLED
        // for beGUI controls to render properly.
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// Set an orthographic projection for beGUI
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// render the window
	myWindow.frameUpdate();
	myWindow.frameRender();
	
	glutSwapBuffers();
	
	angle++;
}
```

Now compile and run the program. You should be able to see a small window, with a button and a listbox inside. Notice however, that our window still does not respond to mouse or keyboard events.

# Forwarding mouse and keyboard events to beGUI #

To finish integrating beGUI with our app, we have to forward all mouse and keyboard events from GLUT to beGUI. Since we are not using beGUI's `FrameWindow` functionality, we have to do that in two parts:
  * Forward mouse/keyboard events to beGUI's input subsystem (namespace `input`)
  * Call the appropriate event handlers of all top-level components (`myWindow` in our case)

The event handling methods in our GLUT program are changes as follows:

```
int prevx = 0, prevy = 0;

void processMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		input::mouseButtonDown(x, y, MOUSE_BUTTON_LEFT);
		myWindow.onMouseDown(x, y, MOUSE_BUTTON_LEFT);
		prevx = x;
		prevy = y;
	}
	else if (state == GLUT_UP) {
		input::mouseButtonUp(x, y, MOUSE_BUTTON_LEFT);
		myWindow.onMouseUp(x, y, MOUSE_BUTTON_LEFT);
	}
}

void processMouseActiveMotion(int x, int y)
{
	input::mousePos(x,y);
	myWindow.onMouseMove(x, y, prevx, prevy);
	prevx = x;
	prevy = y;
}

void processMousePassiveMotion(int x, int y)
{
	input::mousePos(x,y);
	myWindow.onMouseMove(x, y, prevx, prevy);
	prevx = x;
	prevy = y;
}
```

Now you can compile and run the application again. This time, click on the window caption and drag it around, or try clicking the button or selecting items in the list. Cool, huh?

# Handling events in beGUI #

The final step in this tutorial is to see how we can respond to GUI events generated from beGUI. For example, we want to do something when the user clicks our new button, or when they select an item in our new list box. To do so, beGUI uses a C++ construct called functors.

Let's first add two functions, to handle button clicks and list box item selections respectively:

```
void onButtonClick(int id)
{
	// id: the id of the control generating this event
	if (id == 101) // if it was from our button
	{
		MessageBox(0, TEXT("You click my buttons!"), TEXT("Click"), MB_OK);
	}
}

void onListSelect(int sel) {
	// sel is the id of the selected item (starting from 0 for
	// the first item and continuing incrementaly for each subsequent
	// item)
	MessageBox(0, TEXT("List item selected"), TEXT("List event"), MB_OK);
}
```

Now, let's let beGUI know that it has to call our functions when the respective events occur. To do so, we modify our component initialization code in the following way:

```
        ......
	myBtn1.create(20, 20, "Button 1", 101, makeFunctor((Functor1<int>*)0, &onButtonClick)); // set the callback for button clicks
	myWindow.addComponent(&myBtn1);
	myList1.create(20, 50, 160, 100, ListBox::SINGLE_SELECT);
	myList1.handleOnItemSelect(makeFunctor((Functor1<int>*)0, &onListSelect)); // set the callback for list item selections
        ......
```

All done! Compile and run it again, and enjoy stuff happening when you interact with the controls.