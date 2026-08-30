using IoTBackend.Core.Services;

namespace IoTBackend.Service
{
    public class ServiceManager : IServiceManager
    {
        private readonly ITelemetryService _telemetryService;
        private readonly IDeviceService _deviceService;
        public ServiceManager(ITelemetryService telemetryService, IDeviceService deviceService)
        {
            _telemetryService = telemetryService;
            _deviceService = deviceService;
        }
        public ITelemetryService telemetryService => _telemetryService;
        public IDeviceService deviceService => _deviceService;
    }
}