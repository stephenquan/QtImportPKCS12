# QtImportPKCS12

This is a Qt application that demonstrates reading a PKCS#12 (.pfx or p12 file).
The program is designed to work on Windows, Linux, Android, macOS and iOS.

There `QSslCertificate::importPkcs12` is not implemented on macOS and iOS when the Qt kit
is built with Apple Secure Transport instead of OpenSSL.

See: https://bugreports.qt.io/browse/QTBUG-56596

This program demonstrates an implementation whereby we workaround the above limitation
by statically linking to OpenSSL to implement a custom `importPkcs12()` keeping the
Qt kit configured for Apple Secure Transport.

This is something any app can do without needing to patch the Qt kit.

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
