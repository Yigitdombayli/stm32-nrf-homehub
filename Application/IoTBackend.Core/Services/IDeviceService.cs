using IoTBackend.Core.Entities;

namespace IoTBackend.Core.Services;

public interface IDeviceService
{
    Task<IEnumerable<Device>> GetAllActiveDevicesAsync();
    Task<Device> AddDeviceAsync(Device device);
}