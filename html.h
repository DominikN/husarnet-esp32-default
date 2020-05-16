R"rawText(
<!DOCTYPE html>
<html>

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <meta http-equiv="X-UA-Compatible" content="ie=edge" />
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.0.13/css/all.css"
    integrity="sha384-DNOHZ68U8hZfKXOrtjWvjxusGo9WQnrNx2sqG0tfsghAvtVlRW3tvkXWZh58N9jp" crossorigin="anonymous" />
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css"
    integrity="sha384-WskhaSGFgHYWDcbwN70/dfYBj47jz9qbsMId/iRN3ewGhXQFZCSftd1LZCfmhktB" crossorigin="anonymous" />

  <script src="https://cdn.jsdelivr.net/npm/chart.js@2.8.0"></script>

  <title>ESP32 + Bootstrap + WebSocket + JSON + Husarnet</title>
</head>

<body onLoad="javascript:WebSocketBegin()">
  <header id="main-header" class="py-2 bg-success text-white">
    <div class="container">
      <div class="row justify-content-md-center">
        <div class="col-md-6 text-center">
          <h1><i class="fas fa-cog"></i> ESP32 control</h1>
        </div>
      </div>
    </div>
  </header>

  <section class="py-5 bg-white">
    <div class="container">
      <div class="row">
        <div class="col">
          <div class="card bg-light m-2" style="min-height: 15rem;">
            <div class="card-header">Input 1</div>
            <div class="card-body">
              <h5 class="card-title">Last Value</h5>
              <p class="card-text">
                A counter value is updated every 100ms by ESP32.
              </p>
              <p id="cnt" class="font-weight-bold">
                0
              </p>
              <p id="lastTimestamp">
                -
              </p>
              <p id="lastWaveform">
                -
              </p>
            </div>
          </div>
        </div>
        <div class="col">
          <div class="card bg-light m-2">
            <div class="card-header">Output</div>
            <div class="card-body">
              <h5 class="card-title">Select a Waveform</h5>
              <p class="card-text">
                that will be sent by ESP32 over websocket on port 8001
              </p>
              <form onchange="radioChanged()" name="frmRadio" id="radio-buttons" action="">
                <input type="radio" id="radio-sine" name="option">sine<br>
                <input type="radio" id="radio-square" name="option">square<br>
                <input type="radio" id="radio-triangle" name="option">triangle<br>
                <input type="radio" id="radio-none" name="option">none<br>
              </form>
            </div>
          </div>


        </div>
      </div>
      <div class="row">
        <div class="col">
          <div class="card bg-light m-2" style="min-height: 15rem;">
            <div class="card-header">Input 2</div>
            <div class="card-body">
              <h5 class="card-title">Chart</h5>
              <p class="card-text">
                last 60 values received over websocket
              </p>
              <canvas id="myCanvas" width="400" height="200"></canvas>
            </div>
          </div>
        </div>
      </div>
    </div>
  </section>
  <script src="default.js"></script>
</body>
</html>
)rawText"
