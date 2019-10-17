#include "import_pkcs12_patch.h"
#include <QSslCertificate>

#ifdef IMPORTPKCS12_OPENSSL
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <QDebug>
#endif


bool ImportPkcs12Patch::importPkcs12(QIODevice *device, QSslKey *key, QSslCertificate *certificate, QList<QSslCertificate> *caCertificates, const QByteArray &passPhrase)
{
#ifdef IMPORTPKCS12_OPENSSL
    return importPkcs12_openssl(device, key, certificate, caCertificates, passPhrase );
#else
    return QSslCertificate::importPkcs12(device, key, certificate, caCertificates, passPhrase );
#endif
}

#ifdef IMPORTPKCS12_OPENSSL

template<class T>
class openssl_smart_ptr
{
public:
    openssl_smart_ptr( T* ptr = nullptr )
        : m_ptr( ptr )
    {
    }
    openssl_smart_ptr( T&& other)
        : m_ptr( nullptr )
    {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }
    ~openssl_smart_ptr()
    {
        clear();
    }
    void clear()
    {
        if ( m_ptr )
        {
            free( m_ptr );
            m_ptr = nullptr;
        }
    }
    void free( T* ptr )
    {
        Q_UNUSED( ptr )
    }
    openssl_smart_ptr& assign( T* ptr )
    {
        clear();
        m_ptr = ptr;
        return *this;
    }
    T* ptr()
    {
        return m_ptr;
    }
    operator T*()
    {
        return m_ptr;
    }
    T** operator&()
    {
        return &m_ptr;
    }
    openssl_smart_ptr& operator = ( T* ptr ) { return assign( ptr ); }
protected:
    T* m_ptr;
};

template <>
void openssl_smart_ptr<BIO>::free( BIO *ptr )
{
    BIO_free( ptr );
}

template <>
void openssl_smart_ptr<PKCS12>::free( PKCS12 *ptr )
{
    PKCS12_free( ptr );
}

template <>
void openssl_smart_ptr<EVP_PKEY>::free( EVP_PKEY *ptr )
{
    EVP_PKEY_free( ptr );
}

template <>
void openssl_smart_ptr<X509>::free( X509 *ptr )
{
    X509_free( ptr );
}

template <>
void openssl_smart_ptr<STACK_OF(X509)>::free( STACK_OF(X509) *ptr )
{
    sk_X509_pop_free( ptr, X509_free );
}

bool ImportPkcs12Patch::importPkcs12_openssl(QIODevice *device, QSslKey *key, QSslCertificate *certificate, QList<QSslCertificate> *caCertificates, const QByteArray &passPhrase)
{
    QByteArray p12Bytes = device->readAll();

    openssl_smart_ptr<BIO> p12Bio = BIO_new_mem_buf( p12Bytes.data(), p12Bytes.size() );
    if ( !p12Bio )
    {
        return false;
    }

    openssl_smart_ptr<PKCS12> p12 = d2i_PKCS12_bio( p12Bio, nullptr );
    if ( !p12  )
    {
        return false;
    }

    openssl_smart_ptr<EVP_PKEY> pkey;
    openssl_smart_ptr<X509> cert;
    openssl_smart_ptr<STACK_OF(X509)> ca;
    int ret = PKCS12_parse( p12, passPhrase.data(), &pkey, &cert, &ca );
    if ( ret == 0 )
    {
        return false;
    }

    openssl_smart_ptr<BIO> pkeyBio = BIO_new( BIO_s_mem() );
    ret = PEM_write_bio_PrivateKey( pkeyBio, pkey, nullptr, nullptr, 0, nullptr, nullptr );
    if ( ret == 0 )
    {
        return false;
    }

    int len = BIO_pending( pkeyBio );
    QByteArray pkeyBytes( len, 0 );
    ret = BIO_read( pkeyBio, pkeyBytes.data(), len );
    if ( ret == 0 )
    {
        return false;
    }

    *key = QSslKey( pkeyBytes, QSsl::Rsa );

    openssl_smart_ptr<BIO> certBio = BIO_new( BIO_s_mem() );
    ret = PEM_write_bio_X509( certBio, cert );
    if ( ret == 0 )
    {
        return false;
    }

    len = BIO_pending( certBio );
    QByteArray certBytes( len, 0 );
    ret = BIO_read( certBio, certBytes.data(), len );
    if ( ret == 0 )
    {
        return false;
    }

    *certificate = QSslCertificate( certBytes );

    if ( caCertificates )
    {
        caCertificates->clear();

        STACK_OF(X509)* _ca = ca;

        for ( int i = 0; i < sk_X509_num( _ca ); i++ )
        {
            X509* caCert = sk_X509_value( _ca, i );

            openssl_smart_ptr<BIO> caCertBio = BIO_new( BIO_s_mem() );
            ret = PEM_write_bio_X509( caCertBio, caCert );
            if ( ret == 0 )
            {
                continue;
            }

            len = BIO_pending( caCertBio );
            QByteArray caCertBytes( len, 0 );
            ret = BIO_read( caCertBio, caCertBytes.data(), len );
            if ( ret == 0 )
            {
                continue;
            }

            caCertificates->append( QSslCertificate( certBytes ) );
        }

    }

    return true;
}

#endif
