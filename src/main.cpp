#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow *window;


// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; };

color bColor;
color bgColor;
int bSize;
bool bCircle;
bool bSpray;

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

// Set a particular pixel of the frameBuffer to the provided color
void SetFrameBufferPixel(int x, int y, struct color lc)
{
	// Origin is upper left corner. 
	// Changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	frameBuffer[y][x][0] = lc.r;
	frameBuffer[y][x][1] = lc.g;
	frameBuffer[y][x][2] = lc.b;

}

void SetMaskBufferValue(int x, int y, bool val)
{
	// Origin is upper left corner. 
	// Changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	mask[y][x] = val;

}

void ClearFrameBuffer()
{
	
	for (int i = 0; i < WINDOW_WIDTH; i++) {
		for (int j = WINDOW_HEIGHT - 1; j >= 0; j--) {
			mask[j][i] = false;
		}
	}
	

	for (int i = 0; i < WINDOW_WIDTH; i++) {
		for (int j = WINDOW_HEIGHT - 1; j >= 0; j--) {
			frameBuffer[j][i][0] = bgColor.r;
			frameBuffer[j][i][1] = bgColor.g;
			frameBuffer[j][i][2] = bgColor.b;
		}
	}
	
	//memset(mask, false, sizeof(bool) * WINDOW_WIDTH * WINDOW_HEIGHT);
	//memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

// Display frameBuffer on screen
void Display()
{	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		//std::cout << "Mouse position is: x - " << xpos << ", y - " << ypos << std::endl;
		SetFrameBufferPixel(xpos, ypos, bColor);
		SetMaskBufferValue(xpos, WINDOW_HEIGHT - 1 - ypos, true);
		if (bSize != 1 && !bCircle) {
			for (int i = 0; i < bSize + 1; i++) {
				for (int j = 0; j < bSize + 1; j++) {
					if (bSpray) {
						if ((rand() % 100 + 1) > 90) {
							SetFrameBufferPixel(xpos - (bSize / 2) + i, ypos - (bSize / 2) + j, bColor);
							SetMaskBufferValue(xpos - (bSize / 2) + i, WINDOW_HEIGHT - 1 - ((ypos - (bSize / 2) + j)), true);
						}
					} else {
						SetFrameBufferPixel(xpos - (bSize / 2) + i, ypos - (bSize / 2) + j, bColor);
						SetMaskBufferValue(xpos - (bSize / 2) + i, WINDOW_HEIGHT - 1 - ((ypos - (bSize / 2) + j)), true);
					}
				}
			}
		}
		else if (bSize != 1 && bCircle) {
			for (int i = 0; i < bSize + 1; i++) {
				for (int j = 0; j < bSize + 1; j++) {
					if (bSpray) {
						if ((rand() % 100 + 1) > 90) {
							if ((i - (bSize / 2)) * (i - (bSize / 2)) + (j - (bSize / 2)) * (j - (bSize / 2)) <= (bSize / 2) * (bSize / 2)) {
								SetFrameBufferPixel(xpos - (bSize / 2) + i, ypos - (bSize / 2) + j, bColor);
								SetMaskBufferValue(xpos - (bSize / 2) + i, WINDOW_HEIGHT - 1 - ((ypos - (bSize / 2) + j)), true);
							}
						}
					}
					else {
						if ((i - (bSize / 2)) * (i - (bSize / 2)) + (j - (bSize / 2)) * (j - (bSize / 2)) <= (bSize / 2) * (bSize / 2)) {
							SetFrameBufferPixel(xpos - (bSize / 2) + i, ypos - (bSize / 2) + j, bColor);
							SetMaskBufferValue(xpos - (bSize / 2) + i, WINDOW_HEIGHT - 1 - ((ypos - (bSize / 2) + j)), true);
						}
					}
				}
			}
		}
	}
}

void ChangeBackgroundColor(float r, float g, float b) {
	bgColor.r = r;
	bgColor.g = g;
	bgColor.b = b;

	for (int i = 0; i < WINDOW_WIDTH; i++) {
		for (int j = 0; j < WINDOW_HEIGHT; j++) {
			if (mask[j][i] == false) {
				SetFrameBufferPixel(i, j, bgColor);
			}
		}
	}
}

void ChangeBrushColor(float r, float g, float b) {
	bColor.r = r;
	bColor.g = g;
	bColor.b = b;
}

// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		ClearFrameBuffer();
		std::cout << "Painting cleared!" << std::endl;
	}

}

// You can use "switch" or "if" to compare key to a specific character.
// for example,
// if (key == '0')
//     DO SOMETHING

// Keyboard callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch ((char)key)
	{
		case '=':
			if (bSize != 128) {
				bSize *= 2;
				std::cout << "Brush size increased to " << bSize << "!" << std::endl;
			}
			else {
				std::cout << "Brush size already the biggest size!" << std::endl;
			}
			break;
		case '-':
			if (bSize != 1) {
				bSize /= 2;
				std::cout << "Brush size reduced to " << bSize << "!" << std::endl;
			}
			else {
				std::cout << "Brush size already the smallest size!" << std::endl;
			}
			break;
		case '0':
			ChangeBrushColor(0, 0, 0);
			break;
		case '1':
			ChangeBrushColor(0, 0, 255);
			break;
		case '2':
			ChangeBrushColor(0, 255, 0);
			break;
		case '3':
			ChangeBrushColor(0, 255, 255);
			break;
		case '4':
			ChangeBrushColor(255, 0, 0);
			break;
		case '5':
			ChangeBrushColor(255, 0, 255);
			break;
		case '6':
			ChangeBrushColor(255, 255, 0);
			break;
		case '7':
			ChangeBrushColor(255, 255, 255);
			break;
		case ')':
			ChangeBackgroundColor(0, 0, 0);
			break;
		case '!':
			ChangeBackgroundColor(0, 0, 255);
			break;
		case '@':
			ChangeBackgroundColor(0, 255, 0);
			break;
		case '#':
			ChangeBackgroundColor(0, 255, 255);
			break;
		case '$':
			ChangeBackgroundColor(255, 0, 0);
			break;
		case '%':
			ChangeBackgroundColor(255, 0, 255);
			break;
		case '^':
			ChangeBackgroundColor(255, 255, 0);
			break;
		case '&':
			ChangeBackgroundColor(255, 255, 255);
			break;
		case 'b':
			if (!bCircle) {
				bCircle = true;
				std::cout << "Brush shape is now a circle!" << std::endl;
			} else {
				bCircle = false;
				std::cout << "Brush shape is now a rectangle!" << std::endl;
			}
			break;
		case 's':
			if (!bSpray) {
				bSpray = true;
				std::cout << "Brush is now a paint spray brush!" << std::endl;
			}
			else {
				bSpray = false;
				std::cout << "Spray paint brush is now an ordinary brush!" << std::endl;
			}
			break;
	}
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1 - Xchel Diaz", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();
}

void GlobalSetters() {
	bColor.r = 255;
	bColor.g = 255;
	bColor.b = 255;
	bgColor.r = 0;
	bgColor.g = 0;
	bgColor.b = 0;
	bSize = 1;
	bCircle = false;
	bSpray = false;
}

int main()
{	
	GlobalSetters();

	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}