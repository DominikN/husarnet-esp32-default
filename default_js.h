R"rawText(
var chartData = {
    labels: ["0"],
    datasets: [{
        data: [0],
    }]
};

var chartOptions = {
    scales: {
        yAxes: [{
            ticks: {
                beginAtZero: false
            }
        }]
    },
    legend: {
        display: false
    },
    animation: {
        duration: 0
    }
}

var canvas = document.getElementById("myCanvas");
var waveformChart;

if (canvas) {
    waveformChart = new Chart(canvas, {
        type: 'line',
        data: chartData,
        options: chartOptions,
    });
}

var ws;
var cnt = 0;

function WebSocketBegin() {
    if ("WebSocket" in window) {
        // Let us open a web socket
        ws = new WebSocket(
        	location.hostname.match(/\.husarnetusers\.com$/) ? 
			"wss://" + location.hostname + "/__port_8001/" 
			: "ws://" + location.hostname + ":8001/"
        );
        //ws = new WebSocket(
        //"wss://fc947aa01226481b1087d52fd63ab391-8001.husarnetusers.com"
        //"ws://esp32websocket:8001/"
        //);
        ws.onopen = function () {
            // Web Socket is connected
        };

        ws.onmessage = function (evt) {

            //create a JSON object
            var jsonObject = JSON.parse(evt.data);
            var type = jsonObject.output_type;
            var timestamp = jsonObject.timestamp;
            var value = jsonObject.value;

            var date = new Date(timestamp * 1000);
            var hours = date.getHours();
            var minutes = "0" + date.getMinutes();
            var seconds = "0" + date.getSeconds();
            var miliseconds = "0" + date.getMilliseconds();

            document.getElementById("cnt").innerText = value;
            document.getElementById("lastTimestamp").innerText = hours + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);
            document.getElementById("lastWaveform").innerText = type;
            

            //display only last 100 samples
            if (cnt > 100) {
                waveformChart.data.labels.push(cnt);
                waveformChart.data.labels.splice(0, 1);

                waveformChart.data.datasets.forEach((dataset) => {
                    dataset.data.push(value);
                    dataset.data.splice(0, 1);
                });
            } else {
                waveformChart.data.labels.push(cnt);

                waveformChart.data.datasets.forEach((dataset) => {
                    dataset.data.push(value);
                });
            }
            waveformChart.update();
            cnt = cnt + 1;
        };

        ws.onclose = function () {
            // websocket is closed.
            alert("Connection is closed...");
        };
    } else {
        // The browser doesn't support WebSocket
        alert("WebSocket NOT supported by your Browser!");
    }
}

function radioChanged() {
    if (document.getElementById("radio-sine").checked) {
        ws.send('{"set-output":"sine"}');
    }
    if (document.getElementById("radio-square").checked) {
        ws.send('{"set-output":"square"}');
    }
    if (document.getElementById("radio-triangle").checked) {
        ws.send('{"set-output":"triangle"}');
    }
    if (document.getElementById("radio-none").checked) {
        ws.send('{"set-output":"none"}');
    }
}
)rawText"
