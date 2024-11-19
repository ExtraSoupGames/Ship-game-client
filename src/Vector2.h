#pragma once
#include "SDL.h"
using namespace std;
class Vector2 {
public:
    double x;
    double y;
    Vector2();
    Vector2(double pX, double pY);
    Vector2 Add(Vector2 other);
    double FindDistance(Vector2 other);
    Vector2 operator-(Vector2 other);
    Vector2 Normalise();
    Vector2 operator*(const double factor);
    const double Magnitude();
};