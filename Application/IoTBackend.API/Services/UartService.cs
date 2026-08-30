using System.IO.Ports;
using System.Threading.Channels;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace IoTBackend.API.Services;

public record SensorData(double Temperature, double Humidity);

public class UartService : BackgroundService
{
    private const string PORT_ADDRESS = "/dev/ttyACM0";
    private const int BAUDRATE = 9600;
    private readonly ILogger<UartService> _logger;
    private readonly Channel<byte[]> _dataChannel;

    public UartService(ILogger<UartService> logger)
    {
        _logger = logger;
        _dataChannel = Channel.CreateBounded<byte[]>(new BoundedChannelOptions(1000)
        {
            FullMode = BoundedChannelFullMode.DropOldest
        });
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        try
        {
            var processorTask = ProcessDataAsync(stoppingToken);
            var readerTask = Task.Run(() => ReadFromSerialPortAsync(stoppingToken), stoppingToken);
            await Task.WhenAll(readerTask, processorTask);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "UART hatasi.");
        }
    }

    private async Task ReadFromSerialPortAsync(CancellationToken stoppingToken)
    {
    using var serialPort = new SerialPort(PORT_ADDRESS, BAUDRATE);
    serialPort.Open();

    byte[] buffer = new byte[1024];

    while (!stoppingToken.IsCancellationRequested)
    {
        int bytesRead = await serialPort.BaseStream.ReadAsync(buffer, 0, buffer.Length, stoppingToken);

        if (bytesRead > 0)
        {
            byte[] dataFrame = buffer.Take(bytesRead).ToArray();
            await _dataChannel.Writer.WriteAsync(dataFrame, stoppingToken);
        }
    }
    }

   private async Task ProcessDataAsync(CancellationToken stoppingToken)
{
    List<byte> buffer = new List<byte>();

    await foreach (var rawBytes in _dataChannel.Reader.ReadAllAsync(stoppingToken))
    {
        buffer.AddRange(rawBytes);

        while (buffer.Count >= 7)
        {
            int startIndex = buffer.IndexOf(0xAA);
            if (startIndex == -1)
            {
                buffer.Clear();
                break;
            }

            if (startIndex > 0)
            {
                buffer.RemoveRange(0, startIndex);
            }

            if (buffer.Count < 7) 
                break;

            if (buffer[6] == 0x55)
            {
                double humidity = buffer[1];
                double temperature = buffer[3];

                var data = new SensorData(temperature, humidity);

                buffer.RemoveRange(0, 7);
            }
            else
            {
                buffer.RemoveAt(0);
            }
        }
    }
}
}