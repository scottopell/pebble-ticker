Pebble.addEventListener("ready", function(e) {
  console.log("Hello world! - Sent from your javascript application.");
  var msg = { 'value': 66 };
  Pebble.sendAppMessage(msg);
});

// Set callback for appmessage events
Pebble.addEventListener("appmessage", function(e) {
  console.log("message");
  var isInitMsg;
  if (e.payload.init) {
    isInitMsg = true;
    fetchStockQuote(symbol, isInitMsg);
  } else if (e.payload.fetch) {
    isInitMsg = false;
    fetchStockQuote(symbol, isInitMsg);
  } else if (e.payload.symbol) {
    symbol = e.payload.symbol;
    localStorage.setItem("symbol", symbol);
    isInitMsg = false;
    fetchStockQuote(symbol, isInitMsg);
  }
});
