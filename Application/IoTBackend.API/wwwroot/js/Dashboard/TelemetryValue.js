const deviceId = 1; 
const toggleButtons = document.querySelectorAll('[data-toggle="button"]');
async function getData() {
    const url = `/Dashboard/GetDeviceTelemetries?deviceId=${deviceId}`;
    
    try {
        const response = await fetch(url);
        if (response.ok) {
            const telemetries = await response.json();
            
                 telemetries.forEach(tel => {
                const degerElementi = document.getElementById(`val-${tel.id}`);
                
                if (degerElementi) {
                    degerElementi.innerText = tel.value;
                    }});
        }
        } catch (error) {
            console.error('Error:', error);
        } 
}
getData();


toggleButtons.forEach(button => {
    button.addEventListener('click', async function () {
        setTimeout(async () => {
            let isActive = this.getAttribute('aria-pressed') === 'true';
            let roomId = this.getAttribute('data-room');

            if (isActive) {
                this.classList.replace('btn-secondary', 'btn-success');
                this.innerText = `Oda ${roomId} Işık (AÇIK)`;
            } else {
                this.classList.replace('btn-success', 'btn-secondary');
                this.innerText = `Oda ${roomId} Işık (KAPALI)`;
            }

            try {
                let response = await fetch(`/api/DeviceControl/toggle-light?roomId=${roomId}&turnOn=${isActive}`, {
                    method: 'POST'
                });

                if (!response.ok) {
                    console.error("Komut gönderilemedi!");
                }
            } catch (err) {
                console.error("Ağ hatası:", err);
            }
        }, 50);
    });
});


setInterval(getData, 3000);