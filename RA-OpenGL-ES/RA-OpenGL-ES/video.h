#ifndef VIDEO_H
#define VIDEO_H

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaPlayer>
#include <QGLWidget>
#include <QFile>
#include <QImage>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class Grabber : public QAbstractVideoSurface  {
    Q_OBJECT

public:
    GLuint textureId;
    QOpenGLTexture *texture;
    QOpenGLBuffer *videoBuffer;

    Grabber( QObject *parent = 0 ) : QAbstractVideoSurface( parent ) {
        videoBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
        videoBuffer->create();
        static const int coords[1][4][3] = { { { +1, -1, -999 }, { -1, -1, -999 }, { -1, +1, -999 }, { +1, +1, -999 } } };

        QVector<GLfloat> vertData;
        for (int i = 0; i < 1; ++i) {
            for (int j = 0; j < 4; ++j) {
                // vertex position
                vertData.append( coords[i][j][0]);
                vertData.append( coords[i][j][1]);
                vertData.append( coords[ i ][ j ][ 2 ]);

                // texture coordinate
                if ( j==0 )  {  vertData.append(1);  vertData.append(0);  }
                if ( j==1 )  {  vertData.append(0);  vertData.append(0);  }
                if ( j==2 )  {  vertData.append(0);  vertData.append(1);  }
                if ( j==3 )  {  vertData.append(1);  vertData.append(1);  }
            }
        }

        if ( ! videoBuffer->bind() )
            qDebug()<<"Error, bind() videoBuffer";

        videoBuffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );

        texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->setMinificationFilter( QOpenGLTexture::Nearest );
        texture->setMagnificationFilter( QOpenGLTexture::Linear );        
        texture->setFormat( QOpenGLTexture::RGBA8_UNorm );
        textureId = texture->textureId();        
    }

    QList< QVideoFrame::PixelFormat > supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle ) const
    {
        if( handleType == QAbstractVideoBuffer::NoHandle )  {             
             return QList< QVideoFrame::PixelFormat >() << QVideoFrame::Format_RGB32
                                                        << QVideoFrame::Format_ARGB32
                                                        << QVideoFrame::Format_ARGB32_Premultiplied
                                                        << QVideoFrame::Format_RGB32
                                                        << QVideoFrame::Format_RGB24
                                                        << QVideoFrame::Format_BGRA32
                                                        << QVideoFrame::Format_BGRA32_Premultiplied
                                                        << QVideoFrame::Format_BGR32
                                                        << QVideoFrame::Format_Jpeg
                                                        << QVideoFrame::Format_CameraRaw
                                                        << QVideoFrame::Format_AdobeDng;

        }
        else  {            
            return QList< QVideoFrame::PixelFormat >();
        }
    }

    bool start( const QVideoSurfaceFormat &format )  {
        const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat( format.pixelFormat() );
        const QSize size = format.frameSize();

        if ( imageFormat != QImage::Format_Invalid && !size.isEmpty() )  {
            QAbstractVideoSurface::start( format );            
            return true;
        }
        else  {
            return false;
        }
    }

    bool present( const QVideoFrame &frame )  {
        QVideoFrame copy(frame);
        if (copy.map(QAbstractVideoBuffer::ReadOnly)) {
            QImage::Format format = QVideoFrame::imageFormatFromPixelFormat(copy.pixelFormat());
            QImage img(copy.bits(), copy.width(), copy.height(), copy.bytesPerLine(), format);
            QImage rgb = img.convertToFormat(QImage::Format_RGB888);
            texture->setData(rgb);
            texture->bind();
        }
        else
        {
            return false;
        }
        return true;
    }
};

class Video : public QObject  {
    Q_OBJECT

public:
    QMediaPlayer *player;
    Grabber *grabber;
    QString name;
    int volume;

    Video( QString name, QObject *parent = 0 ) : QObject( parent ),
                                                 player( new QMediaPlayer( this ) ),
                                                 grabber( new Grabber( this ) ),
                                                 name( QFileInfo(name).fileName() ),
                                                 volume( 100 )
    {             
        QString videoUri = "../Videos/"+name;
        if( QFile::exists( videoUri ) )  {            
            qDebug()<<grabber->error();
            player->setVideoOutput( grabber );
            player->setMedia(QUrl::fromLocalFile("/home/andres/proyectosQT/OpenGLES/RA-OpenGL-ES/Videos/"+name));
        }
    }
};

#endif // VIDEO_H
