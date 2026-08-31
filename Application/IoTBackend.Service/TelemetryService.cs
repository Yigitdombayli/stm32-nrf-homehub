using IoTBackend.Core.Entities;
using IoTBackend.Core.Repositories;
using IoTBackend.Core.Services;

namespace IoTBackend.Service.Services;

public class TelemetryService : ITelemetryService
{
    private readonly IRepositoryManager _manager;
    
    public TelemetryService(IRepositoryManager manager)
    {
        _manager = manager;
    }

    public Task CreateTelemetryAsync(TelemetryRecord telemetry)
    {
        return _manager.telemetryRepository.CreateTelemetryAsync(telemetry);
    }

    public Task<IEnumerable<TelemetryRecord>> GetTelemetriesByDeviceIdAsync(int deviceId, bool trackChanges)
    {
        return _manager.telemetryRepository.GetTelemetriesByDeviceIdAsync(deviceId, trackChanges);
    }

    public async Task UpdateTelemetryValueAsync(long id, double newValue)
    {
        await _manager.telemetryRepository.UpdateTelemetryValueAsync(id,newValue);
    }
}