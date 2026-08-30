namespace IoTBackend.Core.Entities
{
    public class TelemetryRecord
    {
        public long Id { get; set; } 
        public int DeviceId { get; set; } 
        
        public string MeasurementType { get; set; }
        public double Value { get; set; } 
        public DateTime Timestamp { get; set; } = DateTime.UtcNow; 
        
        public Device Device { get; set; }
    }
}