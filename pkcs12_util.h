#ifndef __pkcs12_util__
#define __pkcs12_util__

#include <QObject>
#include <QVariant>
#include <QUrl>
#include <QString>

class PKCS12Util : public QObject
{
    Q_OBJECT

public:
    PKCS12Util( QObject* parent = nullptr );

    Q_INVOKABLE QVariant importPKCS12( const QVariant& data, const QString& passPhrase );
    Q_INVOKABLE void loadFromUrl( const QUrl& url );

signals:
    void loaded( const QUrl& url, const QVariant& data );

protected:
    QByteArray loadFromFile( const QString& path ) const;

protected slots:
    void onFinished();

};

#endif
