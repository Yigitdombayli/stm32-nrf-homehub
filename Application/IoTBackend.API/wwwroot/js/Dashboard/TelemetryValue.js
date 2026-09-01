const deviceId = 1; 

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
setInterval(getData, 3000);