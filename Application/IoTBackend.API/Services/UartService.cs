using System.IO.Ports;
using System.Threading.Channels;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.DependencyInjection; 
using IoTBackend.Core.Services; 
namespace IoTBackend.API.Services;

public record SensorData(double Temperature, double Humidity);

public class UartService : BackgroundService
{
    private const string PORT_ADDRESS = "/dev/ttyACM0";
    private const int BAUDRATE = 9600;
    private readonly ILogger<UartService> _logger;
    private readonly Channel<byte[]> _dataChannel;
    
    private readonly IServiceProvider _serviceProvider; 

    public UartService(ILogger<UartService> logger, IServiceProvider serviceProvider)
    {
        _logger = logger;
        _serviceProvider = serviceProvider;
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
            while (buffer.Count >= 11)
            {
                int startIndex = buffer.IndexOf(0x55); 
                
                if (startIndex == -1)
                {
                    buffer.Clear();
                    break;
                }

                if (startIndex > 0)
                {
                    buffer.RemoveRange(0, startIndex);
                }

                if (buffer.Count < 11) 
                    break;

                if (buffer[10] == 0xAA)
                {
                    byte senderId = buffer[1];
                    byte targetId = buffer[2];
                    byte cmdType  = buffer[3];

                    if (targetId == 0xFF || targetId == 0x00) 
                    {
                        if (cmdType == 0x10) 
                        {
                            double humidity = buffer[5] + (buffer[6] / 10.0);
                            double temperature = buffer[7] + (buffer[8] / 10.0);

                            _logger.LogInformation("Oda {SenderId} -> Sıcaklık: {Temp}°C, Nem: %{Hum}", senderId, temperature, humidity);

                            try
                            {
                                using (var scope = _serviceProvider.CreateScope())
                                {
                                    var serviceManager = scope.ServiceProvider.GetRequiredService<IServiceManager>();
                                    
                                    await serviceManager.telemetryService.UpdateTelemetryValueAsync(senderId, temperature);
                                }
                            }
                            catch (Exception ex)
                            {
                                _logger.LogError(ex, "Telemetri ID {SenderId} güncellenirken veritabanı hatası oluştu.", senderId);
                            }
                        }
                    }

                    buffer.RemoveRange(0, 11);
                }
                else
                {
                    buffer.RemoveAt(0);
                }
            }
        }
    }
}