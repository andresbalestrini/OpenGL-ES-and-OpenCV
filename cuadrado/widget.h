#ifndef WIDGET_H
#define WIDGET_H

#include<QTimer>

// aruco
#include <aruco/aruco.h>

// opencv headers
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <QOpenGLWidget>
#include<QMatrix4x4>
#include<QOpenGLShaderProgram>
#include<QOpenGLTexture>
#include<QOpenGLBuffer>
#include<QOpenGLContext>
#include<QOpenGLFunctions>

using namespace cv;
using namespace aruco;

struct VertexData
{
    VertexData()
    {

    }
    VertexData(QVector3D p, QVector2D t):
        position(p),textCoord(t)
    {
    }

    VertexData(QVector3D p, QVector2D t, QVector3D n):
        position(p),textCoord(t), normal(n)
    {
    }

    QVector3D position;
    QVector2D textCoord;
    QVector3D normal;
};

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initShaders();
    void initFace();

private:
    QMatrix4x4 m_projectionMatrix, m_orthoMatrix;
    QOpenGLShaderProgram m_program;
    QOpenGLTexture *m_texture,*cam_texture,*marker_texture;
    // buffers para cuadrado donde se pega la textura de la camara
    QOpenGLBuffer m_arrayBuffer;
    QOpenGLBuffer m_indexBuffer;

    // buffers para el cuadrado donde se pega la textura en el marker
    QOpenGLBuffer marker_arrayBuffer;
    QOpenGLBuffer marker_indexBuffer;


    // VideoCapture para levantar la camara numero device
    int device;
    VideoCapture *videoCapture;

    // timer que determina cada cuando se obtiene un nuevo Mat
    QTimer * sceneTimer;

    // para calibrar camara con aruco
    CameraParameters *cameraParameters;

    // donde guarda la imagen capturada por video frame
    Mat matCamera;

    // utilizada para detectar marcadores en la matriz
    MarkerDetector *markerDetector;
    QVector< Marker > detectedMarkers;

    void process( Mat &frame );

    void drawTextureInMarker(float sizeMarker, QMatrix4x4 proj, QMatrix4x4 view,unsigned int percentage = 100);


private slots:
    void slot_updateScene();


};

#endif // WIDGET_H
