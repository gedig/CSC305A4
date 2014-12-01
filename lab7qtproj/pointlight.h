#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <QColor>
#include <QVector3D>

class PointLight
{
public:
    PointLight();
    PointLight(float x, float y, float z);
    PointLight(QVector3D position);
    PointLight(QVector3D position, QColor colour);

public:
    QVector3D position;
    QColor colour;
};

#endif // POINTLIGHT_H
