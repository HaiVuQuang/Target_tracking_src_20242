const map = document.getElementById("map");
const countValue = document.getElementById("countValue");
let selectedCount = 100;

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
      div.className = "cell";
      let cellId = (row - 2) * 10 + col - 1;
      div.setAttribute("data-id", cellId);

      div.addEventListener("click", function () {
        div.classList.toggle("clicked");
        selectedCount += div.classList.contains("clicked") ? -1 : 1;
        countValue.textContent = selectedCount;
      });
    }

    map.appendChild(div);
  }
}

document.getElementById("map_infor").addEventListener("submit", function(event) {
    event.preventDefault();
    const selectedCells = document.querySelectorAll(".cell.clicked");
    const IDs = Array.from(selectedCells).map((cell) =>
        cell.getAttribute("data-id")
    );
    let cellSelected_message = IDs.join(",");
    let area_of_one_unit = document.getElementById("area_of_one_unit").value;
    let router_number = document.getElementById("router_number").value;
    let router_location = document.getElementById("router_location").value;
    let csrftoken = document.querySelector('input[name="csrfmiddlewaretoken"]').value;
    fetch("/web/settingmap/", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "X-CSRFToken": csrftoken
        },
        body: JSON.stringify({
            total_units: selectedCount,
            area_of_one_unit: area_of_one_unit,
            walkable_area: cellSelected_message,
            router_number: router_number,
            router_location: router_location
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
            setTimeout(() => { window.location.href = "/web/collectdata/"; }, 2000);
        }
    });

});