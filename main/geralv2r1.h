#ifndef GERALV2R1_h
#define GERALV2R1_h

#include <stdio.h>								// Acesso as operacoes de entradas/saidas.
#include <string.h>								// Acesso as manipulacoes de strings. 

#include "freertos/FreeRTOS.h"					// Acesso aos termos.
#include "freertos/task.h"						// Acesso as prioridades da TASK.
#include "freertos/queue.h"
#include "freertos/event_groups.h"              // 

#include "driver/gpio.h"						// Acesso ao uso das GPIOs.
#include "driver/adc.h"							// Modulo conversor ADC. OBSOLETO!
#include "soc/adc_channel.h"

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"


#include "esp_system.h"                         // 
#include "esp_wifi.h"                           // 
#include "esp_event.h"          				// => esp_event_base_t
#include "esp_log.h"							// Acesso ao uso dos LOGs.
#include "esp_http_client.h"                    // 
#include "esp_http_server.h"    				// => httpd_uri_t
#include "esp_tls_crypto.h"						// => esp_crypto_base64_encode
#include "esp_netif.h"                          // 
#include "esp_wifi.h"                           // 

#include "nvs_flash.h"          				// => nvs_flash_init

#include "lwip/err.h"                           // 
#include "lwip/sys.h"                           // 
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "rom/ets_sys.h"						// Acesso a escala de tempo em micro segundos.

#include "sys/param.h"          				// => MIN()

#include "mqtt_client.h"

/* RTOS */
#define CONFIG_FREERTOS_HZ 100					// Definicao da Espressif. Escala de tempo base (vTaskDelay).

/* GERAL */
#define bitX(valor,bit) (valor&(1<<bit))		// Testa e retorna o 'bit' de 'valor'.
#define bit1(valor,bit) valor |= (1<<bit)		// Faz o 'bit' de 'valor' =1.
#define bit0(valor,bit) valor &= ~(1<<bit)		// Faz o 'bit' de 'valor' =0.

#endif