//-------------------------------------------------------------------------------------------
//   Painting with Flowsnakes
// fsnake program modified to use open gl vertex arrays  Brian Wyvill October 2012
// added save/restore and postscript driver November 2012
// fixed memory management November 2012 delete works properly now.
// added progress bar to show memory usage.
//-------------------------------------------------------------------------------------------

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
    //Background color will be white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glShadeModel( GL_FLAT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glPointSize(5);

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
    cerr << "gl new size "<< w SEP h NL;
    renderWidth = w;
    renderHeight = h;
}

// no mouse events in this demo
void GLWidget::mousePressEvent( QMouseEvent * )
{
}

void GLWidget::mouseReleaseEvent( QMouseEvent *)
{
}

void GLWidget::mouseMoveEvent ( QMouseEvent * )
{
}

// wheel event
void GLWidget::wheelEvent(QWheelEvent *)
{
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
        cerr << "Null Image\n";
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
                    // TODO-DG: The ray intersects this sphere. Determine the proper colour for the pixel on the screen.
                    myimage.setPixel(i, j, qRgb(255, 100, 100));
                } /*else {
                    // There is no intersection.
                    //myimage.setPixel(i, j, qRgb(0, 0, 0));
                }*/
            }
        }
    }

    qtimage=myimage.copy(0, 0,  myimage.width(), myimage.height());

    prepareImageDisplay(&myimage);
}

void GLWidget::about()
{
    QString vnum;
    QString mess, notes;
    QString title="Images in Qt and Opengl ";

    vnum.setNum ( MYVERSION );
    mess="Qt OpenGl image demo Release Version: ";
    mess = mess+vnum;
    notes = "\n\n News: Every QImage is now on stack, there is no way for memory leak. -- Lucky";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}

void GLWidget::help()
{
    QString vnum;
    QString mess, notes;
    QString title="qtglimages";

    vnum.setNum ( MYVERSION);
    mess="Simple Image Handling in Qt/Opengl by Brian Wyvill Release Version: ";
    mess = mess+vnum;
    notes = "\n\n Save and Load images for display.  Also Make an image such as output from a ray tracer. ";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}

