#include "Vector2.h"
Vector2::Vector2() : Vector2(0, 0) {

}
Vector2::Vector2(double pX, double pY) {
    x = pX;
    y = pY;
}
Vector2 Vector2::Add(Vector2 other) {
    return *new Vector2(x + other.x, y + other.y);
}
double Vector2::FindDistance(Vector2 other) {
    return ((*this) - other).Magnitude();
}
Vector2 Vector2::operator-(Vector2 other) {
    return *new Vector2(x - other.x, y - other.y);
}
Vector2 Vector2::Normalise() {
    double magnitude = this->FindDistance(*new Vector2(0, 0));
    return *new Vector2(x / magnitude, y / magnitude);
}
Vector2 Vector2::operator*(const double factor) {
    return *new Vector2(x * factor, y * factor);
}
const double Vector2::Magnitude() {
    return sqrt(pow(x, 2) + pow(y, 2));
}