#include "glwidget.h"


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{

}

GLWidget::~GLWidget()
{    

}

void GLWidget::clear()
{
     updateGL();
}

void GLWidget::initializeGL()
{

    cameraPos = -100;
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    displayImage();
}

/* 2D */
void GLWidget::resizeGL( int w, int h )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.0,GLdouble(w),0,GLdouble(h),-10.0,10.0);
    glFlush();
    glMatrixMode(GL_MODELVIEW);
    glViewport( 0, 0, (GLint)w, (GLint)h );
    renderWidth = w;
    renderHeight = h;
}

void GLWidget::openImage(QString fileBuf)
{     
    QImage myImage;
    myImage.load(fileBuf);
    prepareImageDisplay(&myImage);
}

/**
 * @brief GLWidget::openScene
 * @param fileBuf name of the file to open
 *
 * This function attempts to open a scene file and, if successful,
 * parses it and populates the various object lists with the information.
 *
 */
void GLWidget::openScene(QString fileBuf)
{
    QFile sceneFile(fileBuf);
    if (sceneFile.open(QFile::ReadOnly)) {
        spheres.clear();
        pointLights.clear();
        QTextStream sceneDataStream(&sceneFile);
        QString line = sceneDataStream.readLine();
        QStringList parameterValue;
        while (!line.isNull()) {
            if (line[0] != '#') { // Ignores commented lines.
                parameterValue = line.split(":");
                if (parameterValue[0] == "sphere") {
                    QStringList sphereProperties = parameterValue[1].split(",");
                    if (sphereProperties.size() == 4) {
                        spheres.append(Sphere(QVector3D(sphereProperties[1].toFloat(), sphereProperties[2].toFloat(), sphereProperties[3].toFloat()), sphereProperties[0].toFloat()));
                    } else if (sphereProperties.size() == 8) {
                        int phongExponent = sphereProperties[0].toInt();
                        QColor sphereColour(qRgb(sphereProperties[1].toInt(), sphereProperties[2].toInt(), sphereProperties[3].toInt()));
                        float sphereRadius(sphereProperties[4].toFloat());
                        QVector3D spherePosition(sphereProperties[5].toFloat(), sphereProperties[6].toFloat(), sphereProperties[7].toFloat());
                        spheres.append(Sphere(spherePosition, sphereRadius, sphereColour, phongExponent));
                    }
                } else if (parameterValue[0] == "point-light") {
                    QStringList lightProperties = parameterValue[1].split(",");
                    if (lightProperties.size() == 3) {
                        pointLights.append(PointLight(QVector3D(lightProperties[1].toFloat(), lightProperties[2].toFloat(), lightProperties[3].toFloat()),lightProperties[0].toFloat()));
                    } else if (lightProperties.size() == 6) {
                        QVector3D position(lightProperties[3].toFloat(), lightProperties[4].toFloat(), lightProperties[5].toFloat());
                        QColor colour(qRgb(lightProperties[0].toFloat(), lightProperties[1].toFloat(), lightProperties[2].toFloat()));

                        pointLights.append(PointLight(position, colour));
                    }
                } else if (parameterValue[0] == "camera-z") {
                    bool conversionSuccessful = false;
                    float temp = parameterValue[1].toFloat(&conversionSuccessful);
                    if (conversionSuccessful)
                        cameraPos = temp;
                } else if (parameterValue[0] == "ambient-light") {
                    QStringList lightProperties = parameterValue[1].split(",");
                    if (lightProperties.size() == 1) {
                        ambientColour = QColor(lightProperties[0].toInt(), lightProperties[0].toInt(), lightProperties[0].toInt());
                    } else if (lightProperties.size() == 3) {
                        ambientColour = QColor(lightProperties[0].toInt(), lightProperties[1].toInt(), lightProperties[2].toInt());
                    }
                }
            }
            line = sceneDataStream.readLine();
        }
        sceneFile.close();
    } else {
        qDebug() << "File open failed";
    }
}

void GLWidget::prepareImageDisplay(QImage* myimage)
{   
    glimage = QGLWidget::convertToGLFormat( *myimage );  // flipped 32bit RGBA stored as mi
    updateGL();
}

void GLWidget::displayImage()
{
    if (glimage.width()==0) {
        return;
    } else {
        glRasterPos2i(0,0);
        glDrawPixels(glimage.width(), glimage.height(), GL_RGBA, GL_UNSIGNED_BYTE, glimage.bits());
        glFlush();
    }
}

void GLWidget::saveImage( QString fileBuf )
{
    // there is no conversion  back toQImage
    // hence the need to keep qtimage as well as glimage
    qtimage.save ( fileBuf );   // note it is the qtimage in the right format for saving
}

/**
 * @brief GLWidget::makeImage
 *
 * This is the function that loops through each pixel to determine which colour
 * value it should display.
 */
void GLWidget::makeImage( )
{   
    QImage myimage(renderWidth, renderHeight, QImage::Format_RGB32);

    QVector3D cameraPoint(renderWidth/2, renderHeight/2, cameraPos);
    for (int i = 0; i < renderWidth; i++) {
        for (int j = 0; j < renderHeight; j++) {

            int closestSphereIndex = -1;
            //QVector3D pixelPosition(i, renderHeight - j - 1, 0); // Accounts for the flip between the image y and world y.
            QVector3D pixelPosition(i, j, 0);
            QVector3D directionVector = (pixelPosition - cameraPoint).normalized();

            QVector3D intersectionPoint;

            closestSphereIndex = intersectSpheres(cameraPoint, directionVector, intersectionPoint);

            if (closestSphereIndex > -1) {
                // We have a collision, now we must determine the proper colour
                    // for the pixel on the screen.

                Sphere hitSphere = spheres[closestSphereIndex];

                // cameraPoint + smallestT * directionVector is the point on the closest sphere.
                QVector3D normalVector = (intersectionPoint - hitSphere.origin).normalized();
                QVector3D viewVector = (cameraPoint - intersectionPoint).normalized();

                // TODO-DG: Currently this crudely adds the effects of the last light in the scene.
                // TODO-DG: Update to take an average

                // The initial value is the effect of the ambient lighting.
                float pixelR = hitSphere.surfaceColour.redF()*ambientColour.redF();
                float pixelG = hitSphere.surfaceColour.greenF()*ambientColour.greenF();
                float pixelB = hitSphere.surfaceColour.blueF()*ambientColour.blueF();
                for ( int k = 0; k < pointLights.size(); k++) {
                    QVector3D lightVector = (pointLights[k].position - intersectionPoint).normalized();
                    QVector3D shadowCollisionPoint;
                    // Test for objects in between light and intersection point
                    int shadowSphereIndex = intersectSpheres(intersectionPoint + lightVector, lightVector, shadowCollisionPoint);
                    if (shadowSphereIndex == -1) { // No intersection
                        //LAMBERTIAN DIFFUSE SHADING:
                        // L += surface Colour x lightIntensity x max(0,normal.lightVector)
                        // lightVector is computed by subtracting intersection point from the light source position
                        float nDotL = (float)QVector3D::dotProduct(normalVector,lightVector);
                        if ( nDotL > 0) {
                            pixelR += hitSphere.surfaceColour.redF()*pointLights[k].colour.redF()*nDotL;
                            pixelG += hitSphere.surfaceColour.greenF()*pointLights[k].colour.greenF()*nDotL;
                            pixelB += hitSphere.surfaceColour.blueF()*pointLights[k].colour.blueF()*nDotL;
                        }

                        //BLINN-PHONG SPECULAR SHADING:
                        // L += surface specular * lightIntensity * max(0,normal.h)^p
                        // h is (v + l) normalized.
                        // p is a Phong exponent, defined on the sphere.
                        QVector3D halfVector = (viewVector + lightVector).normalized();
                        float nDotH = (float)QVector3D::dotProduct(normalVector,halfVector);
                        if (nDotH > 0) {
                            nDotH = pow(nDotH, hitSphere.phongExponent);
                            // TODO-DG: Figure out what to use for the specular colour of the object.
    //                        pixelR += hitSphere.surfaceColour.redF()*pointLights[k].colour.redF()*nDotH;
    //                        pixelG += hitSphere.surfaceColour.greenF()*pointLights[k].colour.greenF()*nDotH;
    //                        pixelB += hitSphere.surfaceColour.blueF()*pointLights[k].colour.blueF()*nDotH;
                            pixelR += 0.8f*pointLights[k].colour.redF()*nDotH;
                            pixelG += 0.8f*pointLights[k].colour.greenF()*nDotH;
                            pixelB += 0.8f*pointLights[k].colour.blueF()*nDotH;
                        }
                    }

                }
                if (pixelR > 1.0f)
                    pixelR = 1.0f;
                if (pixelG > 1.0f)
                    pixelG = 1.0f;
                if (pixelB > 1.0f)
                    pixelB = 1.0f;

                myimage.setPixel(i, renderHeight - 1 -j, qRgb(pixelR*255, pixelG*255, pixelB*255));
            } else {
                // TODO-DG: If no intersections with spheres, try intersecting with planes.

                myimage.setPixel(i, renderHeight - 1 - j, qRgb(0, 0, 0));
            }
        }
    }

    qtimage=myimage.copy(0, 0,  myimage.width(), myimage.height());

    prepareImageDisplay(&myimage);
}

/**
 * @brief intersectSphere
 * @param initialPoint
 * @param direction
 * @param intersectionPoint passed in by reference, this will be updated to the point where the ray intersects the sphere
 * @return the distance to the closest sphere intersected, or -1 if none.
 *
 * Utility function to easily test for ray-sphere intersection.
 */
int GLWidget::intersectSpheres(QVector3D initialPosition, QVector3D direction, QVector3D& intersectionPoint)
{
    float smallestT = -1;
    int closestSphereIndex = -1;
    // Loop through every object to test for collision
    for (int i = 0; i < spheres.size(); i++) {
        float sphereRadius = spheres[i].radius;
        QVector3D sphereOrigin = spheres[i].origin;
        QVector3D rayOriginMinusSphereCenter = initialPosition - sphereOrigin;
        // Ray: R = CameraPoint + t D;

        float partA = direction.lengthSquared();
        float partB = QVector3D::dotProduct(direction, rayOriginMinusSphereCenter);
        float partC = rayOriginMinusSphereCenter.lengthSquared() - (sphereRadius*sphereRadius);

        float discriminant = (partB * partB) - (partA * partC);
        if (discriminant >= 0) {
            float t = (-1*partB -  sqrt(discriminant)) / partA;
            if (t > 0) {
                if (t < smallestT || smallestT == -1) {
                    smallestT = t;
                    closestSphereIndex = i;
                }
            }
        }
    }
    if (closestSphereIndex > -1) {
        intersectionPoint = initialPosition + (smallestT * direction.normalized());
    }
    return closestSphereIndex;

}

