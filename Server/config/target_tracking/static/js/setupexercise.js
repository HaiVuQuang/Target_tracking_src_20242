const map = document.getElementById("map");
const countValue = document.getElementById("countValue");
let selectedCount = 0;
let selectedCell = null;
const colorPicker = document.getElementById('colorPicker');

// Mapping fire size to color
const fireColorMap = {
    1: 'rgb(255, 251, 0)',    // Vàng
    2: 'rgb(240, 165, 5)',    // Cam nhạt
    3: 'rgb(139, 77, 6)',     // Cam đậm
    4: 'rgb(160, 7, 7)',      // Đỏ đậm
    5: 'red'                  // Đỏ
};

for (let row = 11; row >= 1; row--) {
  for (let col = 1; col <= 11; col++) {
    const div = document.createElement("div");

    if (row == 1 && col == 1) {
      div.className = "cell-empty";
    } else if (row == 1) {
      div.className = "index";
      div.textContent = col - 1.5;
    } else if (col == 1) {
      div.className = "index";
      div.textContent = row - 1.5;
    } else {
      let cellId = (row - 2) * 10 + col - 1;
      if (!unwalkableArea.includes(cellId)) {
        div.className = "cell";
        div.setAttribute("data-id", cellId);

        div.addEventListener("click", function (e) {
        // Kiểm tra nếu ô đã được chọn (có class "clicked")
          if (div.classList.contains("clicked")) {
            // Bỏ chọn ô - chuyển về màu xanh
            div.classList.remove("clicked");
            div.removeAttribute("fire_size");
            div.style.backgroundColor = ""; // Reset về màu mặc định
            selectedCount--;
            countValue.textContent = selectedCount;
          } else {
            // Chọn ô mới - hiển thị color picker
            selectedCell = div;
            colorPicker.style.left = e.pageX + 'px';
            colorPicker.style.top = e.pageY + 'px';
            colorPicker.style.display = 'block';
          }
      })
      }
;
    }

    map.appendChild(div);
  }
}

  // Xử lý chọn màu
  document.querySelectorAll('.color-option').forEach(option => {
    option.addEventListener('click', () => {
      if (selectedCell) {
        selectedCell.setAttribute("fire_size", parseInt(option.dataset.code));
        selectedCell.classList.toggle("clicked");
        selectedCount += selectedCell.classList.contains("clicked") ? 1 : -1;
        countValue.textContent = selectedCount;
        selectedCell.style.backgroundColor = option.dataset.color;
        colorPicker.style.display = 'none';
      }
    });
  });

  // Ẩn menu chọn màu nếu click ra ngoài
  window.addEventListener('click', function(e) {
    if (!colorPicker.contains(e.target) && !e.target.classList.contains('cell')) {
      colorPicker.style.display = 'none';
    }
  });

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

// Function để clear tất cả fires hiện tại
function clearAllFires() {
    const allCells = document.querySelectorAll('.cell.clicked');
    allCells.forEach(cell => {
        cell.classList.remove('clicked');
        cell.removeAttribute('fire_size');
        cell.style.backgroundColor = '';
    });
    selectedCount = 0;
    countValue.textContent = selectedCount;
}

// Function để load fires từ response data
function loadFiresFromData(firesData) {
    clearAllFires(); // Clear fires hiện tại
    
    if (firesData.fires_id && firesData.fires_size) {
        firesData.fires_id.forEach((fireId, index) => {
            const fireSize = firesData.fires_size[index];
            const cell = document.querySelector(`[data-id="${fireId}"]`);
            
            if (cell) {
                cell.classList.add('clicked');
                cell.setAttribute('fire_size', fireSize);
                cell.style.backgroundColor = fireColorMap[fireSize];
                selectedCount++;
            }
        });
        countValue.textContent = selectedCount;
    }
}

// Tự động submit khi trang load
window.addEventListener('load', function() {
    const taskSelect = document.getElementById('taskSelect');
    if (taskSelect && taskSelect.value) {
        // Gọi function xử lý select change
        handleSelectChange(taskSelect);
    }
});

function submitTaskSelection(taskNumber) {
  let csrftoken = document.querySelector('input[name="csrfmiddlewaretoken"]').value;
    // Gửi request với task number đã chọn
    fetch('/web/setupexercise/', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            "X-CSRFToken": csrftoken
        },
        body: JSON.stringify({
            task_number: taskNumber
        })
    })
    .then(response => response.json())
    .then(data => {
        console.log('Task selected:', data);
        // Load fires từ response data
        if (data.fires_id && data.fires_size) {
            loadFiresFromData(data);
        }
    })
    .catch(error => {
        console.error('Error selecting task:', error);
    });
}


document.getElementById("map_infor").addEventListener("submit", function(event) {
    event.preventDefault();
    const selectedCells = document.querySelectorAll(".cell.clicked");
    const IDs = Array.from(selectedCells).map((cell) =>
        cell.getAttribute("data-id")
    );
    const fire_sizes = Array.from(selectedCells).map((cell) =>
        cell.getAttribute("fire_size")
    );
    let cellSelected_message = IDs.join(",");
    let fire_size = fire_sizes.join(",");
    let task = document.getElementById("taskSelect").value;
    let csrftoken = document.querySelector('input[name="csrfmiddlewaretoken"]').value;
    fetch("/web/setupexercise/", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "X-CSRFToken": csrftoken
        },
        body: JSON.stringify({
            total_fires: selectedCount,
            fire_locations: cellSelected_message,
            fire_size: fire_size,
            task: task
        })
    })
    .then(response => response.json())
    .then(data => {
        let message = document.getElementById("response-message");
        if (data.status === "error") {
            message.style.color = "red";
            message.innerText = data.message;
            console.log(data.message);
        } else {
            message.style.color = "green";
            message.innerText = data.status;
        }
    });

});