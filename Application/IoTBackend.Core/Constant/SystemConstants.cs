namespace IoTBackend.Core.Constants;
public static class SystemConstants
{
    public const byte START_BYTE = 0x55;
    public const byte STOP_BYTE = 0xAA;
    
    public const byte ID_PC = 0x00;
    public const byte ID_GATEWAY = 0xFF;
    public const byte ID_ROOM_1 = 0x01;
    public const byte ID_ROOM_2 = 0x02;
    public const byte MY_NODE_ID = ID_PC;
    
    public const byte CMD_DHT_11_TEMPERATURE = 0x10;
    public const byte CMD_RELAY_CTRL = 0x20;

    public const string PORT_ADDRESS = "/dev/ttyACM0";
    public const int BAUDRATE = 9600;
}