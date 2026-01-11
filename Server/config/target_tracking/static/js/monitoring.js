const socket = new WebSocket('ws://' + window.location.host + '/ws/web/monitoring/');
const img = document.getElementById('live-img');

socket.onmessage = function(e) {

  const data = JSON.parse(e.data);

  if (data.topic == "image"){
      img.src = 'data:image/png;base64,' + data.image;
  }
  if (data.topic == "opening"){

    const percentage = parseInt(data.message);
    const opening = document.getElementById('opening');

    opening.style.setProperty('--p', percentage);

    opening.textContent = `${percentage}%`;
  }
  if (data.topic == "mode"){

    const percentage = parseInt(data.message);
    const mode = document.getElementById('mode');

    mode.style.setProperty('--p', percentage);

    mode.textContent = `${percentage}%`;
  }
  if (data.topic == "location_x"){
    const location_x = document.getElementById('axis_x_location');
    location_x.textContent = parseFloat(data.message).toFixed(3);
  }
  if (data.topic == "location_y"){
    const location_y = document.getElementById('axis_y_location');
    location_y.textContent = parseFloat(data.message).toFixed(3);
  }
  if (data.topic == "map_id"){
    const map_id = document.getElementById('map_id');
    map_id.textContent = `${data.message}`;
  }
  if (data.topic == "map_area"){
    const map_area = document.getElementById('map_area');
    map_area.textContent = `${data.message}`;
  }
  if (data.topic == "unit_number"){
    const unit_number = document.getElementById('unit_number');
    unit_number.textContent = `${data.message}`;
  }
  
};

let seconds = 0;
let timer = null;

function updateTimerDisplay() {
  const hrs = String(Math.floor(seconds / 3600)).padStart(2, '0');
  const mins = String(Math.floor((seconds % 3600) / 60)).padStart(2, '0');
  const secs = String(seconds % 60).padStart(2, '0');

  document.getElementById("hours").textContent = hrs;
  document.getElementById("minutes").textContent = mins;
  document.getElementById("seconds").textContent = secs;
}

function startTimer() {
  socket.send(JSON.stringify({
    action: "start",
  }));
  if (!timer) {
    timer = setInterval(() => {
      seconds++;
      updateTimerDisplay();
    }, 1000);
  }
}

function stopTimer() {
  socket.send(JSON.stringify({
    action: "stop",
  }));
  clearInterval(timer);
  timer = null;
}

function resetTimer() {
  socket.send(JSON.stringify({
    action: "stop",
  }));
  stopTimer();
  seconds = 0;
  updateTimerDisplay();
}

function initialize() {
  let axis_x = document.getElementById("axis_x").value;
  let axis_y = document.getElementById("axis_y").value;
  socket.send(JSON.stringify({
    action: "initialize",
    axis_x: axis_x,
    axis_y: axis_y,
  }));
}

function createFire(){
  let fire_x = document.getElementById("fire_x").value;
  let fire_y = document.getElementById("fire_y").value;
  let selectedOption = document.querySelector('input[name="optradio"]:checked');
  let zoom = 0;
    if (selectedOption) {
        let size = selectedOption.value.replace('option', '');
        if (size === '1') {
            zoom = 0.015;
        }else if (size === '2') {
            zoom = 0.023;
        }else if (size === '3') {
            zoom = 0.03;
        }else if (size === '4') {
            zoom = 0.04;
        }
        else {
            zoom = 0.05;
        }
    }
  socket.send(JSON.stringify({
    action: "create_fire",
    fire_x: fire_x,
    fire_y: fire_y,
    zoom: zoom,
  }));
}

function expandSelect(selectElement) {
    // Kiểm tra nếu có nhiều hơn 5 options thì hiển thị 5, nếu không thì hiển thị tất cả
    if(selectElement.options.length > 5) {
        selectElement.size = 5;
    } else {
        selectElement.size = selectElement.options.length;
    }
}

function collapseSelect(selectElement) {
    // Trở về size = 1 khi đã chọn hoặc mất focus
    selectElement.size = 1;
    selectElement.blur(); // Loại bỏ focus để tránh giữ trạng thái expanded
}

function handleSelectChange(selectElement) {
    collapseSelect(selectElement);
    
    const selectedValue = selectElement.value;
    
        // Gửi request với task number đã chọn
        submitTaskSelection(selectedValue);

}

function submitTaskSelection(taskNumber) {
  socket.send(JSON.stringify({
    action: "submit_task",
    task_number: taskNumber,
  }));
}