#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>

class Material
{
public:
    Material();
    Material(QColor ambient, QColor diffuse, QColor specular, float shininess, float refractionIndex = 1.0f);

    QColor ambient;
    QColor diffuse;
    QColor specular;

    float phongExponent;
    float refractionIndex;

};

#endif // MATERIAL_H
