#include "sphere.h"

Sphere::Sphere() :
    origin(QVector3D(0,0,0)),
    radius(10.0f),
    material()
{
}
Sphere::Sphere(float x, float y, float z, float radius) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    material()
{
}

Sphere::Sphere(QVector3D origin, float radius) :
    origin(origin),
    radius(radius),
    material()
{
}

Sphere::Sphere(float x, float y, float z, float radius, Material material) :
    origin(QVector3D(x,y,z)),
    radius(radius),
    material(material)
{
}

Sphere::Sphere(QVector3D origin, float radius, Material material) :
    origin(origin),
    radius(radius),
    material(material)
{
}
