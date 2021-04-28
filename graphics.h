#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glu.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include "color.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

class Graphics {
private:
	bool loadShaders();
	void iniCamera();
	void calculaCentreDistancia(glm::vec3 & minim, glm::vec3 & maxim);
	void projectTransform();
	void viewTransform();
	void updateBar(int index, double* data);
	void updateSphere();
	void generateSphere();
	float angleS;
	GLuint VBO_Sphere[2];
	std::vector<glm::vec3> vertexsS;
	std::vector<glm::vec3> colorsS;

	bool initGL();
	void subdivideTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int level);
	void createBuffers();
	SDL_Window* gWindow = NULL;
	SDL_GLContext gContext;
	GLuint gProgramID;
	// attribute locations
	GLuint vertexLoc, colorLoc;
	// uniform locations
	GLuint transLoc, projLoc, viewLoc;
	// VAO i VBO names
	GLuint VAO_Bar, VAO_Sphere;
	GLint ample, alt;
	// Internal vars
	float scale, angle;
	glm::vec3 pos;
	glm::vec3 VRP, OBS;
	float R, distancia, FOV, FOVini, zN, zF, rav, xort, yort;
	bool ortogonal;

	//Angles d'euler
	float AngleX, AngleY, AngleZ;
	GLint Xant, Yant, Zant;
	double* data;
	int numData;
	int r, g, b;
public:
	bool init();
	void close();
	void paint();
	void update(double* data, int numData);
};
