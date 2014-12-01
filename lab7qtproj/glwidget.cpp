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
        sphereRadii.clear();
        spherePositions.clear();
        pointLightIntensities.clear();
        pointLightPositions.clear();
        QTextStream sceneDataStream(&sceneFile);
        QString line = sceneDataStream.readLine();
        QStringList parameterValue;
        while (!line.isNull()) {
            if (line[0] != '#') { // Ignores commented lines.
                parameterValue = line.split(":");
                if (parameterValue[0] == "sphere") {
                    QStringList sphereProperties = parameterValue[1].split(",");
                    sphereRadii.append(sphereProperties[0].toFloat());
                    spherePositions.append(QVector3D(sphereProperties[1].toFloat(), sphereProperties[2].toFloat(), sphereProperties[3].toFloat()));
                } else if (parameterValue[0] == "point-light") {
                    QStringList lightProperties = parameterValue[1].split(",");
                    pointLightIntensities.append(lightProperties[0].toFloat());
                    pointLightPositions.append(QVector3D(lightProperties[1].toFloat(), lightProperties[2].toFloat(), lightProperties[3].toFloat()));
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
            QVector3D directionVector = pixelPosition - cameraPoint;

            // Loop through every object to test for collision
            for (int k = 0; k < spherePositions.size(); k++) {
                float sphereRadius = sphereRadii[k];
                QVector3D sphereOrigin = spherePositions[k];
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
                // The index of the closest sphere is closestSphere Index.
                // TODO-DG: Determine the proper colour for the pixel on the screen.
                // TODO-DG: Compute n
                // TODO-DG: Evaluate Shading Model and set pixel to that colour

                // cameraPoint + smallestT * directionVector is the point on the closest sphere.



                //Pixel colour L = surface Colour x lightIntensity x max(0,normal.l
                // l is computed by subtracting intersection point from the light source position

                myimage.setPixel(i, j, qRgb(255, 100, 100));
            } else {
                // TODO-DG: If no intersections with spheres, try intersecting with planes.
                myimage.setPixel(i, j, qRgb(0, 0, 0));
            }
        }
    }

    qtimage=myimage.copy(0, 0,  myimage.width(), myimage.height());

    prepareImageDisplay(&myimage);
}

