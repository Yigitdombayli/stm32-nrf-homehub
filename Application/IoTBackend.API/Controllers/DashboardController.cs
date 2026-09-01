using IoTBackend.Core.Services;
using Microsoft.AspNetCore.Mvc;

namespace IoTBackend.API.Controllers
{
    public class DashboardController : Controller
    {
        private readonly IServiceManager _manager;

        public DashboardController(IServiceManager manager)
        {
            _manager = manager;
        }

        public async Task<IActionResult> Index()
        {
            var devices = await _manager.deviceService.GetAllActiveDevicesAsync();
            
            return View(devices);
        }

        [HttpGet]
        public async Task<IActionResult> GetDeviceTelemetries(int deviceId)
        {
           var telemetries = await _manager.telemetryService.GetTelemetriesByDeviceIdAsync(deviceId, trackChanges: false);

            var result = telemetries.Select(t => new 
            {
                id = t.Id,
                value = t.Value
            }).ToList();

            return Json(result);
        }
    }
}