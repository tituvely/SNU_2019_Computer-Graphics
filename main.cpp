//
//  main.cpp
//  infografi
//
//  Created by YooKyung Kim on 15/03/2019.
//  Copyright © 2019 YooKyung Kim. All rights reserved.
//
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <algorithm>

GLdouble rotMatrix[16] =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

/* set global variables */
int width, height;

bool leftButton = false;
GLfloat mousePosX, mousePosY;

GLuint judy;
float world_rotate_angle;
float arm_rotate_angle;
float balloon_rotate_angle;

/* vectors that makes the rotation and translation of the cube */
float eye[3] = { 20.0f, 20.0f, 30.0f };
float ori[3] = { 0.0f, 10.0f, 0.0f };
float rot[3] = { 0.0f, 0.0f, 0.0f };

double camera_angle_h = 0;
double camera_angle_v = 0;

int drag_x_origin;
int drag_y_origin;
int dragging = 0;

void loadGlobalCoord()
{
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], ori[0], ori[1], ori[2], 0, 1, 0);
    glMultMatrixd(rotMatrix);
    glRotated(camera_angle_v, 1.0, 0.0, 0.0);
    glRotated(camera_angle_h, 0.0, 1.0, 0.0);
}

//------------------------------------------------------------------------
// Moves the screen based on mouse pressed button
//------------------------------------------------------------------------
void glutMotion(int x, int y)
{
    if(dragging) {
        camera_angle_v += (y - drag_y_origin)*0.1;
        camera_angle_h += (x - drag_x_origin)*0.1;
        drag_x_origin = x;
        drag_y_origin = y;
    }
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
void glutMouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            dragging = 1;
            drag_x_origin = x;
            drag_y_origin = y;
        }
        else {
            dragging = 0;
        }
    }
}

void drawAxis() {
    // draw axes
    glBegin(GL_LINES);
    
    // draw line for x axis
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-1000.0, 0.0, 0.0);
    glVertex3f(1000.0, 0.0, 0.0);
    glEnd();
    
    // draw line for y axis
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, -1000.0, 0.0);
    glVertex3f(0.0, 1000.0, 0.0);
    glEnd();
    
    // draw line for Z axis
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, -1000.0);
    glVertex3f(0.0, 0.0, 1000.0);
    glEnd();
}

void rotateWorld() {
    world_rotate_angle = world_rotate_angle + 0.6;
    if(world_rotate_angle > 360) world_rotate_angle = world_rotate_angle - 360;
}

void rotateArm() {
    arm_rotate_angle = arm_rotate_angle + 0.5;
    if(arm_rotate_angle > 360) arm_rotate_angle = arm_rotate_angle - 360;
}

void rotateBalloon() {
    balloon_rotate_angle = balloon_rotate_angle + 2;
    if(balloon_rotate_angle > 360) balloon_rotate_angle = balloon_rotate_angle - 360;
}

void drawTrunk() {
    glPushMatrix();
    
    glColor3f(0.827451, 0.827451, 0.827451);
    glBegin(GL_QUADS);
    // top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.5, 9, 3);
    glVertex3f(1.5, 9, 3);
    glVertex3f(1.5, 9, 0);
    glVertex3f(-1.5, 9, 0);
    glEnd();
    
    glBegin(GL_QUADS);
    // front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.5, 6, 3);
    glVertex3f(1.5, 9, 3);
    glVertex3f(-1.5, 9, 3);
    glVertex3f(-1.5, 6, 3);
    glEnd();
    
    glBegin(GL_QUADS);
    // right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.5, 9, 0);
    glVertex3f(1.5, 9, 3);
    glVertex3f(1.5, 6, 3);
    glVertex3f(1.5, 6, 0);
    glEnd();
    
    glBegin(GL_QUADS);
    // left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-1.5, 6, 3);
    glVertex3f(-1.5, 9, 3);
    glVertex3f(-1.5, 9, 0);
    glVertex3f(-1.5, 6, 0);
    glEnd();
    
    glBegin(GL_QUADS);
    // bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(1.5, 6, 3);
    glVertex3f(-1.5, 6, 3);
    glVertex3f(-1.5, 6, 0);
    glVertex3f(1.5, 6, 0);
    glEnd();
    
    glBegin(GL_QUADS);
    // back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(1.5, 9, 0);
    glVertex3f(1.5, 6, 0);
    glVertex3f(-1.5, 6, 0);
    glVertex3f(-1.5, 9, 0);
    glEnd();
    
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    loadGlobalCoord();
 
//    drawAxis();
    
    glPushMatrix();
        glRotatef(world_rotate_angle,0,1,0);
    
        // 주디 얼굴
        glPushMatrix();
            glTranslatef(-0.5, 11.6, -0.15);
            glColor3f(0.866667, 0.627451, 0.866667);
            glScalef(25, 25, 25);
            glCallList(judy);
        glPopMatrix();
    
        glColor3f(0.466667, 0.533333, 0.6);
    
        // 주디 왼다리
        glPushMatrix();
            glRotatef(-5, 0, 0, 1);
            glTranslatef(-6, 4, -1);
            glScalef(35, 35, 35);
            glCallList(judy + 3);
        glPopMatrix();
    
        // 주디 오른다리
        glPushMatrix();
            glRotatef(5, 0, 0, 1);
            glTranslatef(-5.5, 4, -1);
            glScalef(35, 35, 35);
            glCallList(judy + 4);
        glPopMatrix();
    
        glPushMatrix();
            // 주디 몸통
            drawTrunk();

            glColor3f(0.745098, 0.745098, 0.745098);
    
            // 주디 왼팔
            glPushMatrix();
                glTranslatef(-0.5, 5.00, -1);
                glTranslatef(-1,3.7, 2.5);
                glRotatef(-arm_rotate_angle,1,0,0);
                glTranslatef(1,-3.7, -2.5);
                glScalef(35, 35, 35);
                glCallList(judy + 1);
            glPopMatrix();
    
            // 주디 오른팔
            glPushMatrix();
                glTranslatef(1.3, 5.00, -1);
                glTranslatef(0.2, 3.7, 2.7);
                glRotatef(arm_rotate_angle,1,0,0);
                glTranslatef(-0.2, -3.7, -2.7);
                glScalef(35, 35, 35);
                glCallList(judy + 2);
                glScalef(0.1, 0.1, 0.1);
    
                // 풍선 선
                glPushMatrix();
                    glTranslatef(0.55, 0.09, 0.8);
                    glRotatef(balloon_rotate_angle,1,0,0);
                    glTranslatef(-0.55, -0.09, -0.8);
                    glBegin(GL_LINES);
                    glColor3f(1, 1, 0.878431);
                    glVertex3f(0.6, 0.1, 0.8);
                    glVertex3f(2, 2.5, 0.8);
                    glEnd();
    
                    // 풍선
                    glPushMatrix();
                        GLUquadric *sphere; // object 선언
                        sphere = gluNewQuadric(); // object 생성
                        glTranslatef(2.1, 2.6, 0.8);
                        glColor3f(0.933333, 0.796078, 0.67843);
                        gluSphere(sphere, 0.4f, 50, 10);  // sphere 그리기
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    rotateWorld();
    rotateArm();
    rotateBalloon();
    
    glutSwapBuffers();
}

void resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (GLfloat)w / (GLfloat)h, .1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:    //esc키
            exit(0);
            break;
        default:
            break;
    }
}

unsigned timeStep = 30;
void Timer(int unused)
{
    /* call the display callback and forces the current window to be displayed */
    glutPostRedisplay();
    glutTimerFunc(timeStep, Timer, 0);  //30밀리초마다 실행
}

void loadObj(char *fname, int index) {
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    
    fp=fopen(fname, "r");
    if(!fp) {
        printf("can't open file %s\n", fname);
        exit(1);
    }
    
    glPointSize(2.0);
    
    glNewList(judy + index, GL_COMPILE);
        glPushMatrix();
            glBegin(GL_POINTS);
            while(!feof(fp)) {
                read = fscanf(fp, "%c %f %f %f", &ch, &x, &y, &z);
                if(read==4&&ch=='v') {
                    glVertex3f(x, y, z);
                }
            }
            glEnd();
        glPopMatrix();
    glEndList();
    
    fclose(fp);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition( 50, 0 );
    glutCreateWindow("Judy with balloon");    //GLUT에게 새로운 윈도우를 생성하라는 명령이다. 사각형이 그려지는 윈도우가 바로 이 함수에 의해 만들어진다. 인자로 전달되는 문자열은 윈도우 상단의 타이틀바(title bar)에 나타난다.
    
    glutReshapeFunc(resize);    //1.처음 윈도우를 열 때, 2.윈도우 위치를 옮길 때, 3.윈도우 크기를 조절할 때 Reshpae Event가 발생한 것으로 취급합니다. 그리고 이 함수의 파라미터로는 변경된 윈도우의 폭과 높이를 받게 됩니다. 만약에 콜백함수를 등록하지 않게 되면 default 콜백이 실행되어 Viewport(0,0,width,height)가 실행되어 왜곡이 생기지 않게 맞춰줍니다.
    glutDisplayFunc(display);   //이 함수는 'display'라는 함수를 디스플레이 이벤트에 대한 콜백 함수로 등록한 것이다. 이 함수의 정의를 찾아보면 다음과 같다. extern void APIENTRY glutDisplayFunc(void (*func)(void)). 매개변수를 함수 포인터로 되어있다. 즉 매개변수로 전달한 함수는 디스플레이 이벤트마다 호출된다. 디스플레이 이벤트는 별일없으면 계속 발생한다.
    glutTimerFunc(timeStep, Timer, 0);  //타이머콜백 - 30밀리초(timestep) 이후 실행
    glutKeyboardFunc(keyboard); //glutKeyboardFunc(void (*func)(unsigned char key, int x, int y)) 함수가 바로 입력 콜백 함수를 등록하는 함수이다.
    glutMouseFunc(glutMouse);   //마우스 콜백
    glutMotionFunc(glutMotion); //모션 콜백
    
    judy = glGenLists(5);
    loadObj("./judy face.obj", 0);
    loadObj("./judy left arm.obj", 1);
    loadObj("./judy right arm.obj", 2);
    loadObj("./judy left leg.obj", 3);
    loadObj("./judy right leg.obj", 4);
    
    glutMainLoop(); //OpenGL의 메인함수는 항상 이 함수로 끝난다. 이 함수의 역할은 이벤트 루프를 돌리는 것이다. 이벤트별로 콜백 함수 등록을 모두 마쳤으니 이벤트 루프로 진입하라는 의미이다. 모든 지엘 프로그램은 항상 glutMainLoop() 함수로 끝난다. OpenGL에서는 GLUT 라이브러리를 통해서 윈도우 및 입출력을 제어할 수 있다. GLUT를 사용하여 프로그래머는 필요한 콜백 함수를 등록하고, 해당 콜백 함수에 원하는 내용을 구현하면 된다. 이벤트가 발생하면 해당 콜백 함수를 호출하는 것은 GLUT가 알아서 처리한다. 프로그래머가 등록한 콜백 함수는 콜백 테이블(Callback Table)에 저장되고, GLUT는 이벤트가 발생하면 이 테이블을 참조하여 콜백 함수를 호출한다. 이벤트는 큐(Queue)를 통해 관리된다. GLUT는 이벤트 루프를 돌면서 큐에 이벤트가 있으면 가장 먼저 발생한 이벤트를 가져와서 테이블을 참조하여 함수를 호출한다.
    
    return 0;
}


