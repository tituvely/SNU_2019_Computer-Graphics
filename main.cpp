////
////  main.cpp
////  infografi
////
////  Created by YooKyung Kim on 15/03/2019.
////  Copyright © 2019 YooKyung Kim. All rights reserved.
////

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

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

float fov = 40.0f;

string curveType;
int numOfCrossSections;
int numOfControlPoints;

vector<vector<vector<float>>> coord;
vector<vector<vector<float>>> transformations;

vector<vector<float>> scalevec;
vector<vector<vector<float>>> translatevec;
vector<vector<glm::quat>> rotatevec;

vector<vector<vector<vector<float>>>> rotatedcoord;

/* vectors that makes the rotation and translation of the cube */
float eye[3] = { 0.0f, 0.0f, 100.0f };
float ori[3] = { 0.0f, 0.0f, 0.0f };
float rot[3] = { 0.0f, 0.0f, 0.0f };

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
    
    return coord;
}

//------------------------------------------------------------------------
// Moves the screen based on mouse pressed button
//------------------------------------------------------------------------

void glutMotion(int x, int y)
{
    if(leftButton) {        //rotate
        currPos = toScreenCoord(x,y);
        
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
        
        mousePosX = x;
        mousePosY = y;
        
        loadGlobalCoord();
    }
    else if ( rightButton ) {     //translate
        float dx = x - mousePosX;
        float dy = y - mousePosY;
        // 마우스 버튼을 누른 상태에서 마우스를 이동하였을때 X축과 Y축으로 얼마만큼 이동되었는지를 계산
        
        mousePosX = x;
        mousePosY = y;
        
        ori[0] -= dx*0.04;
        ori[1] += dy*0.04;
        
        loadGlobalCoord();
    }
    return;
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
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

void DrawControlPoints() {
    glColor3f(1.0f,0,0);
    glPointSize(2);
    glBegin(GL_POINTS);
    for(int i = 0 ; i < numOfCrossSections; i++) {
        for (int j = 0 ; j < numOfControlPoints; j++) {
            glVertex3f(coord[i][j][0],0,coord[i][j][1]);
        }
    }
    glEnd();
}

float CatmullRoll(float t, float p0, float p1, float p2, float p3)
{
    float v;
    float t2 = t*t;
    float t3 = t*t*t;
    
    /* Catmull Rom spline Calculation */
    v = ((2 * p1) + (-p0 + p2) * t + (2*p0 - 5*p1 + 4*p2 - p3) * t2 + (-p0 + 3*p1 - 3*p2 + p3) * t3) * 0.5f;
    
    return v;
}

float BSpline(float t, float p0, float p1, float p2, float p3)
{
    float v; // Interpolated point
    float t2 = t*t;
    float t3 = t*t*t;
    
    /* B Rom spline Calculation */
    v = ((-p0 + 3*p1 - 3*p2 + p3) + (3*p0 - 6*p1 + 3*p2) * t + (-3*p0 + 3*p2) * t2 + (p0 + 4*p1 + p2) * t3) / 6;
    
    return v;
}

void DrawCatmullRollSplines() {
    glLineWidth(1);
    glColor3f(0.933333,0.866667,0.509804);  //Light Goldenrod
    
    for(int i = 0; i < numOfCrossSections; i++) {
        float x, y;    //Interpolated point
        glPushMatrix();
        glRotatef(transformations[i][1][0], transformations[i][1][1], transformations[i][1][2], transformations[i][1][3]);
        glTranslatef(transformations[i][2][0], transformations[i][2][1], transformations[i][2][2]);
        glScalef(transformations[i][0][0], transformations[i][0][0], transformations[i][0][0]);
        
        for (int j = -1 ; j < numOfControlPoints - 2; j++) {
            for(float t=0;t<1;t+=0.2) {
                if(j == -1) {
                    x = CatmullRoll(t,coord[i][numOfControlPoints - 1][0],coord[i][0][0],coord[i][1][0],coord[i][2][0]);
                    y = CatmullRoll(t,coord[i][numOfControlPoints - 1][1],coord[i][0][1],coord[i][1][1],coord[i][2][1]);
                } else if (j == numOfControlPoints - 3)  {
                    x = CatmullRoll(t,coord[i][numOfControlPoints - 3][0],coord[i][numOfControlPoints - 2][0],coord[i][numOfControlPoints - 1][0],coord[i][0][0]);
                    y = CatmullRoll(t,coord[i][numOfControlPoints - 3][1],coord[i][numOfControlPoints - 2][1],coord[i][numOfControlPoints - 1][1],coord[i][0][1]);
                } else {
                    x = CatmullRoll(t,coord[i][j][0],coord[i][j+1][0],coord[i][j+2][0],coord[i][j+3][0]);
                    y = CatmullRoll(t,coord[i][j][1],coord[i][j+1][1],coord[i][j+2][1],coord[i][j+3][1]);
                }
                glBegin(GL_POINTS);
                glVertex3f(x,0,y);
                glEnd();
            }
        }
        glPopMatrix();
    }
}

void DrawBSplines() {
    glLineWidth(1);
    glColor3f(0.941176,0.501961,0.501961);  //Light Coral
    
    for(int i = 0 ; i < numOfCrossSections; i++) {
        float x, y;    //Interpolated point
        glPushMatrix();
        glRotatef(transformations[i][1][0], transformations[i][1][1], transformations[i][1][2], transformations[i][1][3]);
        glTranslatef(transformations[i][2][0], transformations[i][2][1], transformations[i][2][2]);
        glScalef(transformations[i][0][0], transformations[i][0][0], transformations[i][0][0]);
        
        for (int j = -1 ; j < numOfControlPoints - 2; j++) {
            for(float t=0;t<1;t+=0.2) {
                if(j == -1) {
                    x = BSpline(t,coord[i][numOfControlPoints - 1][0],coord[i][0][0],coord[i][1][0],coord[i][2][0]);
                    y = BSpline(t,coord[i][numOfControlPoints - 1][1],coord[i][0][1],coord[i][1][1],coord[i][2][1]);
                } else if (j == numOfControlPoints - 3)  {
                    x = BSpline(t,coord[i][numOfControlPoints - 3][0],coord[i][numOfControlPoints - 2][0],coord[i][numOfControlPoints - 1][0],coord[i][0][0]);
                    y = BSpline(t,coord[i][numOfControlPoints - 3][1],coord[i][numOfControlPoints - 2][1],coord[i][numOfControlPoints - 1][1],coord[i][0][1]);
                } else {
                    x = BSpline(t,coord[i][j][0],coord[i][j+1][0],coord[i][j+2][0],coord[i][j+3][0]);
                    y = BSpline(t,coord[i][j][1],coord[i][j+1][1],coord[i][j+2][1],coord[i][j+3][1]);
                }
                glBegin(GL_POINTS);
                glVertex3f(x,0,y);
                glEnd();
            }
        }
        glPopMatrix();
    }
}

void MakeScaleSpline() {
    for(int i = -1; i < numOfCrossSections - 2; i++) {
        scalevec.push_back(vector<float>());
        float s;    //Interpolated point
        for(float t=0;t<1;t+=0.2) {
            if(i == -1) {
                s = CatmullRoll(t,transformations[numOfCrossSections - 1][0][0],transformations[0][0][0],transformations[1][0][0],transformations[2][0][0]);
            } else if (i == numOfCrossSections - 3)  {
                s = CatmullRoll(t,transformations[numOfCrossSections - 3][0][0],transformations[numOfCrossSections - 2][0][0],transformations[numOfCrossSections - 1][0][0],transformations[0][0][0]);
            } else {
                s = CatmullRoll(t,transformations[i][0][0],transformations[i+1][0][0],transformations[i+2][0][0],transformations[i+3][0][0]);
            }
            scalevec.at(i+1).push_back(s);
        }
    }
}

void MakeTranslateSpline() {
    for(int i = - 1; i < numOfCrossSections - 2; i++) {
        translatevec.push_back(vector<vector<float>>());
        float x,y,z;    //Interpolated point
        int index=0;
        for(float t=0;t<1;t+=0.2) {
            translatevec.at(i+1).push_back(vector<float>());
            if(i == -1) {
                x = CatmullRoll(t,transformations[numOfCrossSections - 1][2][0],transformations[0][2][0],transformations[1][2][0],transformations[2][2][0]);
                y = CatmullRoll(t,transformations[numOfCrossSections - 1][2][1],transformations[0][2][1],transformations[1][2][1],transformations[2][2][1]);
                z = CatmullRoll(t,transformations[numOfCrossSections - 1][2][2],transformations[0][2][2],transformations[1][2][2],transformations[2][2][2]);
            } else if (i == numOfCrossSections - 3)  {
                x = CatmullRoll(t,transformations[numOfCrossSections - 3][2][0],transformations[numOfCrossSections - 2][2][0],transformations[numOfCrossSections - 1][2][0],transformations[0][2][0]);
                y = CatmullRoll(t,transformations[numOfCrossSections - 3][2][1],transformations[numOfCrossSections - 2][2][1],transformations[numOfCrossSections - 1][2][1],transformations[0][2][1]);
                z = CatmullRoll(t,transformations[numOfCrossSections - 3][2][2],transformations[numOfCrossSections - 2][2][2],transformations[numOfCrossSections - 1][2][2],transformations[0][2][2]);
            } else {
                x = CatmullRoll(t,transformations[i][2][0],transformations[i+1][2][0],transformations[i+2][2][0],transformations[i+3][2][0]);
                y = CatmullRoll(t,transformations[i][2][1],transformations[i+1][2][1],transformations[i+2][2][1],transformations[i+3][2][1]);
                z = CatmullRoll(t,transformations[i][2][2],transformations[i+1][2][2],transformations[i+2][2][2],transformations[i+3][2][2]);
            }
            translatevec.at(i+1).at(index).push_back(x);
            translatevec.at(i+1).at(index).push_back(y);
            translatevec.at(i+1).at(index).push_back(z);
            index++;
        }
    }
}

void MakeRotateSpline() {
    for(int i = 0; i < numOfCrossSections - 1; i++) {
        rotatevec.push_back(vector<glm::quat>());
        float sina, x, y, z, w;
        
        sina = sin(transformations[i][1][0]/2);
        x = transformations[i][1][1] * sina;
        y = transformations[i][1][2] * sina;
        z = transformations[i][1][3] * sina;
        w = cos(transformations[i][1][0]/2);
        glm::quat quat1= glm::quat(w,x,y,z);
        
        sina = sin(transformations[i+1][1][0]/2);
        x = transformations[i+1][1][1] * sina;
        y = transformations[i+1][1][2] * sina;
        z = transformations[i+1][1][3] * sina;
        w = cos(transformations[i+1][1][0]/2);
        glm::quat quat2= glm::quat(w,x,y,z);
        
        for(float t=0;t<1;t+=0.1) {
            // 쿼터니언 계산
            glm::quat interpolatedquat = mix(quat1, quat2, t);
            rotatevec.at(i).push_back(interpolatedquat);
        }
    }
}

void MakeVertex() {
    glLineWidth(1);
    glColor3f(0.1,0.5,0.601961);
    
    glPushMatrix();
    for(int i = 0 ; i < numOfCrossSections; i++) {
        rotatedcoord.push_back(vector<vector<vector<float>>>());
        float x, z;    //Interpolated point
        for (int j = 0 ; j < numOfControlPoints; j++) {
            rotatedcoord.at(i).push_back(vector<vector<float>>());
            int index=0;
            x = coord[i][j][0];
            z = coord[i][j][1];
            
            if(i == numOfCrossSections - 1) {
                rotatedcoord.at(i).at(j).push_back(vector<float>());
                glPushMatrix();
                glRotatef(transformations[i][1][0], transformations[i][1][1], transformations[i][1][2], transformations[i][1][3]);
                glTranslatef(transformations[i][2][0], transformations[i][2][1], transformations[i][2][2]);
                glScalef(transformations[i][0][0], transformations[i][0][0], transformations[i][0][0]);
                
                GLdouble m[16];
                glGetDoublev(GL_MODELVIEW_MATRIX, m);
                
                float xx, yy, zz;
                xx = x*m[0] + 0*m[4] + z*m[8] + m[12];
                yy = x*m[1] + 0*m[5] + z*m[9] + m[13];
                zz = x*m[2] + 0*m[6] + z*m[10] + m[14] + 100.0;
                
                rotatedcoord.at(i).at(j).at(0).push_back(xx);
                rotatedcoord.at(i).at(j).at(0).push_back(yy);
                rotatedcoord.at(i).at(j).at(0).push_back(zz);
                glPopMatrix();
            } else {
                for(float t=0;t<1;t+=0.2) {
                    rotatedcoord.at(i).at(j).push_back(vector<float>());
                    glPushMatrix();
                    
                    //rotationMatrix를 구함
                    GLdouble _updatedMatrix[16]{
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1
                    };
                    glm::quat q = rotatevec.at(i).at(index);
                    QuaternionToMatrix(_updatedMatrix, q.w, q.x, q.y, q.z);
                    
                    float angle, rx, ry, rz;
                    float scale = sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
                    if (scale < 0.001) {  // test to avoid divide by zero, s is always positive due to sqrt
                        rx = 1;
                        ry = 0;
                        rz = 0;
                    } else {
                        rx = q.x / scale; // normalise axis
                        ry = q.y / scale;
                        rz = q.z / scale;
                    }
                    
                    angle = acos(q.w) * 2.0f;
                    
                    glRotatef(angle, rx, ry, rz);
                    glTranslatef(translatevec.at(i).at(index).at(0), translatevec.at(i).at(index).at(1), translatevec.at(i).at(index).at(2));
                    glScalef(scalevec.at(i).at(index), scalevec.at(i).at(index), scalevec.at(i).at(index));
                    
                    GLdouble m[16];
                    glGetDoublev(GL_MODELVIEW_MATRIX, m);
                    float xx, yy, zz;
                    xx = x*m[0] + 0*m[4] + z*m[8] + m[12];
                    yy = x*m[1] + 0*m[5] + z*m[9] + m[13];
                    zz = x*m[2] + 0*m[6] + z*m[10] + m[14] + 100.0;
                    
                    rotatedcoord.at(i).at(j).at(index).push_back(xx);
                    rotatedcoord.at(i).at(j).at(index).push_back(yy);
                    rotatedcoord.at(i).at(j).at(index).push_back(zz);
                    
                    glPopMatrix();
                    index++;
                }
            }
        }
    }
    
    glPopMatrix();
}

void Mesh() {
    //  Vertical
    for(int i = 0 ; i < numOfCrossSections - 1; i++) {
        float x,y,z;
        for (int j = 0 ; j < numOfControlPoints; j++) {
            glBegin(GL_LINE_STRIP);
            int index=0;
            for(float t=0;t<1;t+=0.2) {
                x = rotatedcoord.at(i).at(j).at(index).at(0);
                y = rotatedcoord.at(i).at(j).at(index).at(1);
                z = rotatedcoord.at(i).at(j).at(index).at(2);
                
                glVertex3f(x,y,z);
                index++;
            }
            x = rotatedcoord.at(i+1).at(j).at(0).at(0);
            y = rotatedcoord.at(i+1).at(j).at(0).at(1);
            z = rotatedcoord.at(i+1).at(j).at(0).at(2);
            glVertex3f(x,y,z);
            glEnd();
        }
    }
    
    // Horizontal
    for(int i = 0 ; i < numOfCrossSections; i++) {
        float x,y,z;
        if(i < numOfCrossSections - 1) {
            int index=0;
            for(float t=0;t<1;t+=0.2) {
                glBegin(GL_LINE_STRIP);
                for (int j = 0 ; j < numOfControlPoints; j++) {
                    x = rotatedcoord.at(i).at(j).at(index).at(0);
                    y = rotatedcoord.at(i).at(j).at(index).at(1);
                    z = rotatedcoord.at(i).at(j).at(index).at(2);
                    glVertex3f(x,y,z);
                }
                index++;
                glEnd();
            }
        } else {
            glBegin(GL_LINE_STRIP);
            for (int j = 0 ; j < numOfControlPoints; j++) {
                x = rotatedcoord.at(i).at(j).at(0).at(0);
                y = rotatedcoord.at(i).at(j).at(0).at(1);
                z = rotatedcoord.at(i).at(j).at(0).at(2);
                glVertex3f(x,y,z);
            }
            glEnd();
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    loadGlobalCoord();
    
    drawAxis();
    
    //    DrawControlPoints();
    
    if (curveType == "BSPLINE") {
        DrawBSplines();
    } else if (curveType == "CATMULL_ROM") {
        DrawCatmullRollSplines();
    }
    
    MakeScaleSpline();
    MakeRotateSpline();
    MakeTranslateSpline();
    
    MakeVertex();
    
    Mesh();
    
    glutSwapBuffers();
}

void resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, .1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
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
    glutTimerFunc(timeStep, Timer, 0);
}

void loadFile(char *fname) {
    string line;
    ifstream myfile (fname);
    if (myfile.is_open())
    {
        myfile >> curveType >> numOfCrossSections >> numOfControlPoints;
        
        for(int i = 0 ; i < numOfCrossSections; i++) {
            for (int j = 0 ; j < numOfControlPoints; j++) {
                float x, y;
                myfile >> x >> y; // 2D position of control points
                coord.push_back(vector<vector<float>>());
                coord.at(i).push_back(vector<float>());
                coord.at(i).at(j).push_back(x); // coord[i][j][0]
                coord.at(i).at(j).push_back(y); // coord[i][j][1]
            }
            
            float scaling, rotationAngle, rotationX, rotationY, rotationZ, translationX, translationY, translationZ;
            
            myfile >> scaling; // Scaling factor
            transformations.push_back(vector<vector<float>>());
            transformations.at(i).push_back(vector<float>());
            transformations.at(i).at(0).push_back(scaling);   // transformations[i][0][0];
            
            myfile >> rotationAngle >> rotationX >> rotationY >> rotationZ;
            transformations.at(i).push_back(vector<float>());
            transformations.at(i).at(1).push_back(rotationAngle);   // transformations[i][1][0];
            transformations.at(i).at(1).push_back(rotationX);   // transformations[i][1][1];
            transformations.at(i).at(1).push_back(rotationY);   // transformations[i][1][2];
            transformations.at(i).at(1).push_back(rotationZ);   // transformations[i][1][3];
            
            myfile >> translationX >> translationY >> translationZ;
            transformations.at(i).push_back(vector<float>());
            transformations.at(i).at(2).push_back(translationX);   // transformations[i][2][0];
            transformations.at(i).at(2).push_back(translationY);   // transformations[i][2][1];
            transformations.at(i).at(2).push_back(translationZ);   // transformations[i][2][2];
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition( 50, 0 );
    glutCreateWindow("Example");
    
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutTimerFunc(timeStep, Timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(directionkey);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    
    loadFile(argv[1]);
    
    glutMainLoop();
    
    return 0;
}
