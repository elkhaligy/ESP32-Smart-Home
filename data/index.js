// Client side
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var timer=0;
window.addEventListener('load', onLoad);

function onLoad(event) {
    WEBSOCKET_Init();
    CLIENT_Lamp1ButtonToggle();
    CLIENT_FanButtonToggle();
    CLIENT_ShowCurrentTime();
    CLIENT_TimerButton();
}
function CLIENT_TimerButton(){
    document.getElementById('timer_button').addEventListener('click', CLIENT_TimerSend);
}
function CLIENT_TimerSend(){
    var input= document.getElementById('timer_textfield');
}
function CLIENT_ServerUpTime(time) {

    const milliseconds = time;

    const seconds = Math.floor((milliseconds / 1000) % 60);

    const minutes = Math.floor((milliseconds / 1000 / 60) % 60);

    const hours = Math.floor((milliseconds / 1000 / 60 / 60) % 24);

    const formattedTime = [
        hours.toString().padStart(2, "0"),
        minutes.toString().padStart(2, "0"),
        seconds.toString().padStart(2, "0")
    ].join(":");
    document.getElementById('uptime').innerHTML = formattedTime;

}
function CLIENT_ShowCurrentTime() {
    const today = new Date();
    let h = today.getHours();
    let m = today.getMinutes();
    let s = today.getSeconds();
    var ampm = h >= 12 ? 'pm' : 'am';
    h = h % 12;
    h = h ? h : 12;
    m = m < 10 ? '0' + m : m;
    m = checkTime(m);
    s = checkTime(s);
    document.getElementById('time').innerHTML = h + ":" + m + ":" + s + " " + ampm;
    setTimeout(CLIENT_ShowCurrentTime, 1000);

}

function checkTime(i) {
    if (i < 10) { i = "0" + i };  // add zero in front of numbers < 10
    return i;
}


function CLIENT_Lamp1ButtonToggle() {
    document.getElementById('button').addEventListener('click', CLIENT_onLampToggle);
}

function CLIENT_FanButtonToggle() {
    document.getElementById('button2').addEventListener('click', CLIENT_onFanToggle);
}
function WEBSOCKET_Init() {
    console.log('Trying to open a websocket connection with the ESP server...');
    websocket = new WebSocket(gateway); // When a new websocket object is created the connection onclose is called automatically
    websocket.onopen = WEBSOCKET_onOpen;
    websocket.onclose = WEBSOCKET_onClose;
    websocket.onmessage = WEBSOCKET_onMessage;
}

function CLIENT_onLampToggle(event) {
    // Server will receive this toggle message and perform on it
    // and then notify all clients
    websocket.send('Lamp1');
}
function CLIENT_onFanToggle(event) {
    // Server will receive this toggle message and perform on it
    // and then notify all clients
    websocket.send('Fan');
}
function WEBSOCKET_onOpen(event) {
    console.log('Connection Opened :)');
}
function WEBSOCKET_onClose(event) {
    console.log('Connection Closed :(');

    setTimeout(WEBSOCKET_Init, 2000);
}
function WEBSOCKET_onMessage(event) {
    console.log(`Received a notification from ${event.origin}`);
    console.log(event);
    var data = event.data;
    if (event.data == "Lamp1_ON" || event.data == "Lamp1_OFF") {
        if (data == "Lamp1_OFF")
            document.getElementById('state').innerHTML = data.substr(data.length - 3);
        else if (data == "Lamp1_ON")
            document.getElementById('state').innerHTML = data.substr(data.length - 2);
    }
    else if (event.data == "Fan_ON" || event.data == "Fan_OFF") {
        if (data == "Fan_OFF")
            document.getElementById('state2').innerHTML = data.substr(data.length - 3);
        else if (data == "Fan_ON")
            document.getElementById('state2').innerHTML = data.substr(data.length - 2);
    }

    else {
        CLIENT_ServerUpTime(event.data);
    }
}