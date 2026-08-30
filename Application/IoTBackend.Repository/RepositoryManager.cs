using IoTBackend.Core.Repositories;

namespace IoTBackend.Repository
{
    public class RepositoryManager : IRepositoryManager
    {
        private readonly AppDbContext _context;
        private readonly ITelemetryRepository _telemetryRepository;
        private readonly IDeviceRepository _deviceRepository;
        public RepositoryManager(AppDbContext context, ITelemetryRepository telemetryRepository,
                                IDeviceRepository deviceRepository)
        {
            _context = context;
            _deviceRepository = deviceRepository;
            _telemetryRepository = telemetryRepository;
        }
        public IDeviceRepository DeviceRepository => _deviceRepository;

        public ITelemetryRepository telemetryRepository => _telemetryRepository;
    }
}