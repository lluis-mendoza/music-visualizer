#include "graphics.h"

bool Graphics::init()
{
	r = g = b = 0;
	//Initialization flag
	bool success = true;
	data = NULL;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize GLEW
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if (glewError != GLEW_OK)
				{
					printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
				}

				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}
bool Graphics::initGL() {

	//Generate program
	gProgramID = glCreateProgram();

	if (!loadShaders()) return false;

	glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
	
	createBuffers();

	iniCamera();
}
std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

bool Graphics::loadShaders() {

	const char * vertex_path = "./shaders/basicShader.vert";
	const char * fragment_path = "./shaders/basicShader.frag";

	//Create vertex shader
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shaders
	std::string vertShaderStr = readFile(vertex_path);
	std::string fragShaderStr = readFile(fragment_path);
	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	//Set vertex source
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);

	//Compile vertex source
	glCompileShader(vertShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		printf("Unable to compile vertex shader %d!\n", vertShader);
		return false;
	}

	glAttachShader(gProgramID, vertShader);

	//Set fragment source
	glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		printf("Unable to compile fragment shader %d!\n", fragmentShader);
		return false;
	}
	//Attach fragment shader to program
	glAttachShader(gProgramID, fragmentShader);


	//Link program
	glLinkProgram(gProgramID);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", gProgramID);
		return false;
	}


	vertexLoc = glGetAttribLocation(gProgramID, "vertex");
	colorLoc = glGetAttribLocation(gProgramID, "color");
	transLoc = glGetUniformLocation(gProgramID, "TG");
	viewLoc = glGetUniformLocation(gProgramID, "view");
	projLoc = glGetUniformLocation(gProgramID, "proj");
	glDeleteShader(vertShader);
	glDeleteShader(fragmentShader);
	return true;
}
glm::vec3 normalize(glm::vec3 a, glm::vec3 b, float length) {
	
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float dz = b.z - a.z;
	float distance = sqrt(dx*dx+dy*dy+dz*dz);
	dx = dx*length / distance;
	dy = dy*length / distance;
	dz = dz*length / distance;
	
	glm::vec3 c(dx,dy,dz);

	return c;
}

void Graphics::subdivideTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int level) {
	if (level == 0) {
		float value = 1;
		
		float factorColor = 1.4;
		if (data != NULL) {
			value = std::min(1.0f, std::max(0.5f, (float)log(data[(int)(numData*(p1.y + 1) / 2)])/7));
			if (p1.y > 1.0 / 3) r = std::min(255, std::max(r, (int)(255*value*factorColor)));
			else if (p1.y > - 1.0 / 3) g = std::min(255, std::max(g, (int)(255*value*factorColor)));
			else  b = std::min(255, std::max(b, (int)(255*value*factorColor)));
		}
		vertexsS.push_back(normalize(glm::vec3(0, 0, 0), p1, value));
		value = 1;
		if (data != NULL) {
			value = std::min(1.0f, std::max(0.5f, (float)log(data[(int)(numData*(p2.y + 1) / 2)])/7));
			if (p2.y > 1.0 / 3) r = std::min(255, std::max(r, (int)(255*value*factorColor)));
			else if (p2.y > - 1.0 / 3) g = std::min(255, std::max(g, (int)(255*value*factorColor)));
			else  b = std::min(255, std::max(b, (int)(255*value*factorColor)));
		}
		vertexsS.push_back(normalize(glm::vec3(0, 0, 0), p2, value));
		value = 1;
		if (data != NULL) {
			value = std::min(1.0f, std::max(0.5f, (float)log(data[(int)(numData*(p3.y + 1) / 2)])/7));
			if (p3.y > 1.0 / 3) r = std::min(255, std::max(r, (int)(255*value*factorColor)));
			else if (p3.y > - 1.0 / 3) g = std::min(255, std::max(g, (int)(255*value*factorColor)));
			else  b = std::min(255, std::max(b, (int)(255*value*factorColor)));
		}
		vertexsS.push_back(normalize(glm::vec3(0, 0, 0), p3, value));
	}
	else {
		glm::vec3 p12 = 0.5f*(p1 + p2);

		glm::vec3 p13 = 0.5f*(p1 + p3);

		glm::vec3 p23 = 0.5f*(p2 + p3);

		subdivideTriangle(p1, p12, p13, level - 1);
		subdivideTriangle(p13, p23, p3, level - 1);
		subdivideTriangle(p13, p23, p12, level - 1);
		subdivideTriangle(p12, p23, p2, level - 1);
	}
}
void Graphics::createBuffers() {
	GLuint VBO_Object[2];
	
	glm::vec3 vertexs[6] = {
		glm::vec3(-0.5f, -0.5f, -1.0f),
		glm::vec3(-0.5f, 0.5f, -1.0f),
		glm::vec3(0.5f, -0.5f, -1.0f),
		glm::vec3(-0.5f, 0.5f, -1.0f),
		glm::vec3(0.5f, 0.5f, -1.0f),
		glm::vec3(0.5f, -0.5f, -1.0f)
	};
	glm::vec3 colors[6] = {
		glm::vec3(0, 0, 1.0f),
		glm::vec3(0,0,1.0f),
		glm::vec3(0,0,1.0f),
		glm::vec3(0,0,1.0f),
		glm::vec3(0,0,1.0f),
		glm::vec3(0,0,1.0f)
	};
	glGenVertexArrays(1, &VAO_Bar);
	glBindVertexArray(VAO_Bar);

	glGenBuffers(2, VBO_Object);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Object[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);

	// Activem l'atribut vertexLoc
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLoc);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Object[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	// Activem l'atribut colorLoc
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLoc);
	
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), 3);

	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), 3);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), 3);
	
	colorsS = {};
	for (int i = 0; i < vertexsS.size();++i) {
		colorsS.push_back(glm::vec3(r, g, b));
		colorsS.push_back(glm::vec3(r, g, b));
		colorsS.push_back(glm::vec3(r, g, b));
	}
	glGenVertexArrays(1, &VAO_Sphere);
	glBindVertexArray(VAO_Sphere);
	
	glGenBuffers(2, VBO_Sphere);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexsS[0])*vertexsS.size(), &vertexsS[0], GL_STATIC_DRAW);

	// Activem l'atribut vertexLoc
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLoc);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorsS[0])*colorsS.size(), &colorsS[0], GL_STATIC_DRAW);

	// Activem l'atribut colorLoc
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLoc);

	glBindVertexArray(0);
}

void Graphics::close()
{
	//Deallocate program
	glDeleteProgram(gProgramID);

	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void Graphics::updateBar(int index, double* data)
{
	//No per frame update needed
	glm::mat4 transform(1.0f);
	
	transform = glm::translate(transform, -glm::vec3(1.0f/2.0f-1.0f /numData*index, 0, 0));
	transform = glm::scale(transform, glm::vec3(1.0f/numData, data[index]/100, 1.0f));
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void Graphics::updateSphere() {
	glm::mat4 transform(1.0f);
	transform = glm::rotate(transform, -angleS, glm::vec3(0, 1, 0)); 
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void Graphics::generateSphere() {
	r = g = b = 170;
	vertexsS = {};
	int numTriangles = 4;
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, 1.0f, 0.0f), numTriangles);

	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), numTriangles);
	subdivideTriangle( glm::vec3(-sqrt(2)/2.0f, 0.0f, sqrt(0.5f)), glm::vec3(-sqrt(2)/2.0f, 0.0f, -sqrt(0.5f)), glm::vec3(0, -1.0f, 0.0f), numTriangles);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexsS[0])*vertexsS.size(), &vertexsS[0], GL_STATIC_DRAW);
	colorsS = {};
	for (int i = 0; i < vertexsS.size();++i) {
		colorsS.push_back(glm::vec3(r/255.0, g/255.0, b/255.0));
		colorsS.push_back(glm::vec3(r/255.0, g/255.0, b/255.0));
		colorsS.push_back(glm::vec3(r/255.0, g/255.0, b/255.0));
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorsS[0])*colorsS.size(), &colorsS[0], GL_STATIC_DRAW);


}
void Graphics::paint()
{
	// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	ample = vp[2];
	alt = vp[3];
#endif

	// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
	// useu els paràmetres que considereu (els que hi ha són els de per defecte)
	//  glViewport (0, 0, ample, alt);

	// Esborrem el frame-buffer
	glUseProgram(gProgramID);
	glClear(GL_COLOR_BUFFER_BIT);

	// Carreguem la transformació de model

	projectTransform();
	viewTransform();

	generateSphere();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO_Sphere);
	angleS += 0.01;
	updateSphere();
	glDrawArrays(GL_TRIANGLES, 0, vertexsS.size()*3);
	glBindVertexArray(0);
	SDL_GL_SwapWindow(gWindow);
}
void Graphics::update(double* _data, int _numData)
{
	data = _data;
	numData = _numData;
}
void Graphics::iniCamera() {
	ortogonal = false;
	rav = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	glm::vec3 minim(0, 0, 1);
	glm::vec3 maxim(0, 0, -1);
	calculaCentreDistancia(minim, maxim);
	distancia = 2 * R;
	OBS = VRP + glm::vec3(0, 0, distancia);
	FOV = 2 * asin(R / distancia);
	zN = distancia - R;
	zF = distancia + R;
	FOVini = FOV;
	xort = yort = R;

	AngleX = AngleY = AngleZ = 0;
	Xant = Yant = Zant = 0;
	angle = 0;
	scale = 1;
	angleS = 0;
}
void Graphics::calculaCentreDistancia(glm::vec3& minim, glm::vec3& maxim) {

	VRP = (minim + maxim) / glm::vec3(2);
	R = distance(minim, maxim) / 2;
}

void Graphics::projectTransform() {
	if (ortogonal) {
		glm::mat4 Proj = glm::ortho(-xort, xort, -yort, yort, zN, zF);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
	}
	else {
		glm::mat4 Proj = glm::perspective(FOV, rav, zN, zF);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
	}
}

void Graphics::viewTransform() {

	//Angles d'Euler 
	glm::mat4 View(1.0f);

	View = glm::translate(View, glm::vec3(0, 0, -distancia));
	View = glm::rotate(View, -AngleZ, glm::vec3(0, 0, 1)); //Phi   Rotacio
	View = glm::rotate(View, AngleY, glm::vec3(1, 0, 0)); //Fita  Vertical
	View = glm::rotate(View, -AngleX, glm::vec3(0, 1, 0)); //Psi   Horitzontal
	View = glm::translate(View, -VRP);

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

