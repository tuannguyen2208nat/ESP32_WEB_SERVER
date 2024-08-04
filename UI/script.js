var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function toggleButton(index) {
    websocket.send("toggle" + index);
}

function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}


function onMessage(event) {
    var data;
    data = JSON.parse(event.data);
    if (data.temperature && data.humidity) {
        document.getElementById('temperature').innerHTML = data.temperature.toFixed(2);
        document.getElementById('humidity').innerHTML = data.humidity.toFixed(2);
    } else {
        var msg = event.data;
        if (msg.startsWith("state")) {
            var index = msg.charAt(5);
            var state = msg.substring(7);
            document.getElementById("state" + index).innerText = state;
        }
    }
}
