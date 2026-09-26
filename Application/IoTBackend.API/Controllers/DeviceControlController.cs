using Microsoft.AspNetCore.Mvc;
using IoTBackend.API.Services; // IUartService'in bulunduğu namespace
using IoTBackend.Core.Constants;
using System.Threading.Tasks;
using IoTBackend.Core.Services;

namespace IoTBackend.API.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class DeviceControlController : ControllerBase
    {
        private readonly IUartService _uartService;

        public DeviceControlController(IUartService uartService)
        {
            _uartService = uartService;
        }

        [HttpPost("toggle-light")]
        public async Task<IActionResult> ToggleLight([FromQuery] byte roomId, [FromQuery] byte ledId, [FromQuery] bool turnOn)
        {
            byte cmdType = SystemConstants.CMD_RELAY_CTRL; 
            
            byte state = turnOn ? (byte)1 : (byte)0;
            
            byte[] payload = new byte[] { ledId, state, 0x00, 0x00, 0x00 };
            
            await _uartService.SendUARTAsync(roomId, 1, cmdType, payload);
            
            return Ok(new { success = true, message = $"Oda {roomId} LED {ledId + 1} durumu değiştirildi." });
        }
    }
}