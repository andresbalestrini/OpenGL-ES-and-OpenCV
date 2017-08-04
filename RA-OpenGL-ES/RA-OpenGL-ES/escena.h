#ifndef ESCENA_H
#define ESCENA_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QVector>
#include <QTimer>
#include <QVector3D>
#include <QVector2D>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <aruco/aruco.h>

#include "geometryengine.h"
#include "video.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

class QOpenGLShaderProgram;
class QOpenGLTexture;
class QOpenGLBuffer;

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

class Escena : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Escena(QWidget *parent = 0);
    ~Escena();

//    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
//    QSize sizeHint() const Q_DECL_OVERRIDE;

    GLfloat x, y, z, angulo;

signals:
    void clicked();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL( int w, int h ) Q_DECL_OVERRIDE;

private:    

    void crearProgram();
    void crearCam();
    void crearImage();
    void drawCamera();

    QOpenGLShaderProgram * program;
    QOpenGLBuffer * cam_buffer;
    QOpenGLTexture * cam_texture;

    QOpenGLBuffer * image_buffer;
    QOpenGLTexture * image_texture;

    QVector< QOpenGLBuffer * > * vImageBuffer;
    QVector< QOpenGLTexture * > * vImageTexture;

    VideoCapture *videoCapture;

    QTimer *sceneTimer;
    int milisecondsTimer;

    MarkerDetector * markerDetector;
    std::vector< Marker > detectedMarkers;
    QVector< Video * > *videos;
    CameraParameters *cameraParameters;

    Mat matCamera;

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    void process(Mat &frame);

    GeometryEngine *geometries;
    QQuaternion rotation;

    void loadTextures();
    void loadVideos();

private slots:
    void slot_updateScene();



};

#endif // ESCENA_H

