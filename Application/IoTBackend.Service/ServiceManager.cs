using IoTBackend.Core.Services;

namespace IoTBackend.Service
{
    public class ServiceManager : IServiceManager
    {
        private readonly ITelemetryService _telemetryService;
        private readonly IDeviceService _deviceService;
        private readonly IUartService _uartService;
        public ServiceManager(ITelemetryService telemetryService, IDeviceService deviceService, IUartService uartService)
        {
            _uartService = uartService;
            _telemetryService = telemetryService;
            _deviceService = deviceService;
        }
        public ITelemetryService telemetryService => _telemetryService;
        public IDeviceService deviceService => _deviceService;

        public IUartService uartService => _uartService;
    }
}