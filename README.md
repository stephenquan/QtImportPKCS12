# QtImportPKCS12

This is a Qt application that demonstrates reading a PKCS#12 (.pfx or p12 file).
The program is designed to work on Windows, Linux, Android, macOS and iOS.

On Windows, Linux and Android this uses `QSslCertificate::importPkcs12` to load the PKCS#12 file.

On macOS and iOS because we cannot use `QSslCertfiicate::importPkcs12` (see https://devtopia.esri.com/Melbourne/appstudio-factory/issues/302), we will statically link to OpenSSL and implement a custom `importPkcs12()`.

This solutions means we do not need to rebuild the Qt Kit for macOS and iOS from source code. We can keep using the stock Qt Kit configured for Apple Secure Transport.

## Building

This Qt application was developed and tested with Qt 5.13.1 and OpenSSL 1.1.1d.
For macOS and iOS, you will need to build OpenSSL from source code.
Consult this repo for building the OpenSSL prerequisites:

 - https://github.com/stephenquan/build_openssl
  
For all other platforms, OpenSSL is part of the default Qt kits.

## Running the Qt Application

To run the application, you will need access to a PKCS#12 file (usually a .pfx or a .p12)
that contains a private key, certificates, and, optionally, CA certificates.

You may need to specify a passphrase to unlock the PKCS#12 file.

When unlocked, the application display whether it has detected the private key,
certificate and CA certificates.

The debbugging console will show any issues.
