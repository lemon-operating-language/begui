#include <stdlib.h>
#include "../../src/bgui.h"
#include <GL/glut.h>

Window myWindow;
Button myBtn1;
ListBox myList1;
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

void changeSize(int w, int h) {

	width = w;
	height = h;
}

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

void initBeGUI()
{
	// Get windows directory
	wchar_t win_dirw[MAX_PATH+1];
	GetWindowsDirectory(win_dirw, MAX_PATH);
	char win_dir[MAX_PATH+1] = "c:\\windows";

	// Initialize font subsystem
	if (!FontManager::initialize())
		throw std::exception("failed to initialize font subsystem");
	if (!FontManager::setFont(strcat(win_dir, "\\Fonts\\tahoma.ttf"), 11))
		throw std::exception("failed to set default font");
	
	// Initialize the window manager and load resources
	ResourceManager::inst()->setResourceDir("..\\..\\..\\resources\\");
	ResourceManager::inst()->loadResources();
	
	// set some OpenGL states
	glClearColor(0, 0, 0, 0);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
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

	// initialize beGUI subsystems
	initBeGUI();

	// create the beGUI window
	myWindow.create(20, 20, 200, 200, "test");
	myBtn1.create(20, 20, "Button 1", 101);
	myWindow.addComponent(&myBtn1);
	myList1.create(20, 50, 160, 100, ListBox::SINGLE_SELECT);
	myList1.addItem("Wireframe mode");
	myList1.addItem("Smooth shading");
	myList1.addItem("Another silly entry");
	myList1.addItem("Wireframe mode");
	myList1.addItem("Smooth shading");
	myList1.addItem("Another silly entry");
	myList1.addItem("Wireframe mode");
	myList1.addItem("Smooth shading");
	myWindow.addComponent(&myList1);

	// start the main loop
	glutMainLoop();

	return 0;
}