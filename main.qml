import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.2
import PKCS12Util 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    property url loaded_url
    property var loaded_pkcs12
    property string privateKey: ""
    property string certificate: ""
    property var caCertificates: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: qsTr( "PKCS#12 file (*.pfx, *.p12)" )
            font.pointSize: 12
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true

            TextField {
                id: fileTextField

                Layout.fillWidth: true

                font.pointSize: 12
                selectByMouse: true

                onTextChanged: pkcs12Util.loadFromUrl( text )
            }

            Button {
                Layout.preferredWidth: height

                text: "..."
                font.pointSize: 12

                onClicked: fileDialog.open()
            }
        }

        Text {
            property bool isLoaded: loaded_pkcs12 !== null && loaded_pkcs12 !== undefined
            text: isLoaded ? qsTr( "PKCS#12 Loaded" ) : qsTr( "PKCS#12 Not Loaded" )
            font.pointSize: 12
            color: isLoaded ? "green" : "red"
        }

        Item {
            Layout.preferredHeight: 20
        }

        Text {
            text: qsTr( "passphrase")
            font.pointSize: 12
            font.bold: true
        }

        TextField {
            id: passPhraseTextField

            Layout.fillWidth: true

            font.pointSize: 12
            selectByMouse: true
            echoMode: TextInput.Password

            onTextChanged: runImportPKCS12()
        }

        Item {
            Layout.preferredHeight: 20
        }

        Text {
            text: qsTr( "Results" )
            font.pointSize: 12
            font.bold: true
        }

        Flickable {
            id: flickable

            Layout.fillWidth: true
            Layout.fillHeight: true

            contentWidth: columnLayout.width
            contentHeight: columnLayout.height
            clip: true

            ScrollBar.vertical: ScrollBar { }

            ColumnLayout {
                id: columnLayout

                width: flickable.width

                Text {
                    property bool privateKeyValid: privateKey !== ""
                    text: privateKeyValid ? qsTr( "Private Key is valid" ) : qsTr( "Private Key is not valid" )
                    font.pointSize: 12
                    color: privateKeyValid ? "green" : "red"
                }

                Text {
                    property bool certificateValid: certificate !== ""
                    text: certificateValid ? qsTr( "Certificate is valid" ) : qsTr( "Certificate is not valid" )
                    font.pointSize: 12
                    color: certificateValid ? "green" : "red"
                }

                Text {
                    text: qsTr( "CA certificates: %1" ).arg( caCertificates ? caCertificates.length : 0 )
                    font.pointSize: 12
                    color: "green"
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        onAccepted: fileTextField.text = fileUrl
    }

    PKCS12Util {
        id: pkcs12Util

        onLoaded: {
            loaded_url = url;
            loaded_pkcs12 = data;
            runImportPKCS12();
        }
    }

    Component.onCompleted: Qt.callLater( runImportPKCS12 )

    function runImportPKCS12() {
        privateKey = "";
        certificate = "";
        caCertificates = [ ];

        const obj = pkcs12Util.importPKCS12( loaded_pkcs12, passPhraseTextField.text );
        if ( !obj )
        {
            return;
        }

        privateKey = obj[ "privateKey" ] || "";
        certificate = obj[ "certificate" ] || "";
        caCertificates = obj[ "caCertificates" ] || [ ];
    }

}
