using IoTBackend.Core.Entities;
using IoTBackend.Core.Services;
using Microsoft.AspNetCore.Mvc;

namespace IoTBackend.API.Controllers;

[Route("api/[controller]")]
[ApiController]
public class HomeController : Controller
{
    private readonly IDeviceService _deviceService;

    public HomeController(IDeviceService deviceService)
    {
        _deviceService = deviceService;
    }

    [HttpGet("/")]
    [HttpGet("Home")]
    public IActionResult Index()
    {
        return View();
    }
    [HttpGet("api/devices")]    
    public async Task<IActionResult> GetActiveDevices()
    {
        var devices = await _deviceService.GetAllActiveDevicesAsync();
        return Ok(devices);
    }
}