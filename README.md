# DO AN DO HOA MAY TINH - NGUOI 1: NUI LUA, DIA HINH, TEXTURE

## Pham vi hoan thanh
- Dia hinh 3D dang heightmap tao tu value noise; mat dat co doi nhe bao quanh nui.
- Nui lua 3D mesh vong tron voi suon doc, mieng rong va long chao ben trong.
- Mat da va mat dat su dung procedural texture OpenGL, khong phai tai anh.
- Phap tuyen cho anh sang; toa do giao tiep de nguoi 2 ve dung nham/lua/khoi.
- `demo_main.cpp` chi la chuong trinh demo de xem model; camera/anh sang chinh do Nguoi 3 dam nhan.

## Cau truc
```
NuiLua_OpenGL/
  src/VolcanoScene.h       // API module
  src/VolcanoScene.cpp     // dia hinh, mesh, texture
  src/demo_main.cpp        // cua so xem rieng
  Makefile                 // build Linux
```

## Chay Ubuntu (VM hoac may that)
```
sudo apt update
sudo apt install -y build-essential freeglut3-dev libglu1-mesa-dev
cd NuiLua_OpenGL
make
./volcano_demo
```
Phai dang trong phien desktop Ubuntu co ho tro hien thi OpenGL. May ao VirtualBox can cau hinh do hoa phu hop.

## Chay Windows / Code::Blocks / MinGW
Cai dat FreeGLUT vao compiler, dam bao compiler tim thay `GL/glut.h`, `GL/gl.h`.
Them `src/demo_main.cpp`, `src/VolcanoScene.cpp` vao project. Link: `opengl32`, `glu32`, `freeglut` (hoac `glut32`, tuy bo thu vien). Neu dung DLL, chep `freeglut.dll` vao canh exe.
Compiler su dung C++98.

## Dieu khien demo
- A / D: Xoay quanh nui
- W / S: Tang/giam goc nhin
- + / -: Thu/phong
- F: Bat/tat wireframe
- L: Bat/tat anh sang
- ESC: Thoat

## Tich hop vao project chung
Trong chuong trinh chinh cua Nguoi 3:
```
#include "VolcanoScene.h"
// Sau khi tao OpenGL context:
VolcanoScene::initTextures();
// Trong render loop, SAU KHI camera va lighting da duoc cau hinh:
VolcanoScene::drawSceneGeometry();
// Khi thoat (neu con context):
VolcanoScene::releaseTextures();
```
Khong them `demo_main.cpp` vao chuong trinh chinh da co `main()`.

## Thong so de Nguoi 2 tich hop
- Tam mieng nui: `(0, 16, 0)`; huong truc Y len.
- Ban kinh mep mieng: `3.4` (world units).
- San day long chao / dung nham: `Y = 12.0`.
- Tam nui: `(0,0,0)`; chan nui ban kinh khoang `19.5`.
- Goi `VolcanoScene::craterCenterX/Y/Z()`, `craterRadius()`, `lavaFloorY()` thay vi viet so cung.
- Khi ve particle, ve tren mieng (Y >= 16); dung nham trong mieng co the ve o Y >= 12.05.

## Thuat toan
1. **Heightmap**: moi diem `(x,z)` duoc tinh do cao `y` tu noise da tan; vung gan nui duoc lam phang de tranh giao nhau.
2. **Radial profile**: cac moc ban kinh/do cao tinh hinh chop cong, mieng loe va long chao.
3. **Mesh**: lay mau theo ban kinh va goc, noi cac dinh bang quad strips; ap nhiễu nho tao vach da tu nhien.
4. **Texture**: tao anh RGB 128x128 bang noise, dat GL_REPEAT va mipmap de tranh nhoe/hien tuong rang cua.
5. **Phong sang**: tinh vector phap tuyen tu dao ham do cao, cho anh sang bam theo mat da.

## Phan cong ro rang
- Nguoi 1: model nui + mat dat + texture (`VolcanoScene.*`)
- Nguoi 2: lava, fire, smoke, particles (module khac; chua lam trong goi nay)
- Nguoi 3: camera, lighting, UI, keyboard, tich hop (chi co camera demo trong `demo_main.cpp`)
