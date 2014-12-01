#ifndef SPHERE_H
#define SPHERE_H

// Radius and Position
#include <QVector3D>

class Sphere
{

public:
    Sphere();
    Sphere(float x, float y, float z, float radius);
    Sphere(QVector3D origin, float radius);

public:
    QVector3D origin;
    float radius;
};

#endif // SPHERE_H
