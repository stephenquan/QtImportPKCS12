#ifndef __import_pkcs12_patch__
#define __import_pkcs12_patch__

#include <QByteArray>
#include <QSslKey>
#include <QSslCertificate>
#include <QList>
#include <QIODevice>

#ifdef Q_OS_MACOS
#define IMPORTPKCS12_OPENSSL
#endif
#ifdef Q_OS_IOS
#define IMPORTPKCS12_OPENSSL
#endif

class ImportPkcs12Patch
{
public:
    static bool importPkcs12(QIODevice *device, QSslKey *key, QSslCertificate *certificate, QList<QSslCertificate> *caCertificates = nullptr, const QByteArray &passPhrase = QByteArray());
#ifdef IMPORTPKCS12_OPENSSL
    static bool importPkcs12_openssl(QIODevice *device, QSslKey *key, QSslCertificate *certificate, QList<QSslCertificate> *caCertificates = nullptr, const QByteArray &passPhrase = QByteArray());
#endif

};

#endif
