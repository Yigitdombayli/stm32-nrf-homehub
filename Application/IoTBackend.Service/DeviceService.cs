using IoTBackend.Core.Entities;
using IoTBackend.Core.Repositories;
using IoTBackend.Core.Services;

namespace IoTBackend.Service.Services;

public class DeviceService : IDeviceService
{
    private readonly IRepositoryManager _manager;
    public DeviceService(IRepositoryManager manager)
    {
        _manager = manager;
    }
    public Task<Device> AddDeviceAsync(Device device)
    {
        return _manager.DeviceRepository.AddDeviceAsync(device);
    }

    public Task<IEnumerable<Device>> GetAllActiveDevicesAsync()
    {
        return _manager.DeviceRepository.GetAllActiveDevicesAsync();
    }
}