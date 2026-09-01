using IoTBackend.Core.Entities;
using IoTBackend.Core.Repositories;
using Microsoft.EntityFrameworkCore; 

namespace IoTBackend.Repository
{
    public class DeviceRepository : RepositoryBase<Device>, IDeviceRepository
    {
        public DeviceRepository(AppDbContext context) : base(context)
        {
        }

        public async Task<Device> AddDeviceAsync(Device device)
        {
            Create(device);
            await _context.SaveChangesAsync();
            return device; 
        }


        public async Task<IEnumerable<Device>> GetAllActiveDevicesAsync()
        {
            var activeDevices = await FindAll(trackChanges: false)
                                    .Where(d => d.IsActive)
                                    .Include(d => d.TelemetryRecords.OrderByDescending(t => t.Timestamp).Take(5)) 
                                    .ToListAsync();
            return activeDevices;
        }
    }
}