#include "VolcanoScene.h"
#include <GL/gl.h>
#include <cmath>
#include <cstdlib>

namespace {
const float PI = 3.14159265358979323846f;
const float TERRAIN_HALF = 48.0f;
const float CRATER_Y = 16.0f;
const float CRATER_R = 3.4f;
const float LAVA_FLOOR_Y = 12.0f;
GLuint rockTexture = 0;
GLuint groundTexture = 0;

float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
float smooth(float t) { t = clamp01(t); return t * t * (3.0f - 2.0f * t); }
float hash2(int x, int z) {
    unsigned int h = (unsigned int)x * 374761393u + (unsigned int)z * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (float)(h & 65535u) / 65535.0f;
}
float noise(float x, float z) {
    int ix = (int)std::floor(x), iz = (int)std::floor(z);
    float fx = smooth(x - (float)ix), fz = smooth(z - (float)iz);
    float a = hash2(ix, iz) * (1.0f-fx) + hash2(ix+1, iz) * fx;
    float b = hash2(ix, iz+1) * (1.0f-fx) + hash2(ix+1, iz+1) * fx;
    return a * (1.0f-fz) + b * fz;
}
float terrainHeight(float x, float z) {
    const float r = std::sqrt(x*x + z*z);
    // Chan nui bang phang de khop mesh nui lua voi mat dat.
    const float fade = smooth((r - 17.0f) / 9.0f);
    const float hills = 2.8f * (noise(x*0.055f,z*0.055f)-0.48f)
                      + 0.85f * (noise(x*0.16f,z*0.16f)-0.5f);
    return hills * fade;
}
float profile(float r) {
    // Ban kinh tu trong ra ngoai; mieng nui rong, thanh trong doc.
    static const float rr[] = {0.0f, 1.6f, 2.3f, 3.4f, 4.3f, 5.5f, 8.0f, 11.0f, 14.0f, 17.0f, 19.5f};
    static const float yy[] = {12.0f,12.0f,13.0f,16.0f,15.4f,14.2f,11.0f,7.1f,3.9f,1.4f,0.0f};
    for (int i = 1; i < 11; ++i) {
        if (r <= rr[i]) {
            const float t = (r-rr[i-1])/(rr[i]-rr[i-1]);
            return yy[i-1]*(1.0f-t) + yy[i]*t;
        }
    }
    return 0.0f;
}
float mountainHeight(float r, float a) {
    const float y = profile(r);
    // Gam mat nui loi lom; giam bien do o mieng va chan.
    float weight = smooth((r-3.4f)/3.0f) * (1.0f-smooth((r-15.0f)/4.5f));
    const float x = r*std::cos(a), z = r*std::sin(a);
    float irregular = 0.65f*(noise(x*0.27f,z*0.27f)-0.5f)
                    + 0.32f*std::sin(a*9.0f + r*0.63f)
                    + 0.16f*std::cos(a*17.0f-r*1.25f);
    return y + weight * irregular;
}
void vertexMountain(float r, float a) {
    const float x=r*std::cos(a), z=r*std::sin(a);
    glTexCoord2f(a*3.0f/PI, r*0.19f);
    glVertex3f(x, mountainHeight(r,a), z);
}
void makeTexture(GLuint &id, bool rock) {
    const int W=128, H=128;
    unsigned char pixels[W*H*3];
    for (int j=0; j<H; ++j) for (int i=0; i<W; ++i) {
        const float n = 0.6f*noise(i*0.15f,j*0.15f) + 0.3f*noise(i*0.46f,j*0.46f)
                      + 0.1f*hash2(i,j);
        const float grain = (n-0.5f)*85.0f;
        const int k = (j*W+i)*3;
        if (rock) {
            pixels[k]   = (unsigned char)(75 + grain);
            pixels[k+1] = (unsigned char)(69 + grain);
            pixels[k+2] = (unsigned char)(67 + grain);
        } else {
            pixels[k]   = (unsigned char)(92 + grain*0.6f);
            pixels[k+1] = (unsigned char)(94 + grain*0.8f);
            pixels[k+2] = (unsigned char)(70 + grain*0.5f);
        }
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Build mipmap chain manually (khong can GLU trong module).
    int w=W,h=H;
    unsigned char level[W*H*3];
    for(int i=0;i<W*H*3;++i) level[i]=pixels[i];
    int mip=0;
    while(true) {
        glTexImage2D(GL_TEXTURE_2D,mip,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,level);
        if(w==1 && h==1) break;
        const int nw=w>1?w/2:1, nh=h>1?h/2:1;
        unsigned char next[W*H*3];
        for(int y=0;y<nh;++y) for(int x=0;x<nw;++x) for(int c=0;c<3;++c) {
            int sum=0;
            for(int dy=0;dy<2;++dy) for(int dx=0;dx<2;++dx)
                sum += level[((y*2+dy < h ? y*2+dy : h-1)*w + (x*2+dx < w ? x*2+dx : w-1))*3+c];
            next[(y*nw+x)*3+c]=(unsigned char)(sum/4);
        }
        for(int i=0;i<nw*nh*3;++i) level[i]=next[i];
        w=nw;h=nh;++mip;
    }
}

}
namespace VolcanoScene {
void initTextures() {
    if(!rockTexture) makeTexture(rockTexture,true);
    if(!groundTexture) makeTexture(groundTexture,false);
}
void drawTerrain() {
    const int COUNT=100;
    const float step=2.0f*TERRAIN_HALF/COUNT;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,groundTexture);
    glColor3f(1,1,1);
    for(int z=0;z<COUNT;++z) {
        const float z0=-TERRAIN_HALF+z*step;
        const float z1=z0+step;
        glBegin(GL_TRIANGLE_STRIP);
        for(int x=0;x<=COUNT;++x) {
            const float xx=-TERRAIN_HALF+x*step;
            const float h0=terrainHeight(xx,z0),h1=terrainHeight(xx,z1);
            const float eps=0.3f;
            const float dx=(terrainHeight(xx+eps,z0)-terrainHeight(xx-eps,z0))/(2*eps);
            const float dz=(terrainHeight(xx,z0+eps)-terrainHeight(xx,z0-eps))/(2*eps);
            glNormal3f(-dx,1.0f,-dz);
            glTexCoord2f(xx*0.15f,z0*0.15f);glVertex3f(xx,h0,z0);
            const float dx1=(terrainHeight(xx+eps,z1)-terrainHeight(xx-eps,z1))/(2*eps);
            const float dz1=(terrainHeight(xx,z1+eps)-terrainHeight(xx,z1-eps))/(2*eps);
            glNormal3f(-dx1,1.0f,-dz1);
            glTexCoord2f(xx*0.15f,z1*0.15f);glVertex3f(xx,h1,z1);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}
void drawVolcano() {
    const int SEG=128, RINGS=90;
    const float maxR=19.5f;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,rockTexture);
    glColor3f(1,1,1);
    // Dao winding cua dai vong de mat ngoai huong len tren.
    glFrontFace(GL_CW);
    for(int j=0;j<RINGS;++j) {
        const float r0=1.6f+(maxR-1.6f)*j/RINGS;
        const float r1=1.6f+(maxR-1.6f)*(j+1)/RINGS;
        glBegin(GL_QUAD_STRIP);
        for(int i=0;i<=SEG;++i) {
            float a=2.0f*PI*i/SEG;
            const float dr=0.05f,da=0.005f;
            const float hR=(mountainHeight(r0+dr,a)-mountainHeight(r0-dr,a))/(2*dr);
            const float hA=(mountainHeight(r0,a+da)-mountainHeight(r0,a-da))/(2*da);
            // Surface y=f(r,a): normal ~ (-dy/dx,1,-dy/dz).
            const float slopeX=hR*std::cos(a)-hA*std::sin(a)/r0;
            const float slopeZ=hR*std::sin(a)+hA*std::cos(a)/r0;
            glNormal3f(-slopeX,1.0f,-slopeZ);vertexMountain(r0,a);
            const float hR1=(mountainHeight(r1+dr,a)-mountainHeight(r1-dr,a))/(2*dr);
            const float hA1=(mountainHeight(r1,a+da)-mountainHeight(r1,a-da))/(2*da);
            glNormal3f(-(hR1*std::cos(a)-hA1*std::sin(a)/r1),1.0f,
                       -(hR1*std::sin(a)+hA1*std::cos(a)/r1));
            vertexMountain(r1,a);
        }
        glEnd();
    }
    glFrontFace(GL_CCW);
    glDisable(GL_TEXTURE_2D);
    // Nen toi ben trong mieng (nguoi 2 se ve dung nham de len o day).
    glColor3f(0.19f,0.10f,0.08f);
    glNormal3f(0,1,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,LAVA_FLOOR_Y-0.03f,0);
    for(int i=0;i<=SEG;++i) {
        const float a=2.0f*PI*i/SEG;
        glVertex3f(1.62f*std::cos(a),LAVA_FLOOR_Y-0.03f,1.62f*std::sin(a));
    }
    glEnd();
    glColor3f(1,1,1);
}
void drawSceneGeometry() { drawTerrain();drawVolcano(); }
void releaseTextures() {
    if(rockTexture) {glDeleteTextures(1,&rockTexture);rockTexture=0;}
    if(groundTexture) {glDeleteTextures(1,&groundTexture);groundTexture=0;}
}
float craterCenterX() {return 0.0f;}
float craterCenterY() {return CRATER_Y;}
float craterCenterZ() {return 0.0f;}
float craterRadius() {return CRATER_R;}
float lavaFloorY() {return LAVA_FLOOR_Y;}
}
