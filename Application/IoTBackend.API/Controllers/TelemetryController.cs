using IoTBackend.Core.Entities;
using IoTBackend.Core.Services;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Threading.Tasks;

namespace IoTBackend.API.Controllers
{
    public class TelemetryController : Controller
    {
        private readonly IServiceManager _manager;
        
        public TelemetryController(IServiceManager manager)
        {
            _manager = manager;
        }

        [HttpGet]
        public IActionResult Create(int deviceId)
        {
            ViewBag.DeviceId = deviceId;
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> Create(TelemetryRecord telemetry)
        {
            ModelState.Remove("Device");

            if (!ModelState.IsValid)
            {
                ViewBag.DeviceId = telemetry.DeviceId;
                return View(telemetry);
            }

            if (telemetry.Timestamp == default)
            {
                telemetry.Timestamp = DateTime.UtcNow;
            }

            await _manager.telemetryService.CreateTelemetryAsync(telemetry);
                    
            return RedirectToAction("Detail", "Device", new { id = telemetry.DeviceId });
        }
    }
}