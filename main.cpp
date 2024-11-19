#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

std::string names[4] = {"Carlos ", "Mergano", "Mercado", " Yapan "};
//track variables
#define NUM_TRACK_SEGMENTS 10
float roadPositionZ[NUM_TRACK_SEGMENTS];

//bus variables
#define NUM_BUS 10
float busPosZ = -20.0f;
float busPositionsZ[NUM_BUS];
float busPositionsX[NUM_BUS];
float busColors[NUM_BUS][3];
std::string busNames[NUM_BUS];

//barricade variables
float barricadePositionsZ[NUM_BUS*2];
float barricadePositionsX[NUM_BUS*2];

//player variable
// Player position and jump variables
float playerPlaceX[3] = {-4.5f, 0.0f, 4.5f};
float playerPosX = playerPlaceX[1];
int playerPosIndex = 1;
float playerPosNow = playerPosX;
float playerPosY = -4.0f;
float playerVelocityY = 0.0f;
float playerVelocityX = 0.0f;
float reverseVelocity = 0.0f; // If collision happened
float rotateVelocity = 0.0f;
float totalReverse = 0.0f;
std::string sideMove = "";
std::string sideMove2 = "";
float characterAngle = 0.0f;
float grassPositionZ[6] = {-50.0f, -30.0f, -10.0f, 10.0f, 30.0f, 50.0f};

bool isJumping = false;
bool isMoving = false;
bool isRotating = false;
bool hasCollision = false;

bool isIncreasing = true;

int animationFrame = 0;
float lastUpdateTime = 0.0f;

float circleRadius = 0.5f;


float dayColor[3] = {0.5f, 0.8f, 1.0f}; // Light blue for day
float nightColor[3] = {0.0f, 0.0f, 0.05f}; // Dark blue for night
float currentColor[3] = {0.5f, 0.8f, 1.0f}; 


//function prototypes
void centerScreen();
void init();
void display();
void renderBitmapString(float x, float y, float z, void *font, std::string string);
void drawQuads(float x1, float y1, float z1, 
               float x2, float y2, float z2,
               float x3, float y3, float z3,
               float x4, float y4, float z4,
               GLenum type);

void drawLines(float x1, float y1, float z1,
               float x2, float y2, float z2);

void drawQuadPrism(float x1, float x2, float y1, 
                   float y2, float z1, float x3, 
                   float x4, float y3, float y4, float z2);

void renderPlayerModelArms(int animationFrame);
void updateFrame(int value);
bool checkBusCollision();
bool checkBarricadeCollision();
void handleKeys(unsigned char key, int x, int y);
void handleSpecialKeys(int key, int x, int y);
void animateRoad();
void renderMountain();
void renderBus();
void renderBusWheel(float radius, float width, int slices, float posX, float posY, float posZ);
void handleMouse(int button, int state, int x, int y);
void renderCircle(float x, float y, float radius, int segments);
void renderCity();
void displayTitle(float x, float y, const char *text, void *font);

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    centerScreen();
    glutCreateWindow("C5 Surfer");
    glutFullScreen();
    glewInit();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutMouseFunc(handleMouse);
    glutSpecialFunc(handleSpecialKeys);

    glutTimerFunc(16, updateFrame, 0);

      for (int i = 0; i < 3; ++i) {
        currentColor[i] = dayColor[i];
    }
    GLenum err = glewInit();
    if (err == GLEW_OK){
        glutMainLoop();
    } else {
        std::cout << "Error initializing glew!";
        return 0;
    }
    
}

void centerScreen(){
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    int windowPosX = (screenWidth - 1920) / 2;
    int windowPosY = (screenHeight - 1080) / 2;
    glutInitWindowPosition(windowPosX, windowPosY);
}

void init() //initialize buses and tracks positions
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    std::srand(std::time(0));

    // Initialize bus positions
    for (int i = 0; i < NUM_BUS; ++i)
    {
        busPositionsZ[i] = busPosZ - i * 10.0f;
        barricadePositionsZ[i] = busPosZ - i * 10.0f;
        barricadePositionsZ[i + 10] = busPosZ - i * 10.0f;
        int lane = std::rand() % 3;
        busPositionsX[i] = (lane - 1) * 4.5f;
        int lane2 = (lane + 1) % 3;
        int lane3 = (lane + 2) % 3;
        barricadePositionsX[i] = (lane2 - 1) * 4.5f;
        barricadePositionsX[i + 10] = (lane3 - 1) * 4.5f;
    }

    // Random Bus Color Generator
    for (int i = 0; i < NUM_BUS; ++i)
    {
        busColors[i][0] = static_cast<float>(std::rand()) / RAND_MAX;
        busColors[i][1] = static_cast<float>(std::rand()) / RAND_MAX;
        busColors[i][2] = static_cast<float>(std::rand()) / RAND_MAX;
    }

    for (int i = 0; i < NUM_BUS; ++i)
    {
        busNames[i] = names[std::rand() % 4];
    }

    for (int i = 0; i < NUM_TRACK_SEGMENTS; ++i)
    {
        roadPositionZ[i] = -200.0f + i * 20.0f;
    }
}

void drawQuads(float x1, float x2, float x3, float x4, float y1, float y2, float y3, float y4, float z1, float z2, float z3, float z4, GLenum type){
    float quad_vertices[] = {
		x1, y1, z1,
		x2, y2, z2,
		x3, y3, z3,
		x4, y4, z4
	};

    GLuint VBO_ID;
    glGenBuffers(1, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_DYNAMIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT, 0, 0);
    glDrawArrays(type, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_ID);
}

void drawLines(float x1, float x2, float y1, float y2, float z1, float z2) {
    float line_vertices[] = {
        x1, y1, z1,
        x2, y2, z2
    };

    GLuint VBO_ID;
    glGenBuffers(1, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_DYNAMIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_ID);
}

void drawPoints(float x1, float y1, float z1, float pointSize) {
	float point_vertices[] = {
        x1, y1, z1
    };

	glPointSize(pointSize);

    GLuint VBO_ID;
    glGenBuffers(1, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertices), point_vertices, GL_DYNAMIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT, 0, 0);
    glDrawArrays(GL_POINTS, 0, 1);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_ID);
}

void drawQuadPrism(float x1, float x2, float y1, float y2, float z1, float x3, float x4, float y3, float y4, float z2){
    float quadprism_vertices[] = {
        x1, y2, z1,
        x2, y2, z1,
        x2, y1, z1,
        x1, y1, z1,

        x3, y4, z2,
        x4, y4, z2,
        x4, y3, z2,
        x3, y3, z2,

        x3, y4, z2,
        x1, y2, z1,
        x1, y1, z1,
        x3, y3, z2,

        x3, y3, z2,
        x4, y3, z2,
        x2, y1, z1,
        x1, y1, z1,

        x4, y4, z2,
        x2, y2, z1,
        x2, y1, z1,
        x4, y3, z2,

        x3, y4, z2,
        x4, y4, z2,
        x2, y2, z1,
        x1, y2, z1
    };

    GLuint VBO_ID;
    glGenBuffers(1, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadprism_vertices), quadprism_vertices, GL_DYNAMIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, 24);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_ID);
}

void renderBitmapString(float x, float y, float z, void *font, std::string string) {    
    for(int i = 0; i < string.length(); i++) {
    	glRasterPos3f(x, y, z);
        glutBitmapCharacter(font, string[i]);
        x += 0.065f;
    }
}

void renderPlayerModel(){

    glPushMatrix();
    glTranslatef(playerPosX, playerPosY-2.0f, 0.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    glRotatef(characterAngle, 0.0f, 1.0f, 0.0f);

    // Body (torso)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(0.055f, 0.68f, 0.68f);
    drawQuadPrism(-0.3f, 0.3f, 0.0f, 1.333f, 0.2f, -0.3f, 0.3f, 0.0f, 1.333f, -0.2f);

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 1.334f, 0.0f);
    glColor3f(1.0f, 0.8f, 0.6f);
    drawQuadPrism(-0.3f, 0.3f, 0.0f, 1.067f, 0.3f, -0.3f, 0.3f, 0.0f, 1.067f, -0.3f);

	// Hair
    glColor3ub(139,69,19);
    drawQuads(-0.301f, 0.301f, 0.301f, -0.301f,1.068f, 1.068f, 0.8f, 0.8f,-0.301f, -0.301f, -0.301f, -0.301f, GL_QUADS);
    drawQuads(-0.301f, 0.301f, 0.301f, -0.301f, 1.068f, 1.068f, 0.0f, 0.0f, 0.301f, 0.301f, 0.301f, 0.301f, GL_QUADS);
    drawQuads(0.301f, 0.301f, -0.301f, -0.301f, 1.068f, 1.068f, 1.068f, 1.068f, -0.301f, 0.301f, 0.301f, -0.301f, GL_QUADS);
    drawQuads(-0.301f, -0.301f, -0.301f, -0.301f, 0.8f, 1.068f, 1.068f, 0.8f, 0.301f, 0.301f, -0.301f, -0.301f, GL_QUADS);
	drawQuads(0.301f, 0.301f, 0.301f, 0.301f, 0.8f, 1.068f, 1.068f, 0.8f, 0.301f, 0.301f, -0.301f, -0.301f, GL_QUADS);
	drawQuads(-0.301f, -0.301f, -0.301f, -0.301f, 0.0f, 1.068f, 1.068f, 0.0f, 0.301f, 0.301f, 0.075f, 0.075f, GL_QUADS);
	drawQuads(0.301f, 0.301f, 0.301f, 0.301f, 0.0f, 1.068f, 1.068f, 0.0f, 0.301f, 0.301f, 0.075f, 0.075f, GL_QUADS);

    glColor3f(0.0f, 0.0f, 0.0f);
	drawPoints(-0.15f, 0.57f, -0.34f, 8.0f);
	drawPoints(0.15f, 0.57f, -0.34f, 8.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
	drawPoints(-0.15f, 0.667f, -0.3001f, 15.0f);
	drawPoints(0.15f, 0.667f, -0.3001f, 15.0f);

	glColor3f(0.0f, 0.0f, 0.0f); 
	drawQuads(0.15f, 0.3f, -0.3001f, -0.15f, 0.3f, -0.3001f, -0.1f, 0.2f, -0.3001f, 0.1f, 0.2f, -0.3001f, GL_QUADS);
    glPopMatrix();
    renderPlayerModelArms(animationFrame);
    glPopMatrix(); 
}

void renderPlayerModelArms(int animationFrame){
	
    // Left Arm - Part of the Shirt
    glPushMatrix();
    glTranslatef(-0.301f, 0.0f, 0.0f);
	    glColor3f(0.055f,0.68f,0.68f);
	    drawQuadPrism(-0.3f, 0.0f, 0.667f, 1.333f, 0.2f, -0.3f, 0.0f, 0.667f, 1.333f, -0.2f);
    glPopMatrix();
    
    // Right Arm - Part of the Shirt
    glPushMatrix();
    glTranslatef(0.301f, 0.0f, 0.0f);
	    glColor3f(0.055f,0.68f,0.68f);
	    drawQuadPrism(0.0f, 0.3f, 0.667f, 1.333f, 0.2f, 0.0f, 0.3f, 0.667f, 1.333f, -0.2f);
    glPopMatrix();
    


    if(animationFrame == 0) {
	    // Left Arm - Down
	    glPushMatrix();
	    glTranslatef(-0.301f, 0.666f, 0.0f);
		    glColor3f(1.0f, 0.8f, 0.6f);
		    drawQuadPrism(-0.3f, 0.0f, -1.333f, 0.0f, 0.2f, -0.3f, 0.0f, -1.333f, 0.0f, -0.2f);
	    glPopMatrix();
	
	    // Right Arm - Raised
	    glPushMatrix();
	    glTranslatef(0.301f, 0.0f, -0.201f);
		    glColor3f(1.0f, 0.8f, 0.6f);
		    drawQuadPrism(0.0f, 0.3f, 0.667f, 1.333f, 0.0f, 0.0f, 0.3f,  0.667f, 1.333f, -0.6f);    
	    glPopMatrix();
	    
	    // Right Leg - Down
	    glPushMatrix();
	    glTranslatef(0.0, -0.001f, 0.0f);
	    
		    // Pants
		    glColor3f(0.29f, 0.27f, 0.59f);
		    drawQuadPrism(0.0f, 0.3f, -1.333f, 0.0f, 0.2f, 0.0f, 0.3f, -1.333f, 0.0f, -0.2f);   
		     
			// Shoes
		    glColor3ub(49,48,43);
		    drawQuadPrism(0.0f, 0.3f, -1.601f, -1.333f, 0.2f, 0.0f, 0.3f, -1.601f, -1.333f, -0.2f); 
	    glPopMatrix();
	    
	    // Left Leg - Raised
	    glPushMatrix();
	    glTranslatef(0.0f, -0.001f, 0.0f);
		    // Pants
		    glColor3f(0.29f, 0.27f, 0.59f);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -0.2f, -0.2f, 0.2f, 0.2f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, -1.0f, -1.0f, -1.2f, -1.2f, -0.7f, -0.7f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(-0.3f, -0.3f, -0.3f, -0.3f, 0.0f, -1.0f, -1.2f, 0.0f, -0.2f, -0.7f, -0.3f, 0.2f, GL_QUADS);
	        drawQuads(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.2f, 0.0f, -0.2f, -0.7f, -0.3f, 0.2f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, 0.0f, 0.0f, -1.2f, -1.2f, 0.2f, 0.2f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, 0.0f, 0.0f, -1.0f, -1.0f, -0.2f, -0.2f, -0.7f, -0.7f, GL_QUADS);
		    
		    // Shoes
		    glColor3ub(49,48,43);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, -1.001f, -1.001f, -1.201f, -1.201f, -0.7f, -0.7f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, -1.301f, -1.301f, -1.501f, -1.501f, -0.825f, -0.825f, -0.425f, -0.425f, GL_QUADS);
	        drawQuads(-0.3f, -0.3f, -0.3f, -0.3f, -1.001f, -1.301f, -1.501f, -1.201f, -0.7f, -0.825f, -0.425f, -0.3f, GL_QUADS);
	        drawQuads(0.0f, 0.0f, 0.0f, 0.0f, -1.001f, -1.301f, -1.501f, -1.201f, -0.7f, -0.825f, -0.425f, -0.3f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, -1.001f, -1.001f, -1.301f, -1.301f, -0.7f, -0.7f, -0.825f, -0.825f, GL_QUADS);
	        drawQuads(-0.3f, 0.0f, 0.0f, -0.3f, -1.201f, -1.201f, -1.501f, -1.501f, -0.3f, -0.3f, -0.425f, -0.425f, GL_QUADS);
	    glPopMatrix();
	    
	} else if(animationFrame == 1){
		// Left Arm - Raised
	    glPushMatrix();
	    glTranslatef(-0.301f, 0.0f, -0.201f);
		    glColor3f(1.0f, 0.8f, 0.6f);
		    drawQuadPrism(-0.3f, 0.0f, 0.667f, 1.333f, 0.0f, -0.3f, 0.0f,  0.667f, 1.333f, -0.6f);  
	    glPopMatrix();
	
	    // Right Arm - Down
	    glPushMatrix();
	    glTranslatef(0.301f, 0.666f, 0.0f);
	    glColor3f(1.0f, 0.8f, 0.6f);
	    drawQuadPrism(0.0f, 0.3f, -1.333f, 0.0f, 0.2f, 0.0f, 0.3f, -1.333f, 0.0f, -0.2f);	 
	    glPopMatrix();
	    
	    // Right Leg - Raised
	    glPushMatrix();
	    glTranslatef(0.0, -0.001f, 0.0f);
	    glColor3f(0.29f, 0.27f, 0.59f);
	    	// Pants
		    glColor3f(0.29f, 0.27f, 0.59f);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -0.2f, -0.2f, 0.2f, 0.2f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, -1.0f, -1.0f, -1.2f, -1.2f, -0.7f, -0.7f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(0.3f, 0.3f, 0.3f, 0.3f, 0.0f, -1.0f, -1.2f, 0.0f, -0.2f, -0.7f, -0.3f, 0.2f, GL_QUADS);
	        drawQuads(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.2f, 0.0f, -0.2f, -0.7f, -0.3f, 0.2f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, 0.0f, 0.0f, -1.2f, -1.2f, 0.2f, 0.2f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, 0.0f, 0.0f, -1.0f, -1.0f, -0.2f, -0.2f, -0.7f, -0.7f, GL_QUADS);
		    
		    // Shoes
		    glColor3ub(49,48,43);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, -1.001f, -1.001f, -1.201f, -1.201f, -0.7f, -0.7f, -0.3f, -0.3f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, -1.301f, -1.301f, -1.501f, -1.501f, -0.825f, -0.825f, -0.425f, -0.425f, GL_QUADS);
	        drawQuads(0.3f, 0.3f, 0.3f, 0.3f, -1.001f, -1.301f, -1.501f, -1.201f, -0.7f, -0.825f, -0.425f, -0.3f, GL_QUADS);
	        drawQuads(0.0f, 0.0f, 0.0f, 0.0f, -1.001f, -1.301f, -1.501f, -1.201f, -0.7f, -0.825f, -0.425f, -0.3f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, -1.001f, -1.001f, -1.301f, -1.301f, -0.7f, -0.7f, -0.825f, -0.825f, GL_QUADS);
	        drawQuads(0.3f, 0.0f, 0.0f, 0.3f, -1.201f, -1.201f, -1.501f, -1.501f, -0.3f, -0.3f, -0.425f, -0.425f, GL_QUADS);
	    glPopMatrix();
	    
	    // Left Leg - Down
	    glPushMatrix();
	    glTranslatef(0.0f, -0.001f, 0.0f);
	    glColor3f(0.29f, 0.27f, 0.59f);
	    drawQuadPrism(-0.3f, 0.0f, -1.333f, 0.0f, 0.2f, -0.3f, 0.0f, -1.333f, 0.0f, -0.2f);
	    
	    // Shoes
	    glColor3ub(49,48,43);
	    drawQuadPrism(-0.3f, 0.0f, -1.601f, -1.333f, 0.2f, -0.3f, 0.0f, -1.601f, -1.333f, -0.2f); 
	    glPopMatrix();
	}
}

bool checkBusCollision(){
        float collisionThreshold = 1.0f;
    const float epsilon = 0.01f;

    for (int i = 0; i < NUM_BUS; ++i) {
        if (fabs(playerPosX - busPositionsX[i]) < epsilon) {
            if (busPositionsZ[i] > playerPosNow && busPositionsZ[i] < playerPosNow + collisionThreshold) {
                return true; // Collision detected
            }
        }
    }

    return false; // No collision
}

bool checkBarricadeCollision() {
    // Ignore barricade collisions while jumping
    if (isJumping) {
        return false;
    }

    float xThreshold = 0.6f;
    float zThreshold = 0.15f;
    float yThreshold = 0.2f;

    for (int i = 0; i < 20; ++i) {
        if (playerPosX > barricadePositionsX[i] - xThreshold && playerPosX < barricadePositionsX[i] + xThreshold) {
            if (fabs(playerPosY - barricadePositionsZ[i]) < zThreshold) {
                if (playerPosY <= barricadePositionsZ[i] + yThreshold) {
                    return true; // Collision detected only when close enough but not too close
                }
            }
        }
    }
    return false; // No collision
}

void updateFrame(int value) {
	
	for(int i = 0; i < 6; i++){
    	grassPositionZ[i] += 0.1f;
    	if(grassPositionZ[i] > 70.0f) {
    		grassPositionZ[i] = -50.0f;
		}
	}
	
    // Update bus and barricade positions
    for (int i = 0; i < 10; ++i) {
        busPositionsZ[i] += 0.1f;
        barricadePositionsZ[i] += 0.1f;
        barricadePositionsZ[i + 10] += 0.1f;
        if (busPositionsZ[i] > 10.0f) {
            busPositionsZ[i] = -90.0f;
            barricadePositionsZ[i] = -90.0f;
            barricadePositionsZ[i + 10] = -90.0f;
            int lane = std::rand() % 3;
            busPositionsX[i] = (lane - 1) * 4.5f;
            int lane2 = (lane + 1) % 3;
            int lane3 = (lane + 2) % 3;
            barricadePositionsX[i] = (lane2 - 1) * 4.5f;
            barricadePositionsX[i + 10] = (lane3 - 1) * 4.5f;
        }
    }

    // Handle Collision Response
    if (hasCollision) {
        reverseVelocity += 0.05f;
        for (int i = 0; i < 10; ++i) {
            busPositionsZ[i] -= reverseVelocity;
            barricadePositionsZ[i] -= reverseVelocity;
            barricadePositionsZ[i + 10] -= reverseVelocity;
        }
        totalReverse += reverseVelocity;
        if (totalReverse > 30.0f) {
            hasCollision = false;
            reverseVelocity = 0.0f;
            totalReverse = 0.0f;
        }
    }

    // Update player jump
    if (isJumping) {
        playerVelocityY -= 0.03f;
        playerPosY += playerVelocityY;
        if (playerPosY <= -4.0f) {
            playerPosY = -4.0f;
            isJumping = false;
            playerVelocityY = 0.0f;
        }
    }

    // Handle player rotation
    if (isRotating) {
        if (sideMove2 == "left") {
            characterAngle += rotateVelocity;  // Rotate Counter-clockwise
            if (characterAngle > 360) {
                isRotating = false;
                characterAngle = 0.0f;
                sideMove2 = "";
            }
        } else if (sideMove2 == "right") {
            characterAngle -= rotateVelocity;  // Rotate Clockwise
            if (characterAngle < -360) {
                isRotating = false;
                characterAngle = 0.0f;
                sideMove2 = "";
            }
        }
    }

    // Check for bus collision
    if (checkBusCollision()) {
        std::cout << "Collision with bus detected!" << std::endl;
        hasCollision = true;
        reverseVelocity = 2.0f;
        if (playerPosX == playerPlaceX[0]) {
            playerPosIndex++;
            playerPosNow = playerPlaceX[playerPosIndex]; // Update position
            isMoving = true;
            sideMove = "right";
            playerVelocityX = 0.5f;
        } else if (playerPosX == playerPlaceX[2]) {
            playerPosIndex--;
            playerPosNow = playerPlaceX[playerPosIndex]; // Update position
            isMoving = true;
            sideMove = "left";
            playerVelocityX = 0.5f;
        }
    }

    // Check for barricade collision
    if (checkBarricadeCollision()) {
        std::cout << "Collision with barricade detected!" << std::endl;
        hasCollision = true;
        reverseVelocity = 2.0f;
    }

    // Update player movement
    if (isMoving) {
        if (sideMove == "left") {
            playerPosX -= playerVelocityX; // Move left
            if (playerPosX <= playerPlaceX[playerPosIndex]) {
                playerPosX = playerPlaceX[playerPosIndex];
                isMoving = false;
                playerVelocityX = 0.0f;
                sideMove = "";
            }
        } else if (sideMove == "right") {
            playerPosX += playerVelocityX; // Move Right
            if (playerPosX >= playerPlaceX[playerPosIndex]) {
                playerPosX = playerPlaceX[playerPosIndex];
                isMoving = false;
                playerVelocityX = 0.0f;
                sideMove = "";
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateFrame, 0);
}

void handleKeys(unsigned char key, int x, int y) {
	// Player - Jump
	if (key == ' ') {
		if (!isJumping) {
			isJumping = true;
			playerVelocityY = 0.9f;
		}
	} else if (key == 'a') {
        // Player - Move Left
        if (playerPosIndex > 0) {
            playerPosIndex--;
            playerPosNow = playerPlaceX[playerPosIndex];
            isMoving = true;
            sideMove = "left";
            playerVelocityX = 0.5f;
        }
    } else if (key == 'd') {
        // Player - Move Right
        if (playerPosIndex < 2) {
            playerPosIndex++;
            playerPosNow = playerPlaceX[playerPosIndex];
            isMoving = true;
            sideMove = "right";
            playerVelocityX = 0.5f;
        }
    }
}

void handleSpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        // Player - Move Left
        if (playerPosIndex > 0) {
            playerPosIndex--;
            playerPosNow = playerPlaceX[playerPosIndex];
            isMoving = true;
            sideMove = "left";
            playerVelocityX = 0.5f;
        }
    } else if (key == GLUT_KEY_RIGHT) {
        // Player - Move Right
        if (playerPosIndex < 2) {
            playerPosIndex++;
            playerPosNow = playerPlaceX[playerPosIndex];
            isMoving = true;
            sideMove = "right";
            playerVelocityX = 0.5f;
        }
    }
}

void handleMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            // Player - Rotate Counter-clockwise
            rotateVelocity = 10.0f;
            isRotating = true;
            sideMove2 = "left";
        } else if (button == GLUT_RIGHT_BUTTON) {
            // Player - Rotate Clockwise
            rotateVelocity = 10.0f;
            isRotating = true;
            sideMove2 = "right";
        } else if (button == 3) {  // Scroll up
            circleRadius += 1.5f;   // Increase the sun radius
            if (circleRadius > 5.0f) circleRadius = 5.0f;
        } else if (button == 4) {  // Scroll down
            circleRadius -= 1.5f;   // Decrease the sun radius
            if (circleRadius < 0.5f) circleRadius = 0.5f; // Prevent negative radius
        }

        // Change Sky Color based on Sun's radius
        float t = (circleRadius - 0.1f) / 10.0f;
        t = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
        for (int i = 0; i < 3; ++i) {
            currentColor[i] = (1.0f - t) * dayColor[i] + t * nightColor[i];
        }
    }
}

void renderRoad()
{
    // Render tracks
      for (int i = 0; i < NUM_TRACK_SEGMENTS; ++i) {
    	glColor3f(0.25f, 0.25f, 0.25f);
    	
        glPushMatrix();
        glTranslatef(0.0f, -10.0f, roadPositionZ[i]);
        glScalef(0.1f, 0.1f, 100.0f);
        
	        drawQuads(-25.6f, 25.6f, 25.6f, -25.6f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, GL_QUADS);
			drawQuads(-80.0f, -35.0f, -35.0f, -80.0f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, GL_QUADS);
			drawQuads(80.0f, 35.0f, 35.0f, 80.0f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, GL_QUADS);
			
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0.0f, -9.5f, roadPositionZ[i]);
        glScalef(0.1f, 0.1f, 1.0f);
        	
	        glColor3f(1.0f, 1.0f, 1.0f);
	        drawQuads(-60.0f, -50.0f, -50.0f, -60.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, GL_QUADS);
	        drawQuads(-5.0f, 5.0f, 5.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, GL_QUADS);
	        drawQuads(60.0f, 50.0f, 50.0f, 60.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, GL_QUADS);

        glPopMatrix();
    }

    glColor3f(0.15f, 0.15f, 0.15f);

    glPushMatrix();
    glTranslatef(-3.0f, -10.7f, 0.0f);
    glScalef(0.2f, 0.3f, 50.0f);
    glutSolidCube(6);
    glPopMatrix();

    // Draw the inner right rail (long, thin rectangle)
    glPushMatrix();
    glTranslatef(3.0f, -10.7f, 0.0f);
    glScalef(0.2f, 0.3f, 50.0f);
    glutSolidCube(6);
    glPopMatrix();
    animateRoad();
    glColor3f(0.25f, 0.25f, 0.25f);
    
}

void animateRoad() {
    for (int i = 0; i < NUM_TRACK_SEGMENTS; ++i) {
        roadPositionZ[i] += 0.1f; // Move tracks forward
        if (roadPositionZ[i] > 10.0f) {
            roadPositionZ[i] = -200.0f; // Move track back to the beginning
        }
    }
}

void renderMountain()
{
    for (int i = 0; i < NUM_TRACK_SEGMENTS; ++i)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, roadPositionZ[i]);
        glColor3ub(69, 48, 38);//darker brown
        drawQuads(-20.0f, -20.0f, -8.0f, -8.0f, 1.0f, 1.0f, -10.0f, -10.0f, 0.0f, -10.0f, 0.0f, -10.0f, GL_QUADS);
        glColor3ub(92, 64, 51);//lighter brown
        drawQuads(-20.0f, -20.0f, -8.0f, -8.0f, 1.0f, 1.0f, -10.2f, -10.2f, 0.0f, 100.0f, 100.0f, 0.0f, GL_QUADS); // left mountain wall
        glColor3ub(69, 48, 38);
        drawQuads(20.0f, 20.0f, 8.0f, 8.0f, 1.0f, 1.0f, -10.0f, -10.0f, 10.0f, -1.0f, -4.0f, -13.0f, GL_QUADS);
        glColor3ub(92, 64, 51);
        drawQuads(20.0f, 20.0f, 8.0f, 8.0f, 1.0f, 1.0f, -10.2f, -10.2f, 0.0f, -100.0f, 100.0f, 0.0f, GL_QUADS); // right mountain wall
        glPopMatrix();
    }
    glPushMatrix();
    glColor3ub(0,  0, 0);
    glScalef(100.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, -0.9f, 0.0f);
    drawQuads(-1.0f, 1.0f, 1.0f, -1.0f, 2.0f, 2.0f, -10.0f, -10.0f, -70.0f, -70.0f, -70.0f, -70.0f, GL_QUADS);
    glPopMatrix();

}

void renderCity()
{
    float heights[] = {20.0f, 18.0f, 21.0f, 14.0f, 18.0f, 25.0f, 17.0f, 22.0f, 13.0f, 21.0f, 19.0f, 16.0f, 14.0f, 23.0f, 24.0f, 25.0f, 17.0f, 22.0f};
    int numBuildings = sizeof(heights) / sizeof(heights[0]);

    glPushMatrix();
    glTranslatef(6.0f, -10.0f, -80.0f);
    glScalef(1.1f, 1.1f, 0.0f);
    for (int i = 0; i < numBuildings; ++i)
    {
        float x = 7.0f - i * 2.0f;
        float y = heights[i];
        glColor3f(0.1f, 0.1f, 0.1f);
        drawQuads(x, x, x - 2.0f, x - 2.0f, 0.0f, y, y, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS);

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        glVertex3f(x, 0.0f, 0.001f);
        glVertex3f(x, y, 0.001f);
        glVertex3f(x - 2.0f, y, 0.001f);
        if (i < numBuildings - 1)
        {
            glVertex3f(x - 2.0f, heights[i + 1], 0.001f);
        }
        else
        {
            glVertex3f(x - 2.0f, 0.0f, 0.001f);
        }
        glEnd();

        float tempY = y - 0.5f;
        glColor3ub(240, 230, 140);
        while (tempY > 1.0f)
        {
            drawQuads(x - 0.2f, x - 0.2f, x - 0.8f, x - 0.8f, tempY, tempY - 0.6f, tempY - 0.6f, tempY, 5.1f, 5.1f, 5.1f, 5.1f, GL_QUADS);
            drawQuads(x - 1.2f, x - 1.2f, x - 1.8f, x - 1.8f, tempY, tempY - 0.6f, tempY - 0.6f, tempY, 5.1f, 5.1f, 5.1f, 5.1f, GL_QUADS);
            tempY -= 1.5f;
        }
    }
    glPopMatrix();

    float heights2[] = {22.0f, 17.0f, 25.0f, 24.0f, 23.0f, 14.0f, 16.0f, 19.0f, 21.0f, 13.0f, 22.0f, 17.0f, 25.0f, 18.0f, 14.0f, 21.0f, 18.0f, 20.0f};
    glPushMatrix();
    glTranslatef(5.0f, -10.0f, -81.0f);
    glScalef(1.25f, 1.25f, 0.0f);
    for (int i = 0; i < numBuildings; ++i)
    {
        float x = 7.0f - i * 2.0f;
        float y = heights2[i];
        glColor3f(0.0f, 0.0f, 0.0f);
        drawQuads(x, x, x - 2.0f, x - 2.0f, 0.0f, y, y, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS);

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        glVertex3f(x, 0.0f, 0.001f);
        glVertex3f(x, y, 0.001f);
        glVertex3f(x - 2.0f, y, 0.001f);
        if (i < numBuildings - 1)
        {
            glVertex3f(x - 2.0f, heights2[i + 1], 0.001f);
        }
        else
        {
            glVertex3f(x - 2.0f, 0.0f, 0.001f);
        }
        glEnd();

        float tempY = y - 0.5f;
        glColor3f(1.0f, 1.0f, 0.0f);
        while (tempY > 1.0f)
        {
            drawQuads(x - 0.2f, x - 0.2f, x - 0.8f, x - 0.8f, tempY, tempY - 0.6f, tempY - 0.6f, tempY, 5.1f, 5.1f, 5.1f, 5.1f, GL_QUADS);
            drawQuads(x - 1.2f, x - 1.2f, x - 1.8f, x - 1.8f, tempY, tempY - 0.6f, tempY - 0.6f, tempY, 5.1f, 5.1f, 5.1f, 5.1f, GL_QUADS);
            tempY -= 1.5f;
        }
    }
    glPopMatrix();
}

void renderBus()
{
    for (int i = 0; i < NUM_BUS; ++i)
    {
        // Bus body
        glPushMatrix();
        glTranslatef(busPositionsX[i], -4.5f, busPositionsZ[i]);
        glScalef(1.0f, 1.0f, 3.0f);
        glColor3f(busColors[i][0], busColors[i][1], busColors[i][2]);
        glutSolidCube(3.0);
        glPopMatrix();

        float wheelRadius = 1.0f;
        float wheelWidth = 0.5f;
        int wheelSlices = 20;

        for (int j = 0; j < NUM_BUS; ++j)
        {
            float busX = busPositionsX[j];
            float busZ = busPositionsZ[j];
            float busY = -6.5f;

            renderBusWheel(wheelRadius, wheelWidth, wheelSlices, busX - 1.26f, busY, busZ + 3.0f);
            renderBusWheel(wheelRadius, wheelWidth, wheelSlices, busX + 1.26f, busY, busZ + 3.0f);
            renderBusWheel(wheelRadius, wheelWidth, wheelSlices, busX - 1.26f, busY, busZ - 3.0f);
            renderBusWheel(wheelRadius, wheelWidth, wheelSlices, busX + 1.26f, busY, busZ - 3.0f);
        }

        glPushMatrix();
        glTranslatef(busPositionsX[i], -4.5f, busPositionsZ[i] + 4.6f);
        glScalef(1.0f, 0.5f, 0.1f);

        glColor3f(0.5f, 0.5f, 0.5f);

        drawQuads(-1.25f, -0.125f, -0.125f, -1.25f, 1.5f, 1.5f, -0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS); // Front Window (Left Panel)
        drawQuads(-1.25f, -0.75f, -0.75f, -1.25f, -1.25f, -1.25f, -2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS); // Front Window (Left Blinker)

        drawQuads(1.25f, 0.125f, 0.125f, 1.25f, 1.5f, 1.5f, -0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS); // Front Window (Right Panel)
        drawQuads(1.25f, 0.75f, 0.75f, 1.25f, -1.25f, -1.25f, -2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_QUADS); // Front Window (Right Blinker)

        // Front Window (Left Panel Border)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(-1.25f, -0.125f, -0.125f, -1.25f, 1.5f, 1.5f, -0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 0.0f, GL_LINE_LOOP);

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(4.0f);
        drawLines(-0.2f, -0.7f, 1.4f, -0.65f, 0.001f, 0.001f); // Front Window (Left Panel Shine)
        drawLines(-0.7f, -1.2f, 1.4f, -0.65f, 0.001f, 0.001f);
        drawLines(0.7f, 0.2f, 1.4f, -0.65f, 0.001f, 0.001f); // Front Window (Right Panel Shine)
        drawLines(1.2f, 0.7f, 1.4f, -0.65f, 0.001f, 0.001f);

        // Front Window (Right Panel Border)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(1.25f, 0.125f, 0.125f, 1.25f, 1.5f, 1.5f, -0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 0.0f, GL_LINE_LOOP);

        // Front Window (Left Blinker Border)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(-1.25f, -0.75f, -0.75f, -1.25f, -1.25f, -1.25f, -2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_LINE_LOOP);

        // Front Window (Right Blinker Border)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(1.25f, 0.75f, 0.75f, 1.25f, -1.25f, -1.25f, -2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, GL_LINE_LOOP);

        // Front Window (Blinker Lights)
        glColor3f(1.0f, 1.0f, 0.0f);
        drawPoints(-1.0f, -1.625f, 0.0f, 8.0f);
        drawPoints(1.0f, -1.625f, 0.0f, 8.0f);

        glPopMatrix();

        // Side of the Bus Design
        glPushMatrix();
        glTranslatef(busPositionsX[i] - 1.6f, -4.5f, busPositionsZ[i] + 1.0f);
        glScalef(1.0f, 0.5f, 0.1f);

        // Windows - Left Side
        glColor3f(0.5f, 0.5f, 0.5f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, -0.25f, 2.0f, 2.0f, -0.25f, 30.0f, 30.0f, 10.0f, 10.0f, GL_QUADS);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, -0.25f, 2.0f, 2.0f, -0.25f, -20.0f, -20.0f, -40.0f, -40.0f, GL_QUADS);

        // Doorway
        glColor3f(0.25f, 0.25f, 0.25f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, -2.0f, 2.5f, 2.5f, -2.0f, 5.0f, 5.0f, -15.0f, -15.0f, GL_QUADS);

        // Window Border
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.0f, 2.0f, -0.25f, -0.25f, 30.0f, 10.0f, 10.0f, 30.0f, GL_LINE_LOOP);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.0f, 2.0f, -0.25f, -0.25f, -20.0f, -40.0f, -40.0f, -20.0f, GL_LINE_LOOP);
        // End of Window Border

        // Doorway Border
        glLineWidth(3.0f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.5f, 2.5f, -2.0f, -2.0f, -5.0f, 5.0f, 5.0f, -5.0f, GL_LINE_LOOP);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.5f, 2.5f, -2.0f, -2.0f, -15.0f, -5.0f, -5.0f, -15.0f, GL_LINE_LOOP);
        // End of Doorway Border

        // Doorhandle
        glColor3f(1.0f, 1.0f, 1.0f);
        drawPoints(0.09f, 0.25f, -8.5f, 8.0f);
        drawPoints(0.09f, 0.25f, -1.5f, 8.0f);

        glPopMatrix();

        // Windows - Right Side
        glPushMatrix();
        glTranslatef(busPositionsX[i] + 1.45f, -4.5f, busPositionsZ[i] + 1.0f);
        glScalef(1.0f, 0.5f, 0.1f);

        glColor3f(0.5f, 0.5f, 0.5f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, -0.25f, 2.0f, 2.0f, -0.25f, 30.0f, 30.0f, 0.0f, 0.0f, GL_QUADS);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, -0.25f, 2.0f, 2.0f, -0.25f, -10.0f, -10.0f, -40.0f, -40.0f, GL_QUADS);

        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.0f, 2.0f, -0.25f, -0.25f, 30.0f, 0.0f, 0.0f, 30.0f, GL_LINE_LOOP);
        drawQuads(0.09f, 0.09f, 0.09f, 0.09f, 2.0f, 2.0f, -0.25f, -0.25f, -10.0f, -40.0f, -40.0f, -10.0f, GL_LINE_LOOP);

        glPopMatrix();

        // Roof
        glPushMatrix();
        glTranslatef(busPositionsX[i], -1.5f, busPositionsZ[i]);
        glScalef(3.0f, 2.0f, 1.0f); // Roof dimensions, scale appropriately for width, height, and depth

        // Roof Border
        glColor3f(0.9f, 0.9f, 0.9f);
        drawQuadPrism(-0.3f, 0.3f, -0.75f, -0.2f, 4.5f, -0.3f, 0.3f, -0.75f, -0.2f, -4.5f);

        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(5);
        drawLines(-0.3f, 0.3f, -0.19f, -0.19f, 4.6f, 4.6f);
        drawLines(0.3f, -0.3f, -0.74f, -0.74f, 4.6f, 4.6f);
        drawLines(0.3f, 0.3f, -0.19f, -0.74f, 4.6f, 4.6f);
        drawLines(-0.3f, -0.3f, -0.74f, -0.19f, 4.6f, 4.6f);
        drawLines(-0.3f, 0.3f, -0.19f, -0.19f, -4.6f, -4.6f);
        drawLines(0.3f, -0.3f, -0.74f, -0.74f, -4.6f, -4.6f);
        drawLines(0.3f, 0.3f, -0.19f, -0.74f, -4.6f, -4.6f);
        drawLines(-0.3f, -0.3f, -0.19f, -0.74f, -4.6f, -4.6f);
        drawLines(0.3f, 0.3f, -0.19f, -0.19f, -4.6f, 4.6f);
        drawLines(0.3f, 0.3f, -0.74f, -0.74f, -4.6f, 4.6f);
        drawLines(-0.3f, -0.3f, -0.19f, -0.19f, -4.6f, 4.6f);
        drawLines(-0.3f, -0.3f, -0.74f, -0.74f, -4.6f, 4.6f);
        // End of Roof Border

        glPopMatrix(); // End of the roof transformation

        if (busPositionsZ[i] > -10.0f)
        {
            glPushMatrix();
            glTranslatef(busPositionsX[i], -1.5f, busPositionsZ[i]);
            glScalef(3.0f, 2.0f, 1.0f);
            glColor3f(0.0f, 0.0f, 0.0f);
            renderBitmapString(-0.20f, -0.55f, 4.575f, GLUT_BITMAP_HELVETICA_18, busNames[i]);
            glPopMatrix();
        }
    
        // Barricade
        glPushMatrix();
        glTranslatef(barricadePositionsX[i], -6.0f, barricadePositionsZ[i] + 1.5f);

        // Barricade - Top
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuads(-1.5f, 1.5f, 1.5f, -1.5f, 0.8f, 0.8f, 0.0f, 0.0f, 1.5f, 1.5f, 1.5f, 1.5f, GL_QUADS);

        // Barricade - Stripes
        glColor3f(1.0f, 1.0f, 0.0f);
        drawQuads(-1.0f, -0.8f, -1.0f, -1.2f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-0.6f, -0.4f, -0.6f, -0.8f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-0.2f, 0.0f, -0.2f, -0.4f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(0.2f, 0.4f, 0.2f, 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(0.6f, 0.8f, 0.6f, 0.4f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(1.0f, 1.2f, 1.0f, 0.8f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);

        // Barricade - Stand
        glColor3f(0.0f, 0.0f, 0.0f);
        drawQuads(1.5f, 1.3f, 1.3f, 1.5f, 0.8f, 0.8f, -1.5f, -1.5f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-1.5f, -1.3f, -1.3f, -1.5f, 0.8f, 0.8f, -1.5f, -1.5f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);

        glPopMatrix();

        glPushMatrix();
        glTranslatef(barricadePositionsX[i + 10], -6.0f, barricadePositionsZ[i + 10] + 1.5f);
        // Barricade - Top
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuads(-1.5f, 1.5f, 1.5f, -1.5f, 0.8f, 0.8f, 0.0f, 0.0f, 1.5f, 1.5f, 1.5f, 1.5f, GL_QUADS);

        // Barricade - Stripes
        glColor3f(1.0f, 1.0f, 0.0f);
        drawQuads(-1.0f, -0.8f, -1.0f, -1.2f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-0.6f, -0.4f, -0.6f, -0.8f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-0.2f, 0.0f, -0.2f, -0.4f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(0.2f, 0.4f, 0.2f, 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(0.6f, 0.8f, 0.6f, 0.4f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(1.0f, 1.2f, 1.0f, 0.8f, 0.8f, 0.8f, 0.0f, 0.0f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);

        // Barricade - Stand
        glColor3f(0.0f, 0.0f, 0.0f);
        drawQuads(1.5f, 1.3f, 1.3f, 1.5f, 0.8f, 0.8f, -1.5f, -1.5f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);
        drawQuads(-1.5f, -1.3f, -1.3f, -1.5f, 0.8f, 0.8f, -1.5f, -1.5f, 1.501f, 1.501f, 1.501f, 1.501f, GL_QUADS);

        glPopMatrix();
    }
}

void renderBusWheel(float radius, float width, int slices, float posX, float posY, float posZ) {
    const float PI = 3.14159265359f;
    
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Draw the cylindrical wheel body
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
    	glColor3f(0.0f, 0.0f, 0.0f);  
        float angle = 2.0f * PI * i / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glVertex3f(x, y + 0.5f, -width / 2.0f); // Back face
        glVertex3f(x, y + 0.5f, width / 2.0f);  // Front face
    }
    glEnd();

    // Draw the front face of the wheel
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.5f, width / 2.0f); // Center of the front face
    for (int i = 0; i <= slices; ++i) {
        glColor3f(0.75f, 0.75f, 0.75f);
        float angle = 2.0f * PI * i / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glVertex3f(x, y + 0.5f, width / 2.0f);
    }
    glEnd();

    // Draw the back face of the wheel
    glBegin(GL_TRIANGLE_FAN); 
    glVertex3f(0.0f, 0.5f, -width / 2.0f); // Center of the back face
    for (int i = 0; i <= slices; ++i) {
    	glColor3f(0.75f, 0.75f, 0.75f); 
        float angle = 2.0f * PI * i / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glVertex3f(x, y + 0.5f, -width / 2.0f);
    }
    glEnd();

    glPopMatrix();
}

void renderCircle(float x, float y, float radius, int segments){
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void displayTitle(float x, float y, const char *text, void *font){
glMatrixMode(GL_PROJECTION);  
    glPushMatrix(); 
    glLoadIdentity();  
    glOrtho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW); 
    glPushMatrix();  
    glLoadIdentity(); 

    glColor3f(1.0f, 1.0f, 1.0f); 

    glRasterPos2f(x, y);  
    while (*text) {  
        glutBitmapCharacter(font, *text); 
        text++;  
    }

    glPopMatrix();  
    glMatrixMode(GL_PROJECTION);  
    glPopMatrix();  
    glMatrixMode(GL_MODELVIEW); 
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    displayTitle(50.0f, 50.0f, "C5 Surfers!", GLUT_BITMAP_TIMES_ROMAN_24);
    // Set up perspective projection
    gluPerspective(60.0, 1.0, 1.0, 100.0);
    gluLookAt(0.0, 2.0, 15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glClearColor(currentColor[0], currentColor[1], currentColor[2], 1.0f);
    glPushMatrix();
    glTranslatef(20.0f, 20.0f,-70.0f); // Position the sun at the top-right corner and behind objects
    glColor3f(1.0f, 1.0f, 1.0f); 
    renderCircle(0.0f, 0.0f, circleRadius, 100); // Draw the sun with adjustable radius
    glPopMatrix();

    float currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    if (currentTime - lastUpdateTime > 0.25f) {
    	animationFrame = (animationFrame + 1) % 2;
        lastUpdateTime = currentTime;
    }

  
    renderCity();
    renderMountain();
    renderPlayerModel();
    renderRoad();
    renderBus();
    glutSwapBuffers();
}
