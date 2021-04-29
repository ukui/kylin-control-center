/*
    Copyright (C) 2012  Dan Vratil <dvratil@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

import QtQuick 2.1
import QtQuick.Controls 1.1 as Controls
import org.kde.kscreen 1.0

Item {
    id: root;

    property variant virtualScreen: null;

    objectName: "root";
    focus: true;

    SystemPalette {
        id: palette;
    }

    MouseArea {
        anchors.fill: parent;
        focus: true;

        Rectangle {
            id: background;

            anchors.fill: parent;
            focus: true;

            color: "transparent";

            FocusScope {

                id: outputViewFocusScope;

                anchors.fill: parent;
                focus: true;

                QMLScreen {
                    id: outputView;
                    anchors.fill: parent;
                    clip: true;
                    objectName: "outputView";
                }
            }

            Column {
                anchors {
                    left: parent.left;
                    bottom: parent.bottom;
                    margins: 5;
                }
                spacing: 5;
            }
        }

    }

}
