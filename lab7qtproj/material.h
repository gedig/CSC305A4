#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>

class Material
{
public:
    Material();
    Material(QColor ambient, QColor diffuse, QColor specular, float shininess);

    QColor ambient;
    QColor diffuse;
    QColor specular;

    float phongExponent;

};

#endif // MATERIAL_H
