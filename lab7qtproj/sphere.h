#ifndef SPHERE_H
#define SPHERE_H

// Radius and Position
#include <QVector3D>
#include <QColor>

#define MAX_PHONG 10000

class Sphere
{

public:
    Sphere();
    Sphere(float x, float y, float z, float radius);
    Sphere(QVector3D origin, float radius);
    Sphere(float x, float y, float z, float radius, QColor colour, float reflection = 0.1f);
    Sphere(QVector3D origin, float radius, QColor colour, float reflection = 0.1f);

public:
    QVector3D origin;
    float radius;
    QColor surfaceColour;
    float reflection;
    int phongExponent;
};

#endif // SPHERE_H
