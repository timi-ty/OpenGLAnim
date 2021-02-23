#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
const double TWO_PI = 6.2831853;
GLsizei winWidth = 1280, winHeight = 720; // Initial display window size.
GLuint regHex; // Define name for display list.
static GLfloat rotTheta = 0.0;
static GLint old_t = 0;
int currentPose;
float frameTimeElapsed;
float translation;
class scrPt {
public:
	GLint x, y;
};
class PoseAngles {
public:
	GLfloat upperRightArm, lowerRightArm, 
		upperLeftArm, lowerLeftArm, 
		upperRightLeg, lowerRightLeg, 
		upperLeftLeg, lowerLeftLeg;
	PoseAngles() { upperRightArm = lowerRightArm = upperLeftArm = lowerLeftArm = upperRightLeg = lowerRightLeg = upperLeftLeg = lowerLeftLeg = 0; }
	PoseAngles(GLfloat a1, GLfloat a2, GLfloat b1, GLfloat b2, GLfloat c1, GLfloat c2, GLfloat d1, GLfloat d2) {
		upperLeftArm = a1;
		lowerLeftArm = a2;
		upperRightArm = b1;
		lowerRightArm = b2;
		upperLeftLeg = c1;
		lowerLeftLeg = c2;
		upperRightLeg = d1;
		lowerRightLeg = d2;
	}
};
static PoseAngles keyFrames[4];
static void strikePose(PoseAngles);
static PoseAngles lerpPose(float);
static float lerp(float, float, float);
static void init(void)
{
	keyFrames[0] = PoseAngles(10, 175, 5, 160, 5, 175, 5, 175);
	keyFrames[1] = PoseAngles(-20, 165, -10, 175, -45, 125, -5, 175);
	//keyFrames[2] = PoseAngles(0, 180, 0, 180, 0, 180, 0, 180);
	keyFrames[2] = PoseAngles(-5, 160, -10, 175, -5, 175, -5, 175);
	keyFrames[3] = PoseAngles(10, 175, 20, 165, 5, 175, 45, 125);

	strikePose(keyFrames[0]);

	old_t = glutGet(GLUT_ELAPSED_TIME);
}

static void strikePose(PoseAngles poseAngles) {
	scrPt hexVertex;
	GLdouble hexTheta;
	GLint k;
	GLint segmentCount = 360;
	glClearColor(1.0, 1.0, 1.0, 0.0);

	regHex = glGenLists(1);
	glNewList(regHex, GL_COMPILE);
	glColor3f(0.3f, 0.3f, 0.3f);

	//Draw Head
	glBegin(GL_POLYGON);
	for (k = 0; k < segmentCount; k++) {
		hexTheta = TWO_PI * k / segmentCount;
		hexVertex.x = -150 + 25 * cos(hexTheta);
		hexVertex.y = 150 + 25 * sin(hexTheta);
		glVertex2f(hexVertex.x, hexVertex.y);
	}
	glEnd();

	//Draw Body
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	hexVertex.x = hexVertex.x - 25;
	hexVertex.y = hexVertex.y - 25;
	glVertex2f(hexVertex.x, hexVertex.y); //turso start
	glVertex2f(hexVertex.x, hexVertex.y - 90); //turso end

	glVertex2f(hexVertex.x, hexVertex.y); //l.u.arm start
	scrPt elbowVertex;
	elbowVertex.x = hexVertex.x - 40 * sin(poseAngles.upperLeftArm * TWO_PI / 360);
	elbowVertex.y = hexVertex.y - 40 * cos(poseAngles.upperLeftArm * TWO_PI / 360);
	glVertex2f(elbowVertex.x, elbowVertex.y); //l.u.arm end

	glVertex2f(elbowVertex.x, elbowVertex.y); //l.l.arm start
	glVertex2f(elbowVertex.x - 50 * sin((poseAngles.upperLeftArm + poseAngles.lowerLeftArm - 180) * TWO_PI / 360), 
		elbowVertex.y - 50 * cos((poseAngles.upperLeftArm + poseAngles.lowerLeftArm - 180) * TWO_PI / 360)); //l.l.arm end

	glVertex2f(hexVertex.x, hexVertex.y); //r.u.arm start
	elbowVertex.x = hexVertex.x + 40 * sin(poseAngles.upperRightArm * TWO_PI / 360);
	elbowVertex.y = hexVertex.y - 40 * cos(poseAngles.upperRightArm * TWO_PI / 360);
	glVertex2f(elbowVertex.x, elbowVertex.y); //r.u.arm end

	glVertex2f(elbowVertex.x, elbowVertex.y); //r.l.arm start
	glVertex2f(elbowVertex.x + 50 * sin((poseAngles.upperRightArm - poseAngles.lowerRightArm - 180) * TWO_PI / 360),
		elbowVertex.y - 50 * cos((poseAngles.upperRightArm - poseAngles.lowerRightArm - 180) * TWO_PI / 360)); //r.l.arm end


	scrPt waistVert;
	waistVert.x = hexVertex.x;
	waistVert.y = hexVertex.y - 90;
	glVertex2f(waistVert.x, waistVert.y); //l.u.leg start
	elbowVertex.x = waistVert.x - 50 * sin(poseAngles.upperLeftLeg * TWO_PI / 360);
	elbowVertex.y = waistVert.y - 50 * cos(poseAngles.upperLeftLeg * TWO_PI / 360);
	glVertex2f(elbowVertex.x, elbowVertex.y); //l.u.leg end

	glVertex2f(elbowVertex.x, elbowVertex.y); //l.l.leg start
	glVertex2f(elbowVertex.x - 60 * sin((poseAngles.upperLeftLeg - poseAngles.lowerLeftLeg - 180) * TWO_PI / 360),
		elbowVertex.y - 60 * cos((poseAngles.upperLeftLeg - poseAngles.lowerLeftLeg - 180) * TWO_PI / 360)); //l.l.leg end

	glVertex2f(waistVert.x, waistVert.y); //r.u.leg start
	elbowVertex.x = waistVert.x + 50 * sin(poseAngles.upperRightLeg * TWO_PI / 360);
	elbowVertex.y = waistVert.y - 50 * cos(poseAngles.upperRightLeg * TWO_PI / 360);
	glVertex2f(elbowVertex.x, elbowVertex.y); //r.u.leg end

	glVertex2f(elbowVertex.x, elbowVertex.y); //r.l.leg start
	glVertex2f(elbowVertex.x + 60 * sin((poseAngles.upperRightLeg + poseAngles.lowerRightLeg - 180) * TWO_PI / 360),
		elbowVertex.y - 60 * cos((poseAngles.upperRightLeg + poseAngles.lowerRightLeg - 180) * TWO_PI / 360)); //r.l.leg end
	glEnd();

	glEndList();
}

void displayStickMan(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	strikePose(lerpPose(frameTimeElapsed));
	glTranslatef(translation, 0.0, 0.0);
	glCallList(regHex);
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}
void updatePose(float dt)
{
	frameTimeElapsed += dt;
	if (frameTimeElapsed > 1.0f) {
		currentPose++;
		currentPose %= 4;
		frameTimeElapsed = 0;
		cout << currentPose << endl;
	}

	translation += dt * 10.0f;
	if (translation > winWidth / 2) {
		translation = 0;
	}
}
static PoseAngles lerpPose(float tween) {
	PoseAngles inbetweenPose;
	PoseAngles lastPose = keyFrames[currentPose];
	PoseAngles nextPose = keyFrames[(currentPose + 1) % 4];
	inbetweenPose.upperLeftArm = lerp(lastPose.upperLeftArm, nextPose.upperLeftArm, tween);
	inbetweenPose.lowerLeftArm = lerp(lastPose.lowerLeftArm, nextPose.lowerLeftArm, tween);
	inbetweenPose.upperRightArm = lerp(lastPose.upperRightArm, nextPose.upperRightArm, tween);
	inbetweenPose.lowerRightArm = lerp(lastPose.lowerRightArm, nextPose.lowerRightArm, tween);
	inbetweenPose.upperLeftLeg = lerp(lastPose.upperLeftArm, nextPose.upperLeftArm, tween);
	inbetweenPose.lowerLeftLeg = lerp(lastPose.lowerLeftLeg, nextPose.lowerLeftLeg, tween);
	inbetweenPose.upperRightLeg = lerp(lastPose.upperRightLeg, nextPose.upperRightLeg, tween);
	inbetweenPose.lowerRightLeg = lerp(lastPose.lowerRightLeg, nextPose.lowerRightLeg, tween);

	return inbetweenPose;
}

static float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-(GLfloat)newWidth/2, (GLfloat)newWidth / 2, -(GLfloat)newHeight/2, (GLfloat)newHeight/2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

void idle(void) {
	int t;
	/* Delta time in seconds. */
	float dt;
	t = glutGet(GLUT_ELAPSED_TIME);
	dt = ((float)t - (float)old_t) / 1000.0f;
	old_t = t;

	updatePose(dt);

	glutPostRedisplay();
}

void mouseFcn(GLint button, GLint action, GLint x, GLint y)
{
	switch (button) {
	case GLUT_MIDDLE_BUTTON: // Start the rotation.
		if (action == GLUT_DOWN)
			glutIdleFunc(idle);
		break;
	case GLUT_RIGHT_BUTTON: // Stop the rotation.
		if (action == GLUT_DOWN)
			glutIdleFunc(NULL);
		break;
	default:
		break;
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Animation Example");
	init();
	glutDisplayFunc(displayStickMan);
	glutReshapeFunc(winReshapeFcn);
	glutMouseFunc(mouseFcn);
	glutMainLoop();
}