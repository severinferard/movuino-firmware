#ifndef _MOVUINO_CONFIG_WEB_PAGE_H_
#define _MOVUINO_CONFIG_WEB_PAGE_H_

static const char configWebPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>Movuino</title>
  <meta charset=utf8>
  <style>
    html { height: 100%; }
    body {
      font-size: 0.75em; font-family: "Courier New", Courier, monospace;
      margin: 0px; padding: 0px; width: 100%; height: 100%; max-height: 100%;
    }
    div#wrapper { width: 100%; height: 100%; text-align: center; }
    div#title {
      display: inline-block; width: 100%;
      text-align: center !important; margin-left: auto; margin-right: auto;
    }
    div#title-container { display: inline-block; width: auto; text-align: right; }
    pre { display: inline-block; width: auto; margin: 5px 0px; }
    div#settings {
      display: inline-block; max-width: 300px;
      text-align: left; padding: 0px 10px; }
    div#btn-container { text-align: center; }
    div.wide { width: 100%; }
    label { display: block; margin-bottom: 5px; user-select: none; -moz-user-select: none; }
    label.inline { display: inline; }
    label.half { display: inline-block; }
    label.left { width: 51%; }
    #credentials.disabled label { color: #999; }
    #credentials.disabled label input { background-color: #efefef; }
    label > label { margin-bottom: 0px; }
    input { margin: 0px; padding: 2px; border: 0; background-color: #eee; }
    input[type=text], input[type=password], input[type=number], select { width: 100%; }
    .ip { width: 25px !important; }
    input[type=number] { -moz-appearance: textfield; margin: 0; }
    input[type=number]::-webkit-inner-spin-button,
    input[type=number]::-webkit-outer-spin-button {
      -webkit-appearance: none;
      margin: 0;
    }
  </style>
  <script type=text/javascript>
  window.onload = function() {
    var $userid = document.getElementById('userid');
    var $useWiFi = document.getElementById('useWiFi');
    var $wiFiMode = document.getElementById('wiFiMode');
    var $credentials = document.getElementById('credentials');
    var $ssid = document.getElementById('ssid');
    var $pass = document.getElementById('pass');
    var $hostip1 = document.getElementById('hostip1');
    var $hostip2 = document.getElementById('hostip2');
    var $hostip3 = document.getElementById('hostip3');
    var $hostip4 = document.getElementById('hostip4');
    var $inputPort = document.getElementById('inputPort');
    var $outputPort = document.getElementById('outputPort');
    var $accelRange = document.getElementById('accelRange');
    var $gyroRange = document.getElementById('gyroRange');
    var $useSerial = document.getElementById('useSerial');
    var $readMag = document.getElementById('readMag');
    var $outputFramePeriod = document.getElementById('outputFramePeriod');

    function getRadioValue($radio) {
      var fd = new FormData($radio);
      for (var key of fd.entries()) { return key[1]; }
    }

    function checkWiFiMode() {
      var disabled = $wiFiMode.wiFiMode1.checked;
      if (disabled) {
        $ssid.disabled = $pass.disabled = "disabled";
        $credentials.classList.add('disabled');
      } else {
        $ssid.disabled = $pass.disabled = "";
        $credentials.classList.remove('disabled');
      }
    } 

    checkWiFiMode();
    $wiFiMode.addEventListener('change', checkWiFiMode);

    var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
    // connection.onopen = function() { connection.send('Connect ' /* + new Date() */); };
    // connection.onerror = function(error) { console.log('WebSocket Error ', error); };
    connection.onmessage = function(e) {
      var arg;
      var args = e.data.split('\n');
      console.log(e.data);
      if (args[0] === 'settings') {
        $userid.value = args[1];
        $useWiFi.checked = parseInt(args[2]) != 0;

        arg = parseInt(args[3]);
        arg = isNaN(arg) ? 0 : (arg < 0 ? 0 : (arg > 1 ? 1 : arg));
        document.querySelector('#wiFiMode' + arg).checked = true;

        $ssid.value = args[4];
        $pass.value = args[5];

        arg = parseInt(args[6]);
        $hostip1.value = (!isNaN(arg) && arg >= 0) ? arg : 0;
        arg = parseInt(args[7]);
        $hostip2.value = (!isNaN(arg) && arg >= 0) ? arg : 0;
        arg = parseInt(args[8]);
        $hostip3.value = (!isNaN(arg) && arg >= 0) ? arg: 0;
        arg = parseInt(args[9]);
        $hostip4.value = (!isNaN(arg) && arg >= 0) ? arg: 0;

        arg = parseInt(args[10]);
        $inputPort.value = (!isNaN(arg) && arg >= 0) ? arg : 0;
        arg = parseInt(args[11]);
        $outputPort.value = (!isNaN(arg) && arg >= 0) ? arg : 0;

        arg = parseInt(args[12]);
        arg = isNaN(arg) ? 0 : (arg < 0 ? 0 : (arg > 3 ? 3 : arg));
        document.querySelector('#accelRange' + arg).checked = true;

        arg = parseInt(args[13]);
        arg = isNaN(arg) ? 0 : (arg < 0 ? 0 : (arg > 3 ? 3 : arg));
        document.querySelector('#gyroRange' + arg).checked = true;

        $useSerial.checked = parseInt(args[14]) != 0;
        $readMag.checked = parseInt(args[15]) != 0;

        arg = parseInt(args[16]);
        $outputFramePeriod.value = (!isNaN(arg) && arg >= 0) ? arg : 0;

        var fullId = `${args[17]}@${args[18]}`;
        document.getElementById('version').innerHTML = fullId;

        checkWiFiMode();
      }
    };

    document.getElementById('updateBtn').addEventListener('click', function() {
      var settings = 'settings\n';
      settings += `${$userid.value}\n`;
      settings += $useWiFi.checked ? '1\n' : '0\n';
      settings += `${getRadioValue($wiFiMode)}\n`;
      settings += `${$ssid.value}\n${$pass.value}\n`;
      settings += `${$hostip1.value}\n`;
      settings += `${$hostip2.value}\n`;
      settings += `${$hostip3.value}\n`;
      settings += `${$hostip4.value}\n`;
      settings += `${$inputPort.value}\n${$outputPort.value}\n`;
      settings += `${getRadioValue($accelRange)}\n`;
      settings += `${getRadioValue($gyroRange)}\n`;
      settings += $useSerial.checked ? '1\n' : '0\n';
      settings += $readMag.checked ? '1\n' : '0\n';
      settings += `${$outputFramePeriod.value}\n`;
      connection.send(settings);
    });

    document.getElementById('clearBtn').addEventListener('click', function() {
      connection.send('clear\n');
    });
  };
  </script>
</head>
<body>
  <div id=wrapper>

    <div id=title>
      <div id=title-container>
      <pre>
 __   __  _______  __   __  __   __  ___   __    _  _______ 
|  |_|  ||       ||  | |  ||  | |  ||   | |  |  | ||       |
|       ||   _   ||  |_|  ||  | |  ||   | |   |_| ||   _   |
|       ||  | |  ||       ||  |_|  ||   | |       ||  | |  |
|       ||  |_|  ||       ||       ||   | |  _    ||  |_|  |
| ||_|| ||       | |     | |       ||   | | | |   ||       |
|_|   |_||_______|  |___|  |_______||___| |_|  |__||_______|
      </pre>
      <br>
      <span id=version>&nbsp;</span>
      </div>
    </div>

    <div id=settings>
      <label>movuino id <input type=text id=userid></label>
      <label><input type=checkbox id=useWiFi> enable WiFi on boot</label>
      <label>
        mode
        <form id=wiFiMode>
          <label for=wiFiMode0 class=inline>
            <input type=radio id=wiFiMode0 name=wiFiMode value=0> station
          </label>
          <label for=wiFiMode1 class=inline>
            <input type=radio id=wiFiMode1 name=wiFiMode value=1> access point
          </label>
        </form>
      </label>
      <div id=credentials>
      <label>network ssid <input type=text id=ssid></label>
      <label>network password <input type=password id=pass></label>
      </div>
      <label>
        host IP
        <div class=wide>
        <input type=number id=hostip1 class=ip>.
        <input type=number id=hostip2 class=ip>.
        <input type=number id=hostip3 class=ip>.
        <input type=number id=hostip4 class=ip>
        </div>
      </label>
      <label>input UDP port <input type=number id=inputPort></label>
      <label>output UDP port <input type=number id=outputPort></label>
      <label class="half left">
        accelerometer range
        <form id=accelRange>
          <label><input type=radio id=accelRange0 name=accelRange value=0> +/- 2g</label>
          <label><input type=radio id=accelRange1 name=accelRange value=1> +/- 4g</label>
          <label><input type=radio id=accelRange2 name=accelRange value=2> +/- 8g</label>
          <label><input type=radio id=accelRange3 name=accelRange value=3> +/- 16g</label>
        </form>
      </label>
      <label class="half right">
        gyroscope range
        <form id=gyroRange>
          <label><input type=radio id=gyroRange0 name=gyroRange value=0> +/- 250 deg/sec</label>
          <label><input type=radio id=gyroRange1 name=gyroRange value=1> +/- 500 deg/sec</label>
          <label><input type=radio id=gyroRange2 name=gyroRange value=2> +/- 1000 deg/sec</label>
          <label><input type=radio id=gyroRange3 name=gyroRange value=3> +/- 2000 deg/sec</label>
        </form>
      </label>
      </label>
      <label><input type=checkbox id=useSerial> enable serial communication</label>
      <label><input type=checkbox id=readMag> enable magnetometer</label>
      <label>output frame period (ms) <input type=number id=outputFramePeriod></label>
      <div id=btn-container>
        <button id=updateBtn>save</button>
        <button id=clearBtn>clear</button>
      </div>
    </div>

  </div>
</body>
</html>
)=====";

#endif /* _MOVUINO_CONFIG_WEB_PAGE_H_ */