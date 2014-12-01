#include "pointlight.h"

PointLight::PointLight() :
    position(QVector3D(0,0,0)),
    colour(QColor(qRgb(255,255,255)))
{
}

PointLight::PointLight(float x, float y, float z) :
    position(QVector3D(x,y,z)),
    colour(QColor(qRgb(255,255,255)))
{
}

PointLight::PointLight(QVector3D position) :
    position(position),
    colour(QColor(qRgb(255,255,255)))
{
}

PointLight::PointLight(QVector3D position, QColor colour) :
    position(position),
    colour(colour)
{
}
