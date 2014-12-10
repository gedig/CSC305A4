#ifndef SPHERE_H
#define SPHERE_H

// Radius and Position
#include <QVector3D>
#include <QColor>

class Sphere
{

public:
    Sphere();
    Sphere(float x, float y, float z, float radius);
    Sphere(QVector3D origin, float radius);
    Sphere(float x, float y, float z, float radius, QColor colour, int phong = 10);
    Sphere(QVector3D origin, float radius, QColor colour, int phong = 10);

public:
    QVector3D origin;
    float radius;
    QColor surfaceColour;
    int phongExponent;
};

#endif // SPHERE_H
