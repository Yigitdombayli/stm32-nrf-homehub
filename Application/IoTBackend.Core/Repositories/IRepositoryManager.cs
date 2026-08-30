namespace IoTBackend.Core.Repositories
{
    public interface IRepositoryManager 
    {
         IDeviceRepository DeviceRepository { get; }
         ITelemetryRepository telemetryRepository { get; }
    }
}