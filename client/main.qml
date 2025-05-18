import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import MultimeterClient 1.0

Window {
    title: "Multimeter Client"
    width: 600
    height: 400
    visible: true

    SocketClient {
        id: client
        onResponseReceived: {
            console.log("Response:", response);
            output.text += response + "\n";
         }
        onStatusChanged: statusText.text = status
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        RowLayout {
            TextField {
                id: socketPath
                Layout.fillWidth: true
                text: "/tmp/multimeter.sock"
                placeholderText: "Enter socket path"
            }

            Button {
                text: client.connected ? "Disconnect" : "Connect"
                onClicked: {
                    if(client.connected) {
                        client.disconnectFromServer()
                    } else {
                        client.connectToServer(socketPath.text)
                    }
                }
                background: Rectangle {
                    color: client.connected ? "#af4c4c" : "#639949"
                    radius: 4
                }
            }
        }

        RowLayout {
            Label {
                text: "Choose a channel:"
            }
            SpinBox {
                id: channelBox
                from: 0
                stepSize: 1
                value: client.channelNumber
                onValueModified: client.channelNumber = value
            }
        }

        RowLayout {
            spacing: 20
            ComboBox {
                Layout.preferredWidth: 150
                id: commandBox
                model: ["start_measure", "stop_measure", "get_result"]
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Send Command"
                onClicked: {
                    const cmd = `${commandBox.currentText} channel${channelBox.value}`
                    client.sendCommand(cmd)
                }
            }
        }

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: output
                font.family: "Verdana"
                color: "#44bd32"
                font.pixelSize: 16
                background: Rectangle{
                    color: "#2f3640"
                }
                readOnly: true
                wrapMode: Text.Wrap
                function scrollToBottom() {
                    if (scrollView.contentItem) {
                        scrollView.contentItem.contentY = output.height - scrollView.height;
                    }
                }
                onTextChanged: Qt.callLater(scrollToBottom);
            }
        }

        RowLayout {
            Rectangle {
                Layout.alignment: Qt.AlignRight
                width: 20
                height: 20
                radius: 10
                color: {
                    if (statusText.text.toLowerCase().includes("measure")) "green"
                    else if (statusText.text.toLowerCase().includes("error")) "red"
                    else "gray"
                }
            }

            Text {
                id: statusText
                text: ""
            }
        }
    }
}
