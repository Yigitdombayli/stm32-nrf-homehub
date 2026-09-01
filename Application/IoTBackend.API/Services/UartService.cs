using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Channels;
using System.Threading.Tasks;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using IoTBackend.Core.Constants;
using IoTBackend.Core.Services;

namespace IoTBackend.API.Services
{
    public class UartService : BackgroundService, IUartService
    {
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public unsafe struct TelemetryFrame
        {
            public byte startByte;  
            public byte Sender_ID;
            public byte Target_ID;
            public byte Cmd_Type;
            public byte Payload_Lenght;
            public fixed byte data[5];  
            public byte stopByte; 
        }

        private readonly ILogger<UartService> _logger;
        private readonly IServiceProvider _serviceProvider;
        private readonly Channel<byte[]> _dataChannel;
        
        private SerialPort _serialPort; 

        public UartService(ILogger<UartService> logger, IServiceProvider serviceProvider)
        {
            _logger = logger;
            _serviceProvider = serviceProvider;
            _dataChannel = Channel.CreateBounded<byte[]>(new BoundedChannelOptions(1000)
            {
                FullMode = BoundedChannelFullMode.DropOldest
            });
        }

        public override Task StartAsync(CancellationToken cancellationToken)
        {
            _serialPort = new SerialPort(SystemConstants.PORT_ADDRESS, SystemConstants.BAUDRATE);
            try
            {
                _serialPort.Open();
                _logger.LogInformation("UART Portu başarıyla açıldı: {Port}", SystemConstants.PORT_ADDRESS);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "UART Portu açılamadı! Cihaz bağlı mı?");
            }
            
            return base.StartAsync(cancellationToken);
        }

        public override Task StopAsync(CancellationToken cancellationToken)
        {
            if (_serialPort != null && _serialPort.IsOpen)
            {
                _serialPort.Close();
                _serialPort.Dispose();
            }
            return base.StopAsync(cancellationToken);
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
                _logger.LogError(ex, "UART ExecuteAsync hatası.");
            }
        }

        private async Task ReadFromSerialPortAsync(CancellationToken stoppingToken)
        {
            byte[] buffer = new byte[1024];
            while (!stoppingToken.IsCancellationRequested)
            {
                if (_serialPort != null && _serialPort.IsOpen)
                {
                    int bytesRead = await _serialPort.BaseStream.ReadAsync(buffer, 0, buffer.Length, stoppingToken);
                    if (bytesRead > 0)
                    {
                        byte[] dataFrame = buffer.Take(bytesRead).ToArray();
                        await _dataChannel.Writer.WriteAsync(dataFrame, stoppingToken);
                    }
                }
                else
                {
                    await Task.Delay(1000, stoppingToken); 
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
                    int startIndex = buffer.IndexOf(SystemConstants.START_BYTE); 
                    
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

                    if (buffer[10] == SystemConstants.STOP_BYTE)
                    {
                        byte senderId = buffer[1];
                        byte targetId = buffer[2];
                        byte cmdType  = buffer[3];

                        if (targetId == SystemConstants.ID_GATEWAY || targetId == SystemConstants.ID_PC) 
                        {
                            if (cmdType == SystemConstants.CMD_DHT_11_TEMPERATURE) 
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
                                    _logger.LogError(ex, "Telemetri güncellenirken hata oluştu.");
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

        public async Task SendUARTAsync(byte target_ID, byte payload_length, byte command, ReadOnlyMemory<byte> data)
        {
            if (data.Length != 5)
                throw new ArgumentException("Data parametresi tam olarak 5 byte olmalıdır!", nameof(data));

            if (_serialPort == null || !_serialPort.IsOpen)
            {
                _logger.LogWarning("Seri Port kapalı, veri gönderilemiyor.");
                return;
            }
            
            TelemetryFrame telemetryData = new TelemetryFrame();
            telemetryData.startByte = SystemConstants.START_BYTE;
            telemetryData.Sender_ID = SystemConstants.ID_PC; 
            telemetryData.Target_ID = target_ID;
            telemetryData.Cmd_Type = command;
            telemetryData.Payload_Lenght = payload_length;
            telemetryData.stopByte = SystemConstants.STOP_BYTE;

            unsafe
            {
                for (int i = 0; i < 5; i++)
                {
                    telemetryData.data[i] = data.Span[i];
                }
            }

            byte[] frameBytes = StructToByteArray(telemetryData);
            
            await _serialPort.BaseStream.WriteAsync(frameBytes, 0, frameBytes.Length);
            await _serialPort.BaseStream.FlushAsync();
        }

        private unsafe byte[] StructToByteArray(TelemetryFrame frame)
        {
            int size = sizeof(TelemetryFrame);
            byte[] arr = new byte[size];
            fixed (byte* ptr = arr)
            {
                *(TelemetryFrame*)ptr = frame;
            }
            return arr;
        }
    }
}