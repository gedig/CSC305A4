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
                    } else if (sphereProperties.size() == 7) {
                        QColor sphereColour(qRgb(sphereProperties[0].toInt(), sphereProperties[1].toInt(), sphereProperties[2].toInt()));
                        float sphereRadius(sphereProperties[3].toFloat());
                        QVector3D spherePosition(sphereProperties[4].toFloat(), sphereProperties[5].toFloat(), sphereProperties[6].toFloat());
                        spheres.append(Sphere(spherePosition, sphereRadius, sphereColour));
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

void GLWidget::makeImage( )
{   
    QImage myimage(renderWidth, renderHeight, QImage::Format_RGB32);

    QVector3D cameraPoint(renderWidth/2, renderHeight/2, cameraPos);
    for (int i = 0; i < renderWidth; i++) {
        for (int j = 0; j < renderHeight; j++) {
            float smallestT = -1;
            int closestSphereIndex = -1;
            QVector3D pixelPosition(i, j, 0);
            QVector3D directionVector = (pixelPosition - cameraPoint).normalized();

            // Loop through every object to test for collision
            for (int k = 0; k < spheres.size(); k++) {
                float sphereRadius = spheres[k].radius;
                QVector3D sphereOrigin = spheres[k].origin;
                QVector3D rayOriginMinusSphereCenter = cameraPoint - sphereOrigin;
                // Ray: R = CameraPoint + t D;

                float partA = directionVector.lengthSquared();
                float partB = QVector3D::dotProduct(directionVector, rayOriginMinusSphereCenter);
                float partC = rayOriginMinusSphereCenter.lengthSquared() - (sphereRadius*sphereRadius);

                float discriminant = (partB * partB) - (partA * partC);
                if (discriminant >= 0) {
                    float t = (-1*partB -  sqrt(discriminant)) / partA;

                    if (t < smallestT || smallestT == -1) {
                        smallestT = t;
                        closestSphereIndex = k;
                    }
                }
            }
            if (closestSphereIndex > -1) {
                // We have a collision, now we must determine the proper colour
                    // for the pixel on the screen.

                Sphere hitSphere = spheres[closestSphereIndex];

                // cameraPoint + smallestT * directionVector is the point on the closest sphere.
                QVector3D intersectionPoint = cameraPoint + (smallestT * directionVector.normalized());
                QVector3D normalVector = (hitSphere.origin - intersectionPoint).normalized();
                // TODO-DG: Currently this crudely adds the effects of the last light in the scene.
                // TODO-DG: Update to take an average
                float pixelR = 0;
                float pixelG = 0;
                float pixelB = 0;
                for ( int k = 0; k < pointLights.size(); k++) {
                    QVector3D lVector = (pointLights[k].position - intersectionPoint).normalized();
                    pixelR += hitSphere.surfaceColour.redF()*pointLights[k].colour.redF()*qMax(0.0f, (float)QVector3D::dotProduct(normalVector,lVector));
                    pixelG += hitSphere.surfaceColour.greenF()*pointLights[k].colour.greenF()*qMax(0.0f, (float)QVector3D::dotProduct(normalVector,lVector));
                    pixelB += hitSphere.surfaceColour.blueF()*pointLights[k].colour.blueF()*qMax(0.0f, (float)QVector3D::dotProduct(normalVector,lVector));
                }


                //Pixel colour L = surface Colour x lightIntensity x max(0,normal.l
                // l is computed by subtracting intersection point from the light source position

                myimage.setPixel(i, j, qRgb(pixelR*255, pixelG*255, pixelB*255));
            } else {
                // TODO-DG: If no intersections with spheres, try intersecting with planes.
                myimage.setPixel(i, j, qRgb(0, 0, 0));
            }
        }
    }

    qtimage=myimage.copy(0, 0,  myimage.width(), myimage.height());

    prepareImageDisplay(&myimage);
}

