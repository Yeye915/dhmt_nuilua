#ifndef VOLCANO_SCENE_H
#define VOLCANO_SCENE_H

// Module Nguoi 1: dia hinh, nui lua, texture thu tuc.
// C++98 + OpenGL fixed pipeline (GL/freeglut), khong phu thuoc file anh.
namespace VolcanoScene {
    void initTextures();
    void drawTerrain();
    void drawVolcano();
    void drawSceneGeometry();
    void releaseTextures();

    // Toa do cho module dung nham / lua / khoi cua Nguoi 2.
    float craterCenterX();
    float craterCenterY();
    float craterCenterZ();
    float craterRadius();
    float lavaFloorY();
}
#endif
