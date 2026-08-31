using IoTBackend.Core.Entities;

namespace IoTBackend.Core.Services;

public interface ITelemetryService
{
         Task CreateTelemetryAsync(TelemetryRecord telemetry);
        
        Task<IEnumerable<TelemetryRecord>> GetTelemetriesByDeviceIdAsync(int deviceId, bool trackChanges);
        Task UpdateTelemetryValueAsync(long id, double newValue);
    
}