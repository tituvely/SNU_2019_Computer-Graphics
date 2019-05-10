////
////  main.cpp
////  infografi
////
////  Created by YooKyung Kim on 15/03/2019.
////  Copyright © 2019 YooKyung Kim. All rights reserved.
////
//
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <cmath>
#include <math.h>
#include <algorithm>

GLdouble rotMatrix[16] =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

GLdouble updatedMatrix[16]{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

/* set global variables */
int width, height;

bool leftButton, rightButton = false;
GLfloat mousePosX, mousePosY;
glm::vec3 currPos, prevPos;

///* vectors that makes the rotation and translation of the cube */
float eye[3] = { 0.0f, 0.0f, 50.0f };
float ori[3] = { 0.0f, 0.0f, 0.0f };
float rot[3] = { 0.0f, 0.0f, 0.0f };

GLuint judy;
float world_rotate_angle;
float arm_rotate_angle;
float balloon_rotate_angle;

float fov = 40.0f;

double camera_angle_h = 0;
double camera_angle_v = 0;

void loadGlobalCoord()
{
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], ori[0], ori[1], ori[2], 0, 1, 0);
    
    glMultMatrixd(rotMatrix);
}

void matrixCopy(GLdouble* dest, GLdouble* src) {
    dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=src[3];
    dest[4]=src[4]; dest[5]=src[5]; dest[6]=src[6]; dest[7]=src[7];
    dest[8]=src[8]; dest[9]=src[9]; dest[10]=src[10]; dest[11]=src[11];
    dest[12]=src[12]; dest[13]=src[13]; dest[14]=src[14]; dest[15]=src[15];
}

void matrixMultiplication(GLdouble* dest, GLdouble* left, GLdouble* right) {
    dest[0] = left[0]*right[0] + left[1]*right[4] + left[2] *right[8]  + left[3] *right[12];
    dest[1] = left[0]*right[1] + left[1]*right[5] + left[2] *right[9]  + left[3] *right[13];
    dest[2] = left[0]*right[2] + left[1]*right[6] + left[2] *right[10] + left[3] *right[14];
    dest[3] = 0;
    
    dest[4] = left[4]*right[0] + left[5]*right[4] + left[6] *right[8]  + left[7] *right[12];
    dest[5] = left[4]*right[1] + left[5]*right[5] + left[6] *right[9]  + left[7] *right[13];
    dest[6] = left[4]*right[2] + left[5]*right[6] + left[6] *right[10] + left[7] *right[14];
    dest[7] = 0;
    
    dest[8] = left[8]*right[0] + left[9]*right[4] + left[10]*right[8]  + left[11]*right[12];
    dest[9] = left[8]*right[1] + left[9]*right[5] + left[10]*right[9]  + left[11]*right[13];
    dest[10]= left[8]*right[2] + left[9]*right[6] + left[10]*right[10] + left[11]*right[14];
    dest[11]= 0;
    
    dest[12]= 0;
    dest[13]= 0;
    dest[14]= 0;
    dest[15]= 1;
}

// Convert to Quaternion to Matrix
void QuaternionToMatrix(GLdouble *Matrix, float w, float x, float y, float z)
{
    if(!Matrix) return;
    // First row
    Matrix[0] = 1.0f - 2.0f * ( y * y + z * z );
    Matrix[1] = 2.0f * ( x * y - w * z );
    Matrix[2] = 2.0f * ( x * z + w * y );
    Matrix[3] = 0.0f;
    
    // Second row
    Matrix[4] = 2.0f * ( x * y + w * z );
    Matrix[5] = 1.0f - 2.0f * ( x * x + z * z );
    Matrix[6] = 2.0f * ( y * z - w * x );
    Matrix[7] = 0.0f;
    
    // Third row
    Matrix[8] = 2.0f * ( x * z - w * y );
    Matrix[9] = 2.0f * ( y * z + w * x );
    Matrix[10] = 1.0f - 2.0f * ( x * x + y * y );
    Matrix[11] = 0.0f;
    
    // Fourth row
    Matrix[12] = 0;
    Matrix[13] = 0;
    Matrix[14] = 0;
    Matrix[15] = 1.0f;
}

glm::vec3 toScreenCoord( double x, double y ) {
    glm::vec3 coord(0.0f);
    
    if( true )
        coord.x =  (2 * x - width ) / width;
    
    if( true )
        coord.y = -(2 * y - height) / height;
    
    /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
    coord.x = glm::clamp( coord.x, -1.0f, 1.0f );
    coord.y = glm::clamp( coord.y, -1.0f, 1.0f );
    
    float length_squared = coord.x * coord.x + coord.y * coord.y;
    if( length_squared <= 1.0 )
        coord.z = sqrt( 1.0 - length_squared );
    else
        coord = glm::normalize( coord );
    
    printf("%f, %f, %f \n", coord.x, coord.y, coord.z);
    return coord;
}

void glutMotion(int x, int y)
{
    if(leftButton) {
//        //rotate
//        camera_angle_v += (y - mousePosY)*0.1;
//        camera_angle_h += (x - mousePosX)*0.1;
        
        currPos = toScreenCoord( x, y );
        
        /* Calculate the angle in radians, and clamp it between 0 and 90 degrees */
//        float rotAngle = acos( std::min(1.0f, glm::dot(prevPos, currPos) ));
        
        float cos2a = (prevPos[0] * currPos[0]) + (prevPos[1] * currPos[1]) + (prevPos[2] * currPos[2]);
        GLfloat sina = sqrt((1.0 - cos2a)*0.5);
        GLfloat cosa = sqrt((1.0 + cos2a)*0.5);
        
        /* Cross product to get the rotation axis, but it's still in camera coordinate */
        glm::vec3 rotAxis = glm::cross( prevPos, currPos );
        
        // 쿼터니언 계산
        float w = cosa;
        float xi = rotAxis.x * sina;
        float yi = rotAxis.y * sina;
        float zi = rotAxis.z * sina;
        
        GLdouble _updatedMatrix[16]{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        
        //rotationMatrix를 구함
        QuaternionToMatrix(_updatedMatrix, w, xi, yi, zi);
        matrixMultiplication(rotMatrix, updatedMatrix, _updatedMatrix);
        glMultMatrixd(rotMatrix);
        
        mousePosX = x;
        mousePosY = y;
    } else if ( rightButton ) {     //translate
        float dx = x - mousePosX;
        float dy = y - mousePosY;
        // 마우스 버튼을 누른 상태에서 마우스를 이동하였을때 X축과 Y축으로 얼마만큼 이동되었는지를 계산

        mousePosX = x;
        mousePosY = y;

        ori[0] -= dx*0.04;
        ori[1] += dy*0.04;

        loadGlobalCoord();
    }
}

void glutMouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON) {            //rotate
        if(state == GLUT_DOWN) {
            leftButton = true;
            mousePosX = x;
            mousePosY = y;      // 마우스 버튼을 누른 좌표 지점 저장
            
            matrixCopy(updatedMatrix, rotMatrix);
            prevPos = toScreenCoord( x, y );
        } else if ( state == GLUT_UP ) {
            leftButton = false;
        }
    } else if(button == GLUT_RIGHT_BUTTON) {    //translate
        if ( state == GLUT_DOWN ) {
            rightButton = true;
            mousePosX = x;
            mousePosY = y;
        } else if ( state == GLUT_UP ) {
            rightButton = false;
        }
    }
}

void drawAxis() {
    // draw axes
    glBegin(GL_LINES);

    // draw line for x axis - red
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-1000.0, 0.0, 0.0);
    glVertex3f(1000.0, 0.0, 0.0);
    glEnd();

    // draw line for y axis - green
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, -1000.0, 0.0);
    glVertex3f(0.0, 1000.0, 0.0);
    glEnd();

    // draw line for Z axis - blue
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
    
//    glRotated(camera_angle_v, 1.0, 0.0, 0.0);
//    glRotated(camera_angle_h, 0.0, 1.0, 0.0);
    
    drawAxis();

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

//    rotateWorld();
//    rotateArm();
//    rotateBalloon();
    
    glutSwapBuffers();
}

void resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)w / (GLfloat)h, .1f, 1000.0f);
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

void changeFov() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)width / (GLfloat)height, .1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void directionkey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:    // dolly in
            eye[2] -= 5.0f;
            break;
        case GLUT_KEY_RIGHT:   // dolly out
            eye[2] += 5.0f;
            break;
        case GLUT_KEY_UP:   // zoom in
            fov -= 5.0f;      // fov가 작아질수록 물체가 커짐
            changeFov();
            break;
        case GLUT_KEY_DOWN: // zoom out
            fov += 5.0f;      // fov가 커질수록 물체가 작아짐
            changeFov();
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
    glutSpecialFunc(directionkey);
    glutMouseFunc(glutMouse);   //마우스 콜백
    glutMotionFunc(glutMotion); //모션 콜백

    judy = glGenLists(5);

    loadObj("/Users/kimyookyung/Documents/OpenGL/infografi/model/obj/judy face.obj", 0);
    loadObj("/Users/kimyookyung/Documents/OpenGL/infografi/model/obj/judy left arm.obj", 1);
    loadObj("/Users/kimyookyung/Documents/OpenGL/infografi/model/obj/judy right arm.obj", 2);
    loadObj("/Users/kimyookyung/Documents/OpenGL/infografi/model/obj/judy left leg.obj", 3);
    loadObj("/Users/kimyookyung/Documents/OpenGL/infografi/model/obj/judy right leg.obj", 4);

//    loadObj("./judy face.obj", 0);
//    loadObj("./judy left arm.obj", 1);
//    loadObj("./judy right arm.obj", 2);
//    loadObj("./judy left leg.obj", 3);
//    loadObj("./judy right leg.obj", 4);

    glutMainLoop(); //OpenGL의 메인함수는 항상 이 함수로 끝난다. 이 함수의 역할은 이벤트 루프를 돌리는 것이다. 이벤트별로 콜백 함수 등록을 모두 마쳤으니 이벤트 루프로 진입하라는 의미이다. 모든 지엘 프로그램은 항상 glutMainLoop() 함수로 끝난다. OpenGL에서는 GLUT 라이브러리를 통해서 윈도우 및 입출력을 제어할 수 있다. GLUT를 사용하여 프로그래머는 필요한 콜백 함수를 등록하고, 해당 콜백 함수에 원하는 내용을 구현하면 된다. 이벤트가 발생하면 해당 콜백 함수를 호출하는 것은 GLUT가 알아서 처리한다. 프로그래머가 등록한 콜백 함수는 콜백 테이블(Callback Table)에 저장되고, GLUT는 이벤트가 발생하면 이 테이블을 참조하여 콜백 함수를 호출한다. 이벤트는 큐(Queue)를 통해 관리된다. GLUT는 이벤트 루프를 돌면서 큐에 이벤트가 있으면 가장 먼저 발생한 이벤트를 가져와서 테이블을 참조하여 함수를 호출한다.

    return 0;
}

