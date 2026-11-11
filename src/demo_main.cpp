// Demo xem san pham cua Nhi, KHONG phai module camera chinh nha.
#include "VolcanoScene.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

static float yaw=40.0f,pitch=28.0f,distance=75.0f;
static bool wire=false,lightOn=true;
static int winW=1100,winH=700;
const float PI=3.14159265358979323846f;
void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    float ya=yaw*PI/180.0f,pa=pitch*PI/180.0f;
    const float ex=distance*std::cos(pa)*std::sin(ya);
    const float ey=9.0f+distance*std::sin(pa);
    const float ez=distance*std::cos(pa)*std::cos(ya);
    gluLookAt(ex,ey,ez,0,7,0,0,1,0);
    GLfloat pos[]={-30.0f,55.0f,25.0f,1.0f};
    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    if(lightOn)glEnable(GL_LIGHTING);else glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,wire?GL_LINE:GL_FILL);
    VolcanoScene::drawSceneGeometry();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glutSwapBuffers();
}
void reshape(int w,int h) {
    winW=w;winH=h?h:1;glViewport(0,0,winW,winH);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    gluPerspective(55.0,(double)winW/winH,0.2,300.0);
    glMatrixMode(GL_MODELVIEW);
}
void key(unsigned char k,int,int) {
    switch(k) {
        case 27: VolcanoScene::releaseTextures();std::exit(0);
        case 'a':case 'A':yaw-=5;break;
        case 'd':case 'D':yaw+=5;break;
        case 'w':case 'W':pitch+=4;if(pitch>82)pitch=82;break;
        case 's':case 'S':pitch-=4;if(pitch<5)pitch=5;break;
        case '+':case '=':distance-=3;if(distance<24)distance=24;break;
        case '-':case '_':distance+=3;if(distance>150)distance=150;break;
        case 'f':case 'F':wire=!wire;break;
        case 'l':case 'L':lightOn=!lightOn;break;
    }
    glutPostRedisplay();
}
int main(int argc,char**argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("Do hoa may tinh - Nui lua va dia hinh");
    glClearColor(0.63f,0.76f,0.85f,1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    GLfloat ambient[]={0.28f,0.28f,0.30f,1.0f};
    GLfloat diffuse[]={0.90f,0.85f,0.80f,1.0f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    VolcanoScene::initTextures();
    glutDisplayFunc(display);glutReshapeFunc(reshape);glutKeyboardFunc(key);
    std::puts("A/D: xoay | W/S: nang/ha | +/-: phong/thu | F: luoi | L: den | ESC: thoat");
    glutMainLoop();
    return 0;
}
