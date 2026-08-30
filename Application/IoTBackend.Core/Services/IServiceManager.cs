namespace IoTBackend.Core.Services
{
    public interface IServiceManager
    {
        ITelemetryService telemetryService {get;}
        IDeviceService deviceService {get;}
    }
}