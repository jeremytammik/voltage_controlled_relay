#ifndef BTCONTROLLER_H
#define BTCONTROLLER_H

#include <BluetoothSerial.h>
#include "Arduino.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

const char COMMAND_SEPARATOR = '|'; // Value separator in the data
const char PAYLOAD_SEPARATOR = ':'; // Payload data separator

/*
    BluetoothData data2send;
    data2send.command = "GET_CURRENT_TIME";
    data2send.data = "utc"
*/
class BluetoothData
{
public:
    BluetoothData();

    BluetoothData(String cmd, String data);

    bool available();

    String getCommand();

    String getData();

    String payload();

    char commandSeparator();

    char payloadSeparator();

private:
    String m_command = ""; // Command to sent
    String m_data = "";    // Payload to be
    bool m_hasData = false;
};

class BluetoothController
{
public:
    BluetoothController();

    bool init();

    BluetoothData sync();

    bool send(String cmd, String data);

    bool sendInfo(String data);

    bool sendWarning(String data);

private:
    BluetoothSerial m_serialBT;
    String readData = "";
    bool hasBtData = false;
    String cmd = "", _data = "";

    BluetoothData m_btData;
};

#endif // BTCONTROLLER_H