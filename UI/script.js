var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    var data;
    try {
        data = JSON.parse(event.data);
        if (data.temperature && data.humidity) {
            document.getElementById('temperature').innerHTML = data.temperature.toFixed(2);
            document.getElementById('humidity').innerHTML = data.humidity.toFixed(2);
        } else if (data.relay && (data.state === "1" || data.state === "0")) {
            var state = data.state === "1" ? "ON" : "OFF";
            document.getElementById(`state${data.relay}`).innerHTML = state;
        }
    } catch (e) {
        console.error('Error parsing message data:', e);
    }
}

function onLoad(event) {
    initWebSocket();
    initButtons();
}

function initButtons() {
    var buttons = document.querySelectorAll('.button');
    buttons.forEach(function (button, index) {
        button.addEventListener('click', function () {
            toggle(index + 1);
        });
    });
}

function toggle(relay) {
    websocket.send(JSON.stringify({ relay: relay, action: 'toggle' }));
}
