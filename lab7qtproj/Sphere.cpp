#include "sphere.h"

Sphere::Sphere() :
    origin(QVector3D(0,0,0)),
    radius(10.0f)
{
}
Sphere::Sphere(float x, float y, float z, float radius) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    surfaceColour(QColor(qRgb(100,100,100)))
{
}

Sphere::Sphere(QVector3D origin, float radius) :
    origin(origin),
    radius(radius),
    surfaceColour(QColor(qRgb(100,100,100)))
{
}

Sphere::Sphere(float x, float y, float z, float radius, QColor colour) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    surfaceColour(colour)
{
}

Sphere::Sphere(QVector3D origin, float radius, QColor colour) :
    origin(origin),
    radius(radius),
    surfaceColour(colour)
{
}
