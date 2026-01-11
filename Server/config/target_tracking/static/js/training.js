const socket = new WebSocket(
    'ws://'
    + window.location.host
    + '/ws/web/training/'
);
socket.onmessage = function(e){
    const data = JSON.parse(e.data);
    document.querySelector('#display').innerText = data.message;
}
document.getElementById("sendButton").addEventListener("click", function(){
    socket.send(JSON.stringify({
        action: "run_model",
    }));
});
document.getElementById("setupexercise").addEventListener("click", function(){
    window.location.href = "/web/setupexercise/";
});