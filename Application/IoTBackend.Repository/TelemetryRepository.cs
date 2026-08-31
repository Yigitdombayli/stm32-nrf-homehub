using IoTBackend.Core.Entities;
using IoTBackend.Core.Repositories;
using Microsoft.EntityFrameworkCore;

namespace IoTBackend.Repository
{
    public class TelemetryRepository : RepositoryBase<TelemetryRecord>, ITelemetryRepository
    {
        public TelemetryRepository(AppDbContext context) : base(context)
        {
            
        }

        public async Task CreateTelemetryAsync(TelemetryRecord telemetry)
        {
            Create(telemetry);
            
            await _context.SaveChangesAsync();
        }

        public async Task<IEnumerable<TelemetryRecord>> GetTelemetriesByDeviceIdAsync(int deviceId, bool trackChanges)
        {
            var telemetries = await FindAll(trackChanges)
                                    .Where(t => t.DeviceId == deviceId)
                                    .OrderByDescending(t => t.Timestamp) 
                                    .ToListAsync();
                                    
            return telemetries;
        }
        public async Task UpdateTelemetryValueAsync(long id, double newValue)
        {
            var telemetry = await _context.Telemetries.FindAsync(id);
            
            if (telemetry != null)
            {
                telemetry.Value = newValue;
                telemetry.Timestamp = DateTime.UtcNow; 
                _context.Telemetries.Update(telemetry);
                await _context.SaveChangesAsync(); 
            }
        }
    }
}