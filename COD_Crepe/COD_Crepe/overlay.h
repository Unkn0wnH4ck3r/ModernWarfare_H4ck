#pragma once
void InitializeDx12Hook();
void imDrawTextA(int x, int y, int a, int r, int g, int b, const char *fmt, ...);
void imDrawRect(int x, int y, int width, int height, int nL, int a, int r, int g, int b);
void imDrawRectFilled(int x, int y, int width, int height, int a, int r, int g, int b);
void imDrawLine(int x1, int y1, int x2, int y2, int a, int r, int g, int b, int thickness);
void imDrawCircle(int x, int y, float radius, int a, int r, int g, int b, int segments, int thickness);
void imDrawCircleFilled(int x, int y, float radius, int a, int r, int g, int b, int segments);