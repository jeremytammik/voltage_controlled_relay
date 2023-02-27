#include "btcontroller.h"

BluetoothData::BluetoothData()
{
    m_hasData = false;
}

BluetoothData::BluetoothData(String cmd, String data) : m_command(cmd), m_data(data)
{
    if (cmd != "" || data != "")
    {
        m_hasData = true;
    }
}

bool BluetoothData::available()
{
    return m_hasData;
}

String BluetoothData::getCommand()
{
    return m_command;
}

String BluetoothData::getData()
{
    return m_data;
}

String BluetoothData::payload()
{
    return (m_command + String(COMMAND_SEPARATOR) + m_data);
}

char BluetoothData::commandSeparator()
{
    return COMMAND_SEPARATOR;
}

char BluetoothData::payloadSeparator()
{
    return PAYLOAD_SEPARATOR;
}

//----------------------------------------------------------//
//                 BluetoothController                      //
//----------------------------------------------------------//

BluetoothController::BluetoothController()
{
}

bool BluetoothController::init()
{
    m_serialBT.begin("Voltage Controlled Relay"); // Bluetooth device name
    Serial.println("Bluetooth UP & RUNNING");
    return true;
}

BluetoothData BluetoothController::sync()
{
    readData = "";
    hasBtData = false;

    while (m_serialBT.available() > 0)
    {
        char c = (char)m_serialBT.read();

        if (c != '\n')
        {
            readData += c;
            hasBtData = true;
        }
    }

    if (hasBtData)
    {
        // qDebug(readData);

        cmd = _data = "";

        int ind1 = readData.indexOf('-');
        cmd = readData.substring(0, ind1);
        _data = readData.substring(ind1 + 1);

        BluetoothData btData(cmd, _data);

        hasBtData = false;

        return btData;
    }

    return BluetoothData();
}

bool BluetoothController::send(String cmd, String data)
{
    BluetoothData btdata(cmd, data);

    String msg = "\n" + btdata.payload() + "\n"; // Add new lines before and after the message
    char buf[msg.length() + 1];
    msg.toCharArray(buf, msg.length() + 1);

    for (int i = 0; i < msg.length(); i++)
    {
        m_serialBT.write((uint8_t)buf[i]);
    }

    return true;
}

bool BluetoothController::sendInfo(String data)
{
    return send("INFO", data);
}

bool BluetoothController::sendWarning(String data)
{
    return send("WARNING", data);
}
