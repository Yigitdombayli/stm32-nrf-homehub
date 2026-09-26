const toggleButtons = document.querySelectorAll('.toggle-btn');

async function updateAllDevicesTelemetry() {
    let activeDeviceIds = [...new Set(Array.from(toggleButtons).map(btn => btn.getAttribute('data-room')))];

    for (let dId of activeDeviceIds) {
        try {
            const response = await fetch(`/Dashboard/GetDeviceTelemetries?deviceId=${dId}`);
            if (response.ok) {
                const telemetries = await response.json();
                
                telemetries.forEach(tel => {
                    const degerElementi = document.getElementById(`val-${tel.id}`);
                    if (degerElementi) {
                        degerElementi.innerText = tel.value;
                    }
                });
            }
        } catch (error) {
            console.error(`Device ${dId} fetch error:`, error);
        }
    }
}

toggleButtons.forEach(button => {
    button.addEventListener('click', async function () {
        let isCurrentlyOn = this.classList.contains('btn-success');
        let willBeOn = !isCurrentlyOn; 
        
        let roomId = this.getAttribute('data-room'); 
        let ledId = this.getAttribute('data-led'); 
        let ledNumber = parseInt(ledId) + 1; 

        if (willBeOn) {
            this.classList.replace('btn-secondary', 'btn-success');
            this.innerText = `LED ${ledNumber} (AÇIK)`;
        } else {
            this.classList.replace('btn-success', 'btn-secondary');
            this.innerText = `LED ${ledNumber} (KAPALI)`;
        }

        try {
            let response = await fetch(`/api/DeviceControl/toggle-light?roomId=${roomId}&ledId=${ledId}&turnOn=${willBeOn}`, {
                method: 'POST'
            });

            if (!response.ok) {
                console.error("Komut gönderilemedi! Sunucu hatası.");
                this.classList.replace(willBeOn ? 'btn-success' : 'btn-secondary', isCurrentlyOn ? 'btn-success' : 'btn-secondary');
                this.innerText = `LED ${ledNumber} (${isCurrentlyOn ? 'AÇIK' : 'KAPALI'})`;
            }
        } catch (err) {
            console.error("Ağ hatası:", err);
            this.classList.replace(willBeOn ? 'btn-success' : 'btn-secondary', isCurrentlyOn ? 'btn-success' : 'btn-secondary');
            this.innerText = `LED ${ledNumber} (${isCurrentlyOn ? 'AÇIK' : 'KAPALI'})`;
        }
    });
});

updateAllDevicesTelemetry();
setInterval(updateAllDevicesTelemetry, 3000);