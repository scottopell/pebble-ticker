#include "common.h"

extern char ticker[20];
extern char field[20];
extern void new_value_handler(int);

enum {
  TICKER_KEY   = 0x0,
  FIELD_KEY    = 0x1,
  VALUE_KEY    = 0x2,
};

static char *translate_error(AppMessageResult result);

static void out_sent_handler( DictionaryIterator *sent,
                              void *context ) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "message sent\n" );
}

static void out_failed_handler( DictionaryIterator * failed,
                                AppMessageResult reason,
                                void * context ) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "outbox failure - %s\n", translate_error(reason) );
  // retry on failure
  app_message_outbox_send();
}

static void in_received_handler( DictionaryIterator * received,
                                 void * context ) {
  // incoming message received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE RECIEVED\n" );

  Tuple * ticker_value_tuple = dict_find( received, VALUE_KEY );
  if (ticker_value_tuple != NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "GOT A VALUE, length %d", ticker_value_tuple->length );
    APP_LOG(APP_LOG_LEVEL_DEBUG, "GOT A VALUE, val %d", *(ticker_value_tuple->value->data) );
    //value = *(ticker_value_tuple->value->data);

    (*new_value_handler)((int)*(ticker_value_tuple->value->data));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Value is %d", *(ticker_value_tuple->value->data));
  }

}

static void in_dropped_handler( AppMessageResult reason, void * context ) {
  // incoming message dropped
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Dropped incoming message\n" );
}

void send_ticker(char * ticker){
  int rcode = DICT_INVALID_ARGS;

  DictionaryIterator * outbox_iter;
  if ( app_message_outbox_begin(&outbox_iter) != APP_MSG_OK ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "opening outbox failed\n" );
    return;
  }
  if ( outbox_iter == NULL ){
    return;
  }

  rcode = dict_write_cstring( outbox_iter, TICKER_KEY, ticker);

  if (rcode == DICT_NOT_ENOUGH_STORAGE){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "not enough space in the outbox iter");
  }

  if (rcode == DICT_INVALID_ARGS){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "invalid arguments");
  }

  if ( dict_write_end( outbox_iter ) == 0 ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "the parameters for writing were invalid" );
  }

  app_message_outbox_send();
}

void send_field(char * field){
  int rcode = DICT_INVALID_ARGS;

  DictionaryIterator * outbox_iter;
  if ( app_message_outbox_begin(&outbox_iter) != APP_MSG_OK ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "opening outbox failed\n" );
    return;
  }
  if ( outbox_iter == NULL ){
    return;
  }

  rcode = dict_write_cstring( outbox_iter, FIELD_KEY, field );
  if (rcode == DICT_NOT_ENOUGH_STORAGE){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "not enough space in the outbox iter");
  }

  if (rcode == DICT_INVALID_ARGS){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "invalid arguments");
  }

  if ( dict_write_end( outbox_iter ) == 0 ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "the parameters for writing were invalid" );
  }

  app_message_outbox_send();
}

void app_msg_init(){
  /* register message handlers  */
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  /* message handling init */
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
