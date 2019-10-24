#ifndef __import_pkcs12_openssl__
#define __import_pkcs12_openssl__

#include <QByteArray>
#include <QSslKey>
#include <QSslCertificate>
#include <QList>
#include <QIODevice>

bool importPkcs12_openssl(QIODevice *device, QSslKey *key, QSslCertificate *certificate, QList<QSslCertificate> *caCertificates = nullptr, const QByteArray &passPhrase = QByteArray());

#endif
