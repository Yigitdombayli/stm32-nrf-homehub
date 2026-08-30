using IoTBackend.Core.Entities;

namespace IoTBackend.Core.Repositories
{
    public interface IDeviceRepository : IRepositoriesBase<Device>
    {
        Task<IEnumerable<Device>> GetAllActiveDevicesAsync();
        Task<Device> AddDeviceAsync(Device device);
    }
}