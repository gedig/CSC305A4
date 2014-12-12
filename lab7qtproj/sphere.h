#ifndef SPHERE_H
#define SPHERE_H

// Radius and Position
#include <QVector3D>
#include "material.h"

class Sphere
{

public:
    Sphere();
    Sphere(float x, float y, float z, float radius);
    Sphere(QVector3D origin, float radius);
    Sphere(float x, float y, float z, float radius, Material material);
    Sphere(QVector3D origin, float radius, Material material);

public:
    QVector3D origin;
    float radius;
    Material material;
};

#endif // SPHERE_H
