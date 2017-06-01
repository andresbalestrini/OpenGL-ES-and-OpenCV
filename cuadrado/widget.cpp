#include "widget.h"
#include<QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent), m_texture(0),device(0),
      videoCapture ( new cv::VideoCapture( device ) ),
      sceneTimer(new QTimer),m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    if ( ! videoCapture->open(0))  {
            QMessageBox::critical(this, "No se pudo iniciar camara", "Problema con videoCapture->open");
    }    
    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_updateScene() ) );
}

Widget::~Widget()
{

}
/*
 * Inicializo la textura cam_texture que es la que contendra
 * la textura captada por la camara, dandole el formato 'RGBA8_UNorm'
*/
void Widget::initializeGL()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    cam_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    cam_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    cam_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    cam_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    initShaders();
    initFace(this->width(),this->height());
}

void Widget::resizeGL(int w, int h)
{
    float aspect= w/(h?(float)h:1);
    glViewport( 0, 0, w, h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45,aspect, 0.1,10.0f);
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();
    modelViewMatrix.translate(0.0,0.0,-5.0);

    if(matCamera.rows > 0){
        // si ya se tomo la imagen y se cargo en la matriz 'matCamera' bindeamos la textura para dibujarla
        cam_texture->bind();
    }

    m_program.bind();
    m_program.setUniformValue("qt_ModelViewProjectionMatrix",m_projectionMatrix*modelViewMatrix);
    m_program.setUniformValue("qt_Texture0",0);

    m_arrayBuffer.bind();

    int offset = 0;

    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc,GL_FLOAT,offset,3,sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texLoc =m_program.attributeLocation("qt_MultiTexCoord0");
    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc,GL_FLOAT,offset,2,sizeof(VertexData));

    m_indexBuffer.bind();

    glDrawElements(GL_TRIANGLES,m_indexBuffer.size(),GL_UNSIGNED_INT,0);
}

void Widget::initShaders()
{
    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
        close();

    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh"))
        close();

    if(m_program.link())
        close();
}
/*
 * Primero en vertexes defino con QVector3D los cuatro puntos del cuadrado donde se
 * representara la imagen captada por la camara, luego con QVector2D tomo cuatro
 * vertices de la imagen de forma que sea la imagen completa la que se pegue en el
 * cuadrado.
 *
 * Segundo, los indexes para formar los dos triangulos que formaran el cuadrado, se nota que
 * los indices 2 y 1 se repiten, esto es para que con esos mismos puntos de este primer
 * triangulo se forme el siguiente, de esta forma:
 *
 *              2 * * * 3
 *              * *     *
 *              *   *   *
 *              *     * *
 *              0 * * * 1
 *
 * Luego, al final de armar la geometria, se inicia el qtimer q comienza a tomar las imagenes de la camara
 */
void Widget::initFace(float width,float height)
{      

      QVector<VertexData> vertexes;
      vertexes.append(VertexData(QVector3D(-1.0f,-1.0f,1.0f),QVector2D(1.0f,1.0f)));
      vertexes.append(VertexData(QVector3D(1.0f,-1.0f,1.0f),QVector2D(0.0f,1.0f)));
      vertexes.append(VertexData(QVector3D(-1.0f,1.0f,1.0f),QVector2D(1.0f,0.0f)));
      vertexes.append(VertexData(QVector3D(1.0f,1.0f,1.0f),QVector2D(0.0f,0.0f)));

      QVector<GLuint> indexes;
      indexes.append(0);
      indexes.append(1);
      indexes.append(2);
      indexes.append(2);
      indexes.append(1);
      indexes.append(3);

      m_arrayBuffer.create();
      m_arrayBuffer.bind();
      m_arrayBuffer.allocate(vertexes.constData(),vertexes.size()*sizeof(VertexData));
      m_arrayBuffer.release();

      m_indexBuffer.create();
      m_indexBuffer.bind();
      m_indexBuffer.allocate(indexes.constData(), indexes.size()*sizeof(GLuint));
      m_indexBuffer.release();

//      m_texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

//      m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
//      m_texture->setMinificationFilter(QOpenGLTexture::Linear);
//      m_texture->setWrapMode(QOpenGLTexture::Repeat);
      sceneTimer->start( 10 );
}

/*
 * Cada 10 milisegundos este slot es llamado de forma que actualiza la imagen tomada
 * por la camara, en la primer vuelta como el tamaño de cam_texture es por defecto (width = 1) permito
 * que se modifique con setSize(), en las actualizaciones siguientes solo se captura la imagen guardandola
 * en matCamera y se pasa a cam_texture la nueva textura tomada para pintarla, se ejecuta update() y se
 * dibuja la nueva imagen tomada por la camara
 */
void Widget::slot_updateScene()
{
    if(!this->videoCapture->isOpened()){  // check if we succeeded
        qDebug() << "videocapture no se abrio correctamente";
        return;
    }        
    // levanta la camara y guarda la imagen en matCamera
    this->videoCapture->operator >>(this->matCamera);

    // ********************** PARA CAMARA ****************************************************************
    cv::Mat temp(matCamera.cols, matCamera.rows, matCamera.type());
    cvtColor(matCamera, temp, CV_BGR2RGB);
    if(cam_texture->width() == 1) cam_texture->setSize(matCamera.cols,matCamera.rows);
    cam_texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, temp.data);
    cam_texture->allocateStorage();
    qDebug()<<matCamera.cols;
    qDebug()<<matCamera.rows;
    qDebug()<<"\n\n";
    this->update();
}
