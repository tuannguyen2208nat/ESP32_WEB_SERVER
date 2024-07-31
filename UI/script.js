var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        document.getElementById('temperature').innerHTML = data.temperature.toFixed(2);
        document.getElementById('humidity').innerHTML = data.humidity.toFixed(2);
    }
}
