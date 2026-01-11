document.getElementById("data_for_training").addEventListener("submit", function(event) {
    event.preventDefault();
    spinner.style.display = "inline-block";
    let location = document.getElementById("location").value;
    let axis_x = document.getElementById("axis_x").value;
    let axis_y = document.getElementById("axis_y").value;
    let number_of_rssi = document.getElementById("number_of_rssi").value;
    let csrftoken = document.querySelector('input[name="csrfmiddlewaretoken"]').value;
    fetch("/web/collectdata/", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "X-CSRFToken": csrftoken
        },
        body: JSON.stringify({
            location: location,
            axis_x: axis_x,
            axis_y: axis_y,
            number_of_rssi: number_of_rssi
        })
    })
    .then(response => response.json())
    .then(data => {
        let message = document.getElementById("response");
        if (data.status === "error") {
            message.style.color = "red";
            message.innerText = data.message;
            console.log(data.message);
        } else if (data.status === "ok") {
            spinner.style.display = "none";
            message.style.color = "green";
            message.innerText = data.status;
            setTimeout(() => { window.location.href = "/web/collectdata/"; }, 1000);
        }else {
            message.style.color = "green";
            message.innerText = data.status;
        }
    });

});

document.getElementById("finish").addEventListener("click", function() {
    window.location.href = "/web/training/";
});