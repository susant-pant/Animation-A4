// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "boid.h"

#define PI 3.14159265359

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

GLenum drawMode;
vec2 mousePos;

Camera cam;

GLFWwindow* window = 0;

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

bool wPressed = false;
bool sPressed = false;
bool dPressed = false;
bool aPressed = false;
bool ePressed = false;
bool qPressed = false;

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_RELEASE))
		wPressed = !wPressed;
	else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_RELEASE))
		sPressed = !sPressed;
	else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_RELEASE))
		dPressed = !dPressed;
	else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_RELEASE))
		aPressed = !aPressed;
	else if(key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_RELEASE))
		ePressed = !ePressed;
	else if(key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_RELEASE))
		qPressed = !qPressed;
}

bool mousePressed = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) )
		mousePressed = !mousePressed;
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);
	vec2 diff = newPos - mousePos;

	if(mousePressed)
		cam.cameraRotation(-diff.x, diff.y);

	mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

}

//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2

struct VertexBuffers{
	enum{ VERTICES=0, COUNT};

	GLuint id[COUNT];
};

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao, const VertexBuffers& vbo)
{
	glBindVertexArray(vao);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_TRUE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const VertexBuffers& vbo, const vector<vec3>& points)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),		//Size of data in array (in bytes)
		&points[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
GLuint initShader(string vertexName, string fragmentName)
{	
	string vertexSource = LoadSource(vertexName);		//Put vertex file text into string
	string fragmentSource = LoadSource(fragmentName);		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	return LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array
}

//Initialization
void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.1f, 0.2f, 0.2f, 1.f);		//Color to clear the screen with (R, G, B, Alpha)
}

bool loadUniforms(Camera* cam, GLuint program, mat4 perspective, mat4 modelview)
{
	glUseProgram(program);

	mat4 camMatrix = cam->getMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"),
						1,
						false,
						&camMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"),
						1,
						false,
						&modelview[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"),
						1,
						false,
						&perspective[0][0]);

	return !CheckGLErrors("loadUniforms");
}

//Draws buffers to screen
void render(GLuint vao, int startElement, int numElements)
{
	glBindVertexArray(vao);		//Use the LINES vertex array
	glDrawArrays(
			drawMode,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			startElement,
			numElements		//How many indices
			);

	CheckGLErrors("render");
}

void drawBoids(vector<vec3>* vertices, vector<Boid>& boids) {
	vertices->clear();
	for(uint i =0; i < boids.size(); i++){
		vertices->push_back(vec3(boids[i].pos));
		vertices->push_back(vec3(boids[i].pos) + normalize(vec3(boids[i].velocity)));
	}
}

GLFWwindow* createGLFWWindow()
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return NULL;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1024, 1024, "Animation Assignment 3", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return NULL;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwMakeContextCurrent(window);

	return window;
}

void initBoids(int numBoids, vector<Boid>& boids){
	boids.clear();
	for(int i = 0; i < numBoids; i++) {
		boids.push_back(Boid());
	}
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[]) {
	window = createGLFWWindow();
	if(window == NULL)
		return -1;

	//Initialize glad
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	initGL();

	//Initialize shader
	GLuint program = initShader("vertex.glsl", "fragment.glsl");

	GLuint vaoBoids;
	VertexBuffers vboBoids;
	glGenVertexArrays(1, &vaoBoids);
	glGenBuffers(VertexBuffers::COUNT, vboBoids.id);
	initVAO(vaoBoids, vboBoids);

	//Geometry information
	vector<vec3> vertices;

	vector<Boid> boids;

	int numBoids = 250;
	srand(time(0));
	initBoids(numBoids, boids);

	cam = Camera(vec3(0, 0, -1), vec3(0, 1.f, 50.f));
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 500.f);

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)
		glUseProgram(program);

		drawBoids(&vertices, boids);
		loadBuffer(vboBoids, vertices);

		float move = 0.05f;
		if (wPressed) cam.pos += cam.dir*move;
		if (sPressed) cam.pos -= cam.dir*move;
		if (dPressed) cam.pos += cam.right*move;
		if (aPressed) cam.pos -= cam.right*move;
		if (ePressed) cam.pos += cam.up*move;
		if (qPressed) cam.pos -= cam.up*move;

		loadUniforms(&cam, program, perspectiveMatrix, mat4(1.f));

		// call function to draw our scene
		drawMode = GL_LINES;
		render(vaoBoids, 0, vertices.size());

		vec3 velocityMatch;
		vec3 collisionAvoidance;
		//vec3 flockCentering = vec3(0);
		// Didn't get to do flock centering

		for(uint i = 0; i < boids.size(); i++){

			vec3 matchVel = boids[i].velocity;
			int matchCounter = 1;

			for(uint j = 0; j < boids.size(); j++){
				if (i == j) continue;

				bool isVisible = (boids[i].inLineOfSight(boids[j]) && boids[i].inVisibleRange(boids[j]));

				if (isVisible && boids[i].inFlockRange(boids[j])) {
					matchVel += boids[j].velocity;
					matchCounter++;
				}
			}

			if (matchCounter == 1) {
				matchVel = boids[i].velocity;

				for(uint j = 0; j < boids.size(); j++){
					if (i == j) continue;

					float boidDist = boids[i].distanceTo(boids[j]);
					bool isVisible = (boids[i].inLineOfSight(boids[j]) && boids[i].inVisibleRange(boids[j]));

					if (isVisible && !boids[i].inFlockRange(boids[j])) {
						matchVel = normalize(boids[j].pos - boids[i].pos) * boidDist;
						break;
					}
				}
			}

			velocityMatch = matchVel/float(matchCounter);

			vec3 avoidVel = boids[i].velocity;

			for(uint j = 0; j < boids.size(); j++){
				if (i == j) continue;

				float collisionDist = 2.f;
				vec3 boid1Pos = boids[i].pos + boids[i].velocity;
				vec3 boid2Pos = boids[j].pos + boids[j].velocity;

				float boidDist = length(boid2Pos - boid1Pos);
				if (boidDist < collisionDist) {
					avoidVel += vec3(float((rand() % 7) - 3) / 100.f, float((rand() % 7) - 3) / 100.f, float((rand() % 7) - 3) / 100.f);
				}

			}
			collisionAvoidance = normalize(avoidVel);

			//vec3 finalVelocity = normalize((velocityMatch + collisionAvoidance + flockCentering) / 3.f);
			boids[i].setVelocity((velocityMatch + collisionAvoidance) / 2.f);
			boids[i].updatePos();
		}

		// scene is rendered to the back buffer, so swap to front for display
		glfwSwapBuffers(window);
		// sleep until next event before drawing again
		glfwPollEvents();
	}

	// clean up allocated resources before exit
	glDeleteVertexArrays(1, &vaoBoids);
	glDeleteBuffers(VertexBuffers::COUNT, vboBoids.id);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();

   return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		 << "with GLSL [ " << glslver << " ] "
		 << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			 istreambuf_iterator<char>(),
			 back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file " << filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}


// ==========================================================================
