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

    QVector3D position;
    QVector2D textCoord;    
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
    void initFace(float width, float height);

private:
    QMatrix4x4 m_projectionMatrix;
    QOpenGLShaderProgram m_program;
    QOpenGLTexture *m_texture,*cam_texture;
    QOpenGLBuffer m_arrayBuffer;
    QOpenGLBuffer m_indexBuffer;


    // VideoCapture para levantar la camara numero device
    int device;
    VideoCapture *videoCapture;

    // timer que determina cada cuando se obtiene un nuevo Mat
    QTimer * sceneTimer;

    // para calibrar camara con aruco
    CameraParameters *cameraParameters;

    // donde guarda la imagen capturada por video frame
    Mat matCamera;

private slots:
    void slot_updateScene();


};

#endif // WIDGET_H
