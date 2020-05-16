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

  <title>Husarnet ESP32 example project</title>
</head>

<body onLoad="javascript:WebSocketBegin()">
  <header id="main-header" class="py-2 text-white"
    style="background-image:linear-gradient(56.72deg, #FB6060 16.14%, #EC305D 100.26%)">
    <div class="container">
      <div class="row justify-content-md-center">
        <div class="col-md-8 text-center">
          <h1>Husarnet <i class="fas fa-heart"></i> ESP32</h1>
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
              <p class="card-text"> Parsed from a last frame received over the websocket </p>
              <p id="cnt" class="font-weight-bold">0</p>
              <p id="lastTimestamp">-</p>
              <p id="lastWaveform">-</p>
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
              <form onchange="radioChanged()" name="formRadio" id="radio-buttons">
                <input type="radio" id="radio-sine" name="option" checked>sine<br>
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
                last 100 values received over websocket on port 8001
              </p>
              <canvas id="myCanvas" width="400" height="100"></canvas>
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
