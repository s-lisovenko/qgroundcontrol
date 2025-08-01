import QtQuick
import QtQuick.Controls

import QGroundControl


import QGroundControl.Controls
import QGroundControl.ScreenTools

Flow {
    spacing: ScreenTools.defaultFontPixelWidth

    /// true: Checking the first entry will clear and disable all other entries
    property bool firstEntryIsAll: false

    property Fact fact: Fact { }

    Component.onCompleted: {
        if (firstEntryIsAll && repeater.itemAt(0).checked) {
            for (var i=1; i<repeater.count; i++) {
                var otherCheckbox = repeater.itemAt(i)
                otherCheckbox.enabled = false
            }
        }
    }

    Repeater {
        id:     repeater
        model:  fact ? fact.bitmaskStrings : []

        QGCCheckBox {
            id:         checkbox
            text:       modelData
            checked:    fact.value & fact.bitmaskValues[index]

            onClicked: {
                var i;
                var otherCheckbox;
                if (checked) {
                    if (firstEntryIsAll && index == 0) {
                        for (i = 1; i < repeater.count; i++) {
                            otherCheckbox = repeater.itemAt(i)
                            fact.value &= ~fact.bitmaskValues[i]
                            otherCheckbox.checked = false
                            otherCheckbox.enabled = false
                        }
                    }
                    fact.value |= fact.bitmaskValues[index]
                } else {
                    if (firstEntryIsAll && index == 0) {
                        for (i = 1; i < repeater.count; i++) {
                            otherCheckbox = repeater.itemAt(i)
                            otherCheckbox.enabled = true
                        }
                    }
                    fact.value &= ~fact.bitmaskValues[index]
                }
            }
        }
    }
}
