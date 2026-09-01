namespace IoTBackend.Core.Services
{
    public interface IUartService
    {
        Task SendUARTAsync(byte target_ID, byte payload_length, byte command, ReadOnlyMemory<byte> data);
    }
}