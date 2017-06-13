#include "widget.h"
#include<QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent), m_texture(0),device(0),
      videoCapture ( new cv::VideoCapture( device ) ), markerDetector( new MarkerDetector ),
      sceneTimer(new QTimer),m_indexBuffer(QOpenGLBuffer::IndexBuffer),
      cameraParameters( new CameraParameters)
{
    if ( ! videoCapture->open(0))  {
            QMessageBox::critical(this, "No se pudo iniciar camara", "Problema con videoCapture->open");
    }

    cameraParameters->readFromXMLFile( "../cuadrado/files/CameraParameters.yml" );

    if ( ! cameraParameters->isValid() )
        QMessageBox::critical(this, "Error con YML", "No es valido");

    sceneTimer->start( 10 );
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
    // creo la textura la contendra la imagen que captura el videoframe
    cam_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    cam_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    cam_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    cam_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);

    // creo la textura que contendra la imagen que debe pegarse en el marker
    marker_texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());
    marker_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    marker_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    marker_texture->setWrapMode(QOpenGLTexture::Repeat);
    initShaders();
    initFace();
}

void Widget::resizeGL(int w, int h)
{    
    glViewport( 0, 0, w, h );
//    m_projectionMatrix.setToIdentity();
//    m_projectionMatrix.perspective(45,w/(float)h,0.1f,10.0f);
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();    

    modelViewMatrix.translate(0.0,0.0,-5.0);

    // matriz orthonormal
    m_orthoMatrix.ortho(-1.0f, +1.0f, +1.0f, -1.0f, -10.0f, 100.0f);

    if(matCamera.rows > 0){
        // si ya se tomo la imagen y se cargo en la matriz 'matCamera' bindeamos la textura para dibujarla        
        cam_texture->bind();        
    }

    m_program.bind();
//    m_program.setUniformValue("qt_ModelViewProjectionMatrix",m_projectionMatrix*modelViewMatrix);    
    m_program.setUniformValue("projection",m_orthoMatrix);
    m_program.setUniformValue("modelview",modelViewMatrix);
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

    context()->functions()->glDrawElements(GL_TRIANGLES,m_indexBuffer.size(),GL_UNSIGNED_INT,0);

    // Deberia graficar en el marker si es que hay detectado uno
    if(detectedMarkers.length() != 0)  {
        QMatrix4x4 maker_modelViewMatrix, maker_projectionMatrix;
        maker_modelViewMatrix.setToIdentity();
        maker_projectionMatrix.setToIdentity();

        double projectionMatrix[16];
        cv::Size2i sceneSize( this->width(), this->height() );
        cv::Size2i openGlSize( this->width(), this->height() );
        cameraParameters->glGetProjectionMatrix( sceneSize, openGlSize, projectionMatrix, 0.05, 10 );

        double modelview_matrix[16];
        detectedMarkers.operator []( 0 ).glGetModelViewMatrix( modelview_matrix );

        // VER LOS 16 VALORES DE modelview_matrix Y projectionMatrix
//        qDebug()<<"\nprojectionMatrix:";
//        qDebug()<<projectionMatrix[0];
//        qDebug()<<projectionMatrix[1];
//        qDebug()<<projectionMatrix[2];
//        qDebug()<<projectionMatrix[3];
//        qDebug()<<projectionMatrix[4];
//        qDebug()<<projectionMatrix[5];
//        qDebug()<<projectionMatrix[6];
//        qDebug()<<projectionMatrix[7];
//        qDebug()<<projectionMatrix[8];
//        qDebug()<<projectionMatrix[9];
//        qDebug()<<projectionMatrix[10];
//        qDebug()<<projectionMatrix[11];
//        qDebug()<<projectionMatrix[12];
//        qDebug()<<projectionMatrix[13];
//        qDebug()<<projectionMatrix[14];
//        qDebug()<<projectionMatrix[15];

//        qDebug()<<"\nmodelview_matrix:";
//        qDebug()<<modelview_matrix[0];
//        qDebug()<<modelview_matrix[1];
//        qDebug()<<modelview_matrix[2];
//        qDebug()<<modelview_matrix[3];
//        qDebug()<<modelview_matrix[4];
//        qDebug()<<modelview_matrix[5];
//        qDebug()<<modelview_matrix[6];
//        qDebug()<<modelview_matrix[7];
//        qDebug()<<modelview_matrix[8];
//        qDebug()<<modelview_matrix[9];
//        qDebug()<<modelview_matrix[10];
//        qDebug()<<modelview_matrix[11];
//        qDebug()<<modelview_matrix[12];
//        qDebug()<<modelview_matrix[13];
//        qDebug()<<modelview_matrix[14];
//        qDebug()<<modelview_matrix[15];

//        maker_projectionMatrix(0,0) = projectionMatrix[0];
//        maker_projectionMatrix(1,0) = projectionMatrix[1];
//        maker_projectionMatrix(2,0) = projectionMatrix[2];
//        maker_projectionMatrix(3,0) = projectionMatrix[3];
//        maker_projectionMatrix(0,1) = projectionMatrix[4];
//        maker_projectionMatrix(1,1) = projectionMatrix[5];
//        maker_projectionMatrix(2,1) = projectionMatrix[6];
//        maker_projectionMatrix(3,1) = projectionMatrix[7];
//        maker_projectionMatrix(0,2) = projectionMatrix[8];
//        maker_projectionMatrix(1,2) = projectionMatrix[9];
//        maker_projectionMatrix(2,2) = projectionMatrix[10];
//        maker_projectionMatrix(3,2) = projectionMatrix[11];
//        maker_projectionMatrix(0,3) = projectionMatrix[12];
//        maker_projectionMatrix(1,3) = projectionMatrix[13];
//        maker_projectionMatrix(2,3) = projectionMatrix[14];
//        maker_projectionMatrix(3,3) = projectionMatrix[15];

//        maker_modelViewMatrix(0,0) = modelview_matrix[0];
//        maker_modelViewMatrix(1,0) = modelview_matrix[1];
//        maker_modelViewMatrix(2,0) = modelview_matrix[2];
//        maker_modelViewMatrix(3,0) = modelview_matrix[3];
//        maker_modelViewMatrix(0,1) = modelview_matrix[4];
//        maker_modelViewMatrix(1,1) = modelview_matrix[5];
//        maker_modelViewMatrix(2,1) = modelview_matrix[6];
//        maker_modelViewMatrix(3,1) = modelview_matrix[7];
//        maker_modelViewMatrix(0,2) = modelview_matrix[8];
//        maker_modelViewMatrix(1,2) = modelview_matrix[9];
//        maker_modelViewMatrix(2,2) = modelview_matrix[10];
//        maker_modelViewMatrix(3,2) = modelview_matrix[11];
//        maker_modelViewMatrix(0,3) = modelview_matrix[12];
//        maker_modelViewMatrix(1,3) = modelview_matrix[13];
//        maker_modelViewMatrix(2,3) = modelview_matrix[14];
//        maker_modelViewMatrix(3,3) = modelview_matrix[15];

//        maker_modelViewMatrix.rotate(detectedMarkers.at(0).Rvec.at<double>(0,0),detectedMarkers.at(0).Rvec.at<double>(0,1),detectedMarkers.at(0).Rvec.at<double>(0,2));
//        maker_modelViewMatrix.translate(detectedMarkers.at(0).Tvec.at<double>(0,0),detectedMarkers.at(0).Tvec.at<double>(0,1),detectedMarkers.at(0).Tvec.at<double>(0,2));
//        modelViewMatrix.rotate(detectedMarkers.at(0).Rvec.at<double>(0,0),detectedMarkers.at(0).Rvec.at<double>(0,1),detectedMarkers.at(0).Rvec.at<double>(0,2));
//        modelViewMatrix.translate(detectedMarkers.at(0).Tvec.at<double>(0,0),detectedMarkers.at(0).Tvec.at<double>(0,1),detectedMarkers.at(0).Tvec.at<double>(0,2));
//        m_program.setUniformValue("projection",maker_projectionMatrix);
//        m_program.setUniformValue("modelview",maker_modelViewMatrix);
        drawTextureInMarker(detectedMarkers.at(0).ssize,maker_projectionMatrix,maker_modelViewMatrix);
    }

    cam_texture->release();
    m_arrayBuffer.release();
    m_indexBuffer.release();
    this->update();
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
void Widget::initFace()
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

    process(matCamera);


    // ********************** PARA CAMARA ****************************************************************
    cv::Mat temp(matCamera.cols, matCamera.rows, matCamera.type());
    cvtColor(matCamera, temp, CV_BGR2RGB);
    if(cam_texture->width() == 1) cam_texture->setSize(matCamera.cols,matCamera.rows);
    cam_texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, temp.data);
    cam_texture->allocateStorage();
    this->update();
}

/*
 * Detecta el marker y dibuja el contorno en el mismo
 */
void Widget::process( Mat &frame )
{
    cameraParameters->resize( frame.size() );
    Mat grayscaleMat; cvtColor( frame, grayscaleMat, CV_BGR2GRAY );
    Mat binaryMat; threshold( grayscaleMat, binaryMat, 128, 255, cv::THRESH_BINARY );

    std::vector< Marker > detectedMarkersVector;
    cameraParameters->resize( binaryMat.size() );
    markerDetector->detect( binaryMat, detectedMarkersVector, *cameraParameters, 0.57f );
    detectedMarkers = QVector< Marker >::fromStdVector( detectedMarkersVector );    

    // descripcion del marker ( dibuja el contorno del marker )

    for( int i = 0; i < detectedMarkers.size(); i++ )
        detectedMarkers.at( i ).draw( frame, Scalar( 255, 0, 255 ), 1 );
}

void Widget::drawTextureInMarker(float sizeMarker,  QMatrix4x4 proj, QMatrix4x4 view, unsigned int percentage)
{
    sizeMarker = sizeMarker * (float)percentage / (float)100;    
    QVector<VertexData> vertexes;
    vertexes.append(VertexData(QVector3D(-sizeMarker/2,  sizeMarker/2, 0),QVector2D(0.0f,0.0f),QVector3D(0.0f, 0.0f,-1.0f)));
    vertexes.append(VertexData(QVector3D(sizeMarker/2,  sizeMarker/2, 0),QVector2D(1.0f,0.0f),QVector3D(0.0f, 0.0f,-1.0f)));
    vertexes.append(VertexData(QVector3D(-sizeMarker/2, -sizeMarker/2, 0),QVector2D(0.0f,1.0f),QVector3D(0.0f, 0.0f,-1.0f)));
    vertexes.append(VertexData(QVector3D(sizeMarker/2, -sizeMarker/2, 0),QVector2D(1.0f,1.0f),QVector3D(0.0f, 0.0f,-1.0f)));

    QVector<GLuint> indexes;
    indexes.append(0);
    indexes.append(1);
    indexes.append(2);
    indexes.append(2);
    indexes.append(1);
    indexes.append(3);

    marker_arrayBuffer.create();
    marker_arrayBuffer.bind();
    marker_arrayBuffer.allocate(vertexes.constData(),vertexes.size()*sizeof(VertexData));
    marker_arrayBuffer.release();

    marker_indexBuffer.create();
    marker_indexBuffer.bind();
    marker_indexBuffer.allocate(indexes.constData(), indexes.size()*sizeof(GLuint));
    marker_indexBuffer.release();

//    qDebug()<<proj;
//    qDebug()<<view;
//    m_program.bind();
//    m_program.setUniformValue("projection",proj);
//    m_program.setUniformValue("modelview",view);
    m_program.setUniformValue("qt_Texture0",0);

    // procedo a dibujar la textura en el marker
    marker_texture->bind();

    marker_arrayBuffer.bind();

    int offset = 0;

    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc,GL_FLOAT,offset,3,sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texLoc =m_program.attributeLocation("qt_MultiTexCoord0");
    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc,GL_FLOAT,offset,2,sizeof(VertexData));

    marker_indexBuffer.bind();

    context()->functions()->glDrawElements(GL_TRIANGLES,marker_indexBuffer.size(),GL_UNSIGNED_INT,0);
    marker_texture->release();
}
