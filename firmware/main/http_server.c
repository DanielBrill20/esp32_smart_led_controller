#include "http_server.h"

#define LIGHT_BUF_SIZE 32
#define BLINKY_BUF_SIZE 32
#define MORSE_BUF_SIZE 256
#define COLOR_BUF_SIZE 64

static const char* SERVER_TAG = "http server";

static led_t* led;
static morse_iterator_t morse_iterator;

// Declaring URI Handlers
static esp_err_t light_handler(httpd_req_t*);
static esp_err_t blinky_handler(httpd_req_t*);
static esp_err_t morse_handler(httpd_req_t*);
static esp_err_t color_handler(httpd_req_t*);

// Server and Config
static httpd_handle_t server = NULL;
static httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();

// URIs
static httpd_uri_t light_uri = {
    .uri = "/light",
    .method = HTTP_POST,
    .handler = light_handler,
    .user_ctx = NULL
};
static httpd_uri_t blinky_uri = {
    .uri = "/blinky",
    .method = HTTP_POST,
    .handler = blinky_handler,
    .user_ctx = NULL
};
static httpd_uri_t morse_uri = {
    .uri = "/morse",
    .method = HTTP_POST,
    .handler = morse_handler,
    .user_ctx = NULL
};
static httpd_uri_t color_uri = {
    .uri = "/color",
    .method = HTTP_POST,
    .handler = color_handler,
    .user_ctx = NULL
};

// JSON Helpers
static esp_err_t read_request_payload(httpd_req_t* req, char* buf, size_t buf_size)
{
    int payload_len = req->content_len;
    int char_index = 0;
    int received = 0;

    if (payload_len >= buf_size) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }

    while (char_index < payload_len) {
        received = httpd_req_recv(req, buf + char_index, payload_len - char_index);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read request");
            return ESP_FAIL;
        }
        char_index += received;
    }
    buf[payload_len] = '\0'; // Null-terminate JSON string for parsing
    return ESP_OK;
}

static cJSON* json_parser(const char* buf)
{
    cJSON* json = cJSON_Parse(buf);
    if (json == NULL) {
        ESP_LOGE(SERVER_TAG, "Failed to parse JSON string");
    }
    return json;
}

// URI Handlers
static esp_err_t light_handler(httpd_req_t* req)
{
    char buf[LIGHT_BUF_SIZE];
    ESP_ERROR_CHECK(read_request_payload(req, buf, LIGHT_BUF_SIZE));
    cJSON* json = json_parser(buf);

    char* state = cJSON_GetObjectItem(json, "state")->valuestring;

    if (strcmp(state, "on") == 0) {
        set_led_state(led, ON);
    } else if (strcmp(state, "off") == 0) {
        set_led_state(led, OFF);
    } else {
        ESP_LOGE(SERVER_TAG, "Unknown light command");
        cJSON_Delete(json);
        return ESP_FAIL;
    }

    set_led_mode(led, LED_MODE_LIGHT);
    cJSON_Delete(json);
    httpd_resp_sendstr(req, "Successfully activated Light mode");
    return ESP_OK;
}

static esp_err_t blinky_handler(httpd_req_t* req)
{
    char buf[BLINKY_BUF_SIZE];
    ESP_ERROR_CHECK(read_request_payload(req, buf, BLINKY_BUF_SIZE));
    cJSON* json = json_parser(buf);

    uint32_t duration = cJSON_GetObjectItem(json, "duration")->valueint;
    set_led_blink_duration(led, duration);
    set_led_mode(led, LED_MODE_BLINKY);

    cJSON_Delete(json);
    httpd_resp_sendstr(req, "Successfully activated Blinky mode");
    return ESP_OK;
}

static esp_err_t morse_handler(httpd_req_t* req)
{
    char buf[MORSE_BUF_SIZE];
    ESP_ERROR_CHECK(read_request_payload(req, buf, MORSE_BUF_SIZE));
    cJSON* json = json_parser(buf);

    char* morse_code = cJSON_GetObjectItem(json, "morse")->valuestring;
    set_led_morse_code(led, strdup(morse_code));
    set_led_mode(led, LED_MODE_MORSE);

    cJSON_Delete(json);
    httpd_resp_sendstr(req, "Successfully activated Morse Code mode");
    return ESP_OK;
}

static esp_err_t color_handler(httpd_req_t* req)
{
    char buf[COLOR_BUF_SIZE];
    ESP_ERROR_CHECK(read_request_payload(req, buf, COLOR_BUF_SIZE));
    cJSON* json = json_parser(buf);

    uint8_t red = cJSON_GetObjectItem(json, "red")->valueint;
    uint8_t green = cJSON_GetObjectItem(json, "green")->valueint;
    uint8_t blue = cJSON_GetObjectItem(json, "blue")->valueint;
    set_led_rgb(led, red, green, blue);

    cJSON_Delete(json);
    httpd_resp_sendstr(req, "Successfully updated LED color");
    return ESP_OK;
}

// Helpers
static void start_server()
{
    ESP_ERROR_CHECK(httpd_start(&server, &server_config));
    ESP_LOGI(SERVER_TAG, "HTTP server started");
}

static void register_uri_handlers()
{
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &light_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blinky_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &morse_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &color_uri));
    ESP_LOGI(SERVER_TAG, "URI handlers registered");
}

// TODO: Add a stop server function? Maybe also deinit or similar in wifi module?

void http_server_init()
{
    // Setting up server
    start_server();
    register_uri_handlers();

    // Creating LED, preparing iterator for Morse Code mode, initializing timers
    led = create_led(0);
    morse_iterator.led = led;
    morse_iterator.index = 0;
    led_timers_init(led, &morse_iterator);

    // Ensure LED off after flash
    set_led_state(led, OFF);
    set_led_mode(led, LED_MODE_LIGHT);
}
