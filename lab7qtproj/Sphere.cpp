#include "sphere.h"

Sphere::Sphere() :
    origin(QVector3D(0,0,0)),
    radius(10.0f)
{
}
Sphere::Sphere(float x, float y, float z, float radius) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    surfaceColour(QColor(qRgb(100,100,100))),
    reflection(0.1f),
    phongExponent(10)
{
}

Sphere::Sphere(QVector3D origin, float radius) :
    origin(origin),
    radius(radius),
    surfaceColour(QColor(qRgb(100,100,100))),
    reflection(0.1f),
    phongExponent(10)
{
}

Sphere::Sphere(float x, float y, float z, float radius, QColor colour, float reflection) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    surfaceColour(colour),
    reflection(reflection)
{
        phongExponent = MAX_PHONG*(reflection*reflection*reflection);
}

Sphere::Sphere(QVector3D origin, float radius, QColor colour, float reflection) :
    origin(origin),
    radius(radius),
    surfaceColour(colour),
    reflection(reflection)
{
        phongExponent = MAX_PHONG*(reflection*reflection*reflection);
}
