namespace IoTBackend.Core.Entities
{
    public class Device
    {
        public int Id { get; set; }
        public string MacAddress { get; set; } = string.Empty;
        public string Name { get; set; } = string.Empty;
        public bool IsActive { get; set; }
        public ICollection<TelemetryRecord> TelemetryRecords { get; set; }
    }
}