using IoTBackend.Core.Entities;
using Microsoft.EntityFrameworkCore;

namespace IoTBackend.Repository;

public class AppDbContext : DbContext
{
    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    public DbSet<Device> Devices { get; set; }
    public DbSet<TelemetryRecord> Telemetries {get; set;}
}