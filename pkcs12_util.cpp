#include "pkcs12_util.h"
#include "import_pkcs12_patch.h"

#include <QBuffer>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QQmlEngine>
#include <QEventLoop>
#include <QNetworkReply>
#include <QCryptographicHash>

PKCS12Util::PKCS12Util( QObject* parent ) :
    QObject( parent )
{
}

QByteArray PKCS12Util::loadFromFile( const QString& path ) const
{
    QFile file( path );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return QByteArray();
    }
    return file.readAll();
}

void PKCS12Util::loadFromUrl( const QUrl& url )
{
    if ( !url.isValid() )
    {
        emit loaded( url, QByteArray() );
        return;
    }

    if ( url.isLocalFile() )
    {
        emit loaded( url, loadFromFile( url.toLocalFile()  ) );
        return;
    }

    QNetworkAccessManager* mgr = qmlEngine(this)->networkAccessManager();
    QNetworkRequest req( url );
    QNetworkReply* reply = mgr->get( req );
    QEventLoop loop;
    QObject::connect( reply, &QNetworkReply::finished, this, &PKCS12Util::onFinished );
    /*
    loop.exec();
    if ( reply->error() != QNetworkReply::NoError ) return QByteArray();
    if ( reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ) != 200 ) return QByteArray();
    QByteArray data = reply->readAll();
    return data;
    */
}

QVariant PKCS12Util::importPKCS12( const QVariant& data, const QString& passPhrase )
{
    //qDebug() << Q_FUNC_INFO << __LINE__ << data << passPhrase;
    //qDebug() << Q_FUNC_INFO << data.type();
    //qDebug() << Q_FUNC_INFO << static_cast<int>( data.type() );

    //qDebug() << Q_FUNC_INFO << __LINE__ << url << passPhrase;

    if ( !data.isValid() || data.isNull() )
    {
        return QVariant();
    }

    if ( data.type() != QVariant::ByteArray )
    {
        qDebug() << Q_FUNC_INFO << " line: " << __LINE__ << data.type() << " unexpected.";
        return QVariant();
    }

    QByteArray bytes = data.toByteArray();

    QBuffer buffer;
    buffer.open( QIODevice::ReadWrite );
    buffer.write( bytes );
    buffer.reset();

    QVariantMap result;

    QSslKey privateKey;
    QSslCertificate certificate;
    QList<QSslCertificate> caCertificates;

    bool ok = ImportPkcs12Patch::importPkcs12( &buffer, &privateKey, &certificate, &caCertificates, passPhrase.toUtf8() );
    if ( !ok )
    {
        return QVariant();
    }

    result[ "privateKey" ] = QString::fromUtf8( privateKey.toPem() );
    result[ "certificate" ] = QString::fromUtf8( certificate.toPem() );
    QStringList _ca;
    foreach ( QSslCertificate caCert, caCertificates )
    {
        _ca.append( caCert.toPem() );
    }
    result[ "caCertificates" ] = _ca;

    return result;
}

void PKCS12Util::onFinished()
{
    qDebug() << Q_FUNC_INFO << QObject::sender();

    QNetworkReply* reply = ::qobject_cast<QNetworkReply*>( QObject::sender() );
    if ( !reply )
    {
        return;
    }

    QUrl url = reply->url();
    QNetworkReply::NetworkError error = reply->error();
    if ( error != QNetworkReply::NoError )
    {
        qDebug() << Q_FUNC_INFO << __LINE__ << url << "error: " << error;
        emit loaded( url, QVariant() );
        return;
    }

    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    if ( statusCode != 200 )
    {
        qDebug() << Q_FUNC_INFO << __LINE__ << url << "statusCode: " << statusCode;
        emit loaded( url, QVariant() );
        return;
    }

    QString contentType = reply->header( QNetworkRequest::ContentTypeHeader ).toString();
    qDebug() << Q_FUNC_INFO << __LINE__ << "contentType: " << contentType;

    QByteArray data = reply->readAll();
    if ( data.isNull() || data.isEmpty() )
    {
        qDebug() << Q_FUNC_INFO << __LINE__ << url << "empty";
        emit loaded( url, QVariant() );
        return;
    }

    emit loaded( url, data );
}
