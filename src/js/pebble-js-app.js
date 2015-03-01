var randUrl = "https://www.random.org/integers/?num=1&min=10&max=60&col=1&base=10&format=plain&rnd=new";

function ajax(url, method, data, callback) {
  var req = new XMLHttpRequest();
  req.open(method, url, true);
  req.onload = function(e) {
    if (req.readyState === 4 && req.status === 200) {
      if (req.status === 200) {
        callback(JSON.parse(req.responseText));
      } else {
        console.log("HTTP Error: " + JSON.stringify(req));
      }
    }
  };
  req.send(data);
}

var getNewDataLoop = function(newNumber){
  if (newNumber){
    Pebble.sendAppMessage({ 'value': newNumber });
  }
  ajax(randUrl, 'get', null, getNewDataLoop);
};

// Set callback for appmessage events
Pebble.addEventListener("appmessage", function(e) {
  console.log("message");
  var isInitMsg;
  if (e.payload.TICKER) {
    isInitMsg = true;
  }
});

Pebble.addEventListener("ready", function(e) {
  console.log("Hello world! - Sent from your javascript application.");
  var msg = { 'value': 66 };
  getNewDataLoop();
  Pebble.sendAppMessage(msg);
});
