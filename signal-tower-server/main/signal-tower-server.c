/* HTTP server */
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_http_server.h>

/* GPIO */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

// #define BLINK_GPIO CONFIG_BLINK_GPIO
#define BLINK_GPIO_1 0
#define BLINK_GPIO_2 2
#define BLINK_GPIO_3 5

static const char *TAG = "example";

/* Initialize Wi-Fi as sta and set scan method */
static void wifi_init(void)
{
    tcpip_adapter_init();

    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);

    /* IPアドレスの設定 */
    tcpip_adapter_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, xxx, xxx, xxx, xxx);
    IP4_ADDR(&ipInfo.gw, xxx, xxx, xxx, xxx);
    IP4_ADDR(&ipInfo.netmask, xxx, xxx, xxx, xxx);
    tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
}


/* An HTTP GET handler */
static esp_err_t light_get_handler(httpd_req_t *req)
{

    char*  buf;
    size_t buf_len;

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            /* クエリ全体の文字列を取得する */
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "color", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => color=%s", param);

                char *green = "green";
                char *yellow = "yellow";
                char *red = "red";
                if(strcmp(param,green)==0) {
                    /* 緑 */
                    gpio_set_level(BLINK_GPIO_1, 0);
                    gpio_set_level(BLINK_GPIO_2, 0);
                    gpio_set_level(BLINK_GPIO_3, 1);
                    /* Blink on (output high) */
                    printf("Turning on the LED\n");
                } else if(strcmp(param,yellow)==0) {
                    /* 黄 */
                    gpio_set_level(BLINK_GPIO_1, 0);
                    gpio_set_level(BLINK_GPIO_2, 1);
                    gpio_set_level(BLINK_GPIO_3, 0);
                    /* Blink on (output high) */
                    printf("Turning on the LED\n");
                } else if(strcmp(param,red)==0) {
                    /* 赤 */
                    gpio_set_level(BLINK_GPIO_1, 1);
                    gpio_set_level(BLINK_GPIO_2, 0);
                    gpio_set_level(BLINK_GPIO_3, 0);
                    /* Blink on (output high) */
                    printf("Turning on the LED\n");
                } else {
                    /* 全部OFF */
                    gpio_set_level(BLINK_GPIO_1, 0);
                    gpio_set_level(BLINK_GPIO_2, 0);
                    gpio_set_level(BLINK_GPIO_3, 0);
                }
            }
        }
        free(buf);
    }

    return ESP_OK;
}

static const httpd_uri_t light = {
    .uri       = "/light",
    .method    = HTTP_GET,
    .handler   = light_get_handler,
    .user_ctx  = NULL
};

/* 404 error */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/light", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/light URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &light);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void app_main(void)
{

    /* gpio init */
    gpio_reset_pin(BLINK_GPIO_1);
    gpio_reset_pin(BLINK_GPIO_2);
    gpio_reset_pin(BLINK_GPIO_3);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO_3, GPIO_MODE_OUTPUT);

    /* Blink off (output low) */
    printf("Turning off the LED\n");
    gpio_set_level(BLINK_GPIO_1, 0);
    gpio_set_level(BLINK_GPIO_2, 0);
    gpio_set_level(BLINK_GPIO_3, 0);

    /* server init */
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */

    /* wifi init */
    wifi_init();

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

    /* Start the server for the first time */
    server = start_webserver();
}
