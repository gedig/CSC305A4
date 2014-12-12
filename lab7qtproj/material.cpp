#include "material.h"

// Some material information found at http://globe3d.sourceforge.net/g3d_html/gl-materials__ads.htm

// This default material is defined as a neutral material from above link.
Material::Material() :
    ambient(51, 51, 51),
    diffuse(201, 201, 201),
    specular(0, 0, 0),
    phongExponent(0)
{
}

Material::Material(QColor ambient, QColor diffuse, QColor specular, float shininess):
    ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    phongExponent(shininess)
{
}
