#include "http_server.h"

static const char* SERVER_TAG = "http server";

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

// URI Handlers
static esp_err_t light_handler(httpd_req_t* req)
{
    return ESP_OK;
}

static esp_err_t blinky_handler(httpd_req_t* req)
{
    // // The LED strip object
    // led_strip_handle_t led_handle = NULL;
    // // Creating the LED strip based on RMT TX channel, checks for errors
    // // ESP_ERROR_CHECK checks an esp_err_t value and prints an error message to the console if the value isn't 0 (ESP_OK)
    // ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));

    // // Creating instance of my own led struct
    // led my_led = {
    //     .led_handle = led_handle,
    //     .index = 0,
    //     .red = 50,
    //     .green = 20,
    //     .blue = 0,
    //     .state = false
    // };

    // char buf[10];  // Enough for small payloads like "0", "1"
    // int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    // if (ret <= 0) {
    //     httpd_resp_send_500(req);
    //     return ESP_FAIL;
    // }

    // buf[ret] = '\0';  // Null-terminate

    // if (strcmp(buf, "0") == 0) {
    //     ESP_ERROR_CHECK(led_strip_clear(my_led.led_handle));
    //     ESP_LOGI("", "LED Off");
    // } else if (strcmp(buf, "1") == 0) {
    //     ESP_ERROR_CHECK(led_strip_set_pixel(my_led.led_handle, my_led.index, my_led.red, my_led.green, my_led.blue));
    //     ESP_ERROR_CHECK(led_strip_refresh(my_led.led_handle));
    //     ESP_LOGI("", "Outputting color (%d, %d, %d)", my_led.red, my_led.green, my_led.blue);
    // }
    return ESP_OK;
}

static esp_err_t morse_handler(httpd_req_t* req)
{
    return ESP_OK;
}

static esp_err_t color_handler(httpd_req_t* req)
{
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
    start_server();
    register_uri_handlers();
}
