/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-20 19:21:27
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 01:33:42
 * @FilePath: \esps3_V1\components\WIFI\src\web_serve.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
// #include "protocol_examples_common.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>

#include "web_serve.h"
#include "Serve_Http.h"

#include "Http_Weather.h"
#include "Http_time.h"
#include "SHT20.h"
#include "mpu6050.h"
#include "power.h"

static const char *TAG = "example";

httpd_handle_t server_OUT = NULL;

char HTTP_Show_Data[HTTP_SHOW_LEN];

/**
 * @brief An HTTP GET handler
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
  char *buf;
  size_t buf_len;

  /* Get header value string length and allocate memory for length + 1,
   * extra byte for null termination */
  buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
  if (buf_len > 1)
  {
    buf = malloc(buf_len);
    /* Copy null terminated value string into buffer */
    if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK)
    {
      ESP_LOGI(TAG, "Found header => Host: %s", buf);
    }
    free(buf);
  }

  buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
  if (buf_len > 1)
  {
    buf = malloc(buf_len);
    if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK)
    {
      ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
    }
    free(buf);
  }

  buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
  if (buf_len > 1)
  {
    buf = malloc(buf_len);
    if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK)
    {
      ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
    }
    free(buf);
  }

  /* Read URL query string length and allocate memory for length + 1,
   * extra byte for null termination */
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1)
  {
    buf = malloc(buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
    {
      ESP_LOGI(TAG, "Found URL query => %s", buf);
      char param[32];
      /* Get value of expected key from query string */
      if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK)
      {
        ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
      }
      if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK)
      {
        ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
      }
      if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK)
      {
        ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
      }
    }
    free(buf);
  }

  // httpd_resp_set_type(req, "html");
  /* Set some custom headers */
  httpd_resp_set_hdr(req, "Content-Type", "text/html");
  httpd_resp_set_hdr(req, "Refresh", "0.500");

  /* Send response with custom headers and body set as the
   * string passed in user context*/
  httpd_resp_send(req, (const char *)HTTP_Show_Data, strlen(HTTP_Show_Data));

  /* After sending the HTTP response the old HTTP request
   * headers are lost. Check if HTTP request headers can be read now. */
  if (httpd_req_get_hdr_value_len(req, "Host") == 0)
  {
    ESP_LOGI(TAG, "Request headers lost");
  }
  return ESP_OK;
}

static const httpd_uri_t User_http_t = {
    .uri = "/baymax",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    /*    <meta charset="utf-8">\*/
    // .user_ctx = User_HTTP
};

/**
 * @brief An HTTP POST handler
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
  char buf[100];
  int ret, remaining = req->content_len;

  while (remaining > 0)
  {
    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf,
                              MIN(remaining, sizeof(buf)))) <= 0)
    {
      if (ret == HTTPD_SOCK_ERR_TIMEOUT)
      {
        /* Retry receiving if timeout occurred */
        continue;
      }
      return ESP_FAIL;
    }

    /* Send back the same data */
    httpd_resp_send_chunk(req, buf, ret);
    remaining -= ret;

    /* Log data received */
    ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    ESP_LOGI(TAG, "%.*s", ret, buf);
    ESP_LOGI(TAG, "====================================");
  }

  // End response
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri = "/echo",
    .method = HTTP_POST,
    .handler = echo_post_handler,
    .user_ctx = NULL};

/**
 * @brief This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 *
 * @param req
 * @param err
 * @return esp_err_t
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
  if (strcmp("/User_http_t", req->uri) == 0)
  {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/User_http_t URI is not available");
    /* Return ESP_OK to keep underlying socket open */
    return ESP_OK;
  }
  else if (strcmp("/echo", req->uri) == 0)
  {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
    /* Return ESP_FAIL to close underlying socket */
    return ESP_FAIL;
  }
  /* For any other URI send 404 and close socket */
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}

/**
 * @brief An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
  char buf;
  int ret;

  if ((ret = httpd_req_recv(req, &buf, 1)) <= 0)
  {
    if (ret == HTTPD_SOCK_ERR_TIMEOUT)
    {
      httpd_resp_send_408(req);
    }
    return ESP_FAIL;
  }

  if (buf == '0')
  {
    /* URI handlers can be unregistered using the uri string */
    ESP_LOGI(TAG, "Unregistering /User_http_t and /echo URIs");
    httpd_unregister_uri(req->handle, "/User_http_t");
    httpd_unregister_uri(req->handle, "/echo");
    /* Register the custom error handler */
    httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
  }
  else
  {
    ESP_LOGI(TAG, "Registering /User_http_t and /echo URIs");
    httpd_register_uri_handler(req->handle, &User_http_t);
    httpd_register_uri_handler(req->handle, &echo);
    /* Unregister custom error handler */
    httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
  }

  /* Respond with empty body */
  httpd_resp_send(req, NULL, 0);
  return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri = "/ctrl",
    .method = HTTP_PUT,
    .handler = ctrl_put_handler,
    .user_ctx = NULL};
/**
 * @brief
 *
 * @return httpd_handle_t
 */
static httpd_handle_t start_webserver(void)
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK)
  {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &User_http_t);
    httpd_register_uri_handler(server, &echo);
    httpd_register_uri_handler(server, &ctrl);
#if CONFIG_EXAMPLE_BASIC_AUTH
    httpd_register_basic_auth(server);
#endif
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}
/**
 * @brief
 *
 * @param server
 */
static void stop_webserver(httpd_handle_t server)
{
  // Stop the httpd server
  httpd_stop(server);
}

/**
 * @brief
 *
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server)
  {
    ESP_LOGI(TAG, "Stopping webserver");
    stop_webserver(*server);
    *server = NULL;
  }
}
/**
 * @brief
 *
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server == NULL)
  {
    ESP_LOGI(TAG, "Starting webserver");
    *server = start_webserver();
  }
}
/**
 * @brief
 *
 */
void Web_Serve(void)
{
  /* Start the server for the first time */
  server_OUT = start_webserver();
}
/**
 * @brief 关闭web服务器
 *
 */
void stop_web_server(void)
{
  if (server_OUT != NULL)
  {
    httpd_stop(server_OUT);
  }
}
/**
 * @brief
 *
 * @param pvParameters
 */
void Web_Test(void *pvParameters)
{
  while (1)
  {
    strcat(HTTP_Show_Data, User_HTTP_Front);
    sprintf(HTTP_Show_Data,
            "SHT20_T:%.2fC.<br>SHT20_H:%.2f%%.<br>\
            MPU_T:->%.2fC.<br>MPU_pitch-->%.2f.<br>\
            MPU_roll--->%.2f.<br>MPU_yaw---->%.2f.<br>\
            BAT_ADC->%.2f.",
            SHT_Value.SHT_T, SHT_Value.SHT_H,
            MPU_Value.Temp, MPU_Value.pitch,
            MPU_Value.roll, MPU_Value.yaw,
            Power_C.BAT_Value);
    strcat(HTTP_Show_Data, User_HTTP_Queen);
    MY_DEBUG("web test runing");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}