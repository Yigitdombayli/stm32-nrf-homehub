using IoTBackend.Core.Entities;
using IoTBackend.Core.Services;
using Microsoft.AspNetCore.Mvc;

namespace IoTBackend.API.Controllers
{
    public class DeviceController : Controller
    {
        private readonly IDeviceService _deviceService;
        private readonly IServiceManager _manager;

        public DeviceController(IDeviceService deviceService, IServiceManager manager)
        {
            _deviceService = deviceService;
            _manager = manager;
        }

        public async Task<IActionResult> Index()
        {
            var devices = await _deviceService.GetAllActiveDevicesAsync();
            return View(devices);
        }

        [HttpGet]
        public IActionResult Create()
        {
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> Create(Device device)
        {
                device.IsActive = true; 
                
                await _deviceService.AddDeviceAsync(device);
                
                return RedirectToAction(nameof(Index));
        }

        public async Task<IActionResult> Detail(int id)
        {
            var telemetries =await _manager.telemetryService.GetTelemetriesByDeviceIdAsync(id,false);
            ViewBag.DeviceId = id;
            return View(telemetries);
        }
    }
}