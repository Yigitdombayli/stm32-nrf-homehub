using IoTBackend.Core.Entities;

namespace IoTBackend.Core.Repositories
{
    public interface ITelemetryRepository : IRepositoriesBase<TelemetryRecord>
    {
        Task CreateTelemetryAsync(TelemetryRecord telemetry);
        
        Task<IEnumerable<TelemetryRecord>> GetTelemetriesByDeviceIdAsync(int deviceId, bool trackChanges);
        Task UpdateTelemetryValueAsync(long id, double newValue);
    } 
}
