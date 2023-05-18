#ifndef WIFISTARGBV1R1_H
#define WIFISTARGBV1R1_H

#include "config.h"
#include "geralv2r1.h"
#include "lcdv2r6.h"

// Pelo SDKCONFIG
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10                              // Define quantas pilhas estaticas (min).  //  Usar para nao dar erro no WIFI_INIT_CONFIG_DEFAULT()
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32                             // Define quantas pilhas dinamicas (max).  //  Usar para nao dar erro no WIFI_INIT_CONFIG_DEFAULT()    
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1                                    // Define o tipo de Buffer da Tx do WiFi.   //  Usar para nao dar erro no WIFI_INIT_CONFIG_DEFAULT()


//Essa configuração define o número máximo de dispositivos que podem ser criptografados ao usar o protocolo ESP-NOW no ESP32. Se o valor for definido como 0, nenhum dispositivo será criptografado. Se um valor diferente de 0 for definido, somente os dispositivos especificados serão criptografados. A criptografia é importante para garantir a segurança das informações transmitidas entre os dispositivos.
#define CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM 0                          //  Usar para nao dar erro no WIFI_INIT_CONFIG_DEFAULT()





#define CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK                                     // Define a seguranca da conexao.           
#define ESP_MAXIMUM_RETRY 5                                                  // Numero de tentativas de conexao.
#define MAX_HTTP_RECV_BUFFER 512                                            // Tamanho (em bytes) do buffer de entrada.
#define MAX_HTTP_OUTPUT_BUFFER 2048                                        // Tamanho (em bytes) do buffer de saida.
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK            // Modo de autenticacao.






#define CONFIG_HTTPD_MAX_REQ_HDR_LEN 2048                                 //Para o erro: Header fields are too long
#define CONFIG_HTTPD_MAX_URI_LEN 1024

int vlrQuery1; // Variavel para 'Query' do metodo 'GET'.
char vlrAscIn[] = {"0000"};



/* Aplicacao */
char __convVlr(char valor) // Sub rotina de conversao de valores.
{
    char volta = 0; // Variavel Local criada e inicializada (obrigatorio).
    if (valor >= '0' && valor <= '9')
        volta = (valor - 48); // Calcula valores numericos.
    if (valor >= 'A' && valor <= 'F')
        volta = (valor - 55); // Calcula alfa maiusculas.
    if (valor >= 'a' && valor <= 'f')
        volta = (valor - 87); // Calcula alfa minusculas.
    return (volta);           // Retorna o valor calculado.
}



void int2Asc(unsigned int valor, char *buffer, char digi) // Converte INT em ASCII: Valor(Bin), Matriz, Numero de digitos (0 a 5).
{
    if (digi > 5)
        digi = 5; // Previne erros.
    switch (digi) // Seleciona o numero de algarismos.
    {
    case 0:                                       // Nao converte o Valor(Bin).
        break;                                    // Retorno.
    case 1:                                       // Um algarismo.
        buffer[0] = (valor % 10) + 0x30;          // Separa a unidade.
        break;                                    // Retorno.
    case 2:                                       // Dois algarismos.
        buffer[0] = (valor / 10) + 0x30;          // Separa a dezena.
        buffer[1] = (valor % 10) + 0x30;          // Separa a unidade.
        break;                                    // Retorno.
    case 3:                                       // Tres algarismos.
        buffer[0] = (valor / 100) + 0x30;         // Separa a centena.
        buffer[1] = ((valor / 10) % 10) + 0x30;   // Separa a dezena.
        buffer[2] = (valor % 10) + 0x30;          // Separa a unidade.
        break;                                    // Retorno.
    case 4:                                       // Quatro algarismos.
        buffer[0] = (valor / 1000) + 0x30;        // Separa a unidade de milhar.
        buffer[1] = ((valor / 100) % 10) + 0x30;  // Separa a centena.
        buffer[2] = ((valor / 10) % 10) + 0x30;   // Separa a dezena.
        buffer[3] = (valor % 10) + 0x30;          // Separa a unidade.
        break;                                    // Retorno.
    case 5:                                       // Cinco algarismos.
        buffer[0] = (valor / 10000) + 0x30;       // Separa a dezena de milhar.
        buffer[1] = ((valor / 1000) % 10) + 0x30; // Separa a unidade de milhar
        buffer[2] = ((valor / 100) % 10) + 0x30;  // Separa a centena.
        buffer[3] = ((valor / 10) % 10) + 0x30;   // Separa a dezena.
        buffer[4] = (valor % 10) + 0x30;          // Separa a unidade.
        break;                                    // Retorno.
    }
}

/**
 * @brief   Grupo de eventos do FreeRTOS para sinalizar quando estamos conectados.
 */
static EventGroupHandle_t s_wifi_event_group;

/* O grupo de eventos permite varios bits para cada evento, mas nos preocupamos apenas com dois eventos:
 * - estamos conectados ao AP com um IP.
 * - falhamos ao conectar apos a quantidade maxima de tentativas. */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

int ipPos1 = 0, ipPos2 = 0, ipPos3 = 0, ipPos4 = 0; // Variaveis para salvar o valor IP.
static const char *TAG = "N1K_ESP32";               // TAG da Task atual.
static int s_retry_num = 0;                         // Variavel para o numero de tentativas.

/**
 * @brief   Manipulador de eventos. Responsavel pelos eventos do WiFi.
 */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) // Se ocorreu um evento do WiFi, E, o modo foi Station iniciou...
    {
        esp_wifi_connect(); // Tenta conectar o WiFi do ESP ao Hub(AP).
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) // Se ocorreu um evento do WiFi, E, o modo foi Station desconectou...
    {
        if (s_retry_num < ESP_MAXIMUM_RETRY) // Se nao atingiu o limite de tentativas...
        {
            esp_wifi_connect();                       // Tenta conectar o WiFi do ESP ao Hub(AP).
            s_retry_num++;                            // Incrementa o numero de tentativas.
            ESP_LOGI(TAG, "Tentando conexao no AP."); // Envia a mensagem para o Terminal (Informacao).
        }
        else // Se acabou o numero de tentativas...
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT); // Ativa o bit de falha no grupo de eventos.
        }
        ESP_LOGI(TAG, "Falha conexao no AP."); // Envia a mensagem para o Terminal (Informacao).
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) // Se ocorreu o evento do IP, E, pegou um IP.
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;    // Carrega os dados do evento.
        ESP_LOGI(TAG, "Peguei IP:" IPSTR, IP2STR(&event->ip_info.ip)); // Envia os dados para o Terminal (Informacao).
        ipPos1 = esp_ip4_addr1(&event->ip_info.ip);                    // Copia o valor do grupo 1 do endereco IP.
        ipPos2 = esp_ip4_addr2(&event->ip_info.ip);                    // Copia o valor do grupo 2 do endereco IP.
        ipPos3 = esp_ip4_addr3(&event->ip_info.ip);                    // Copia o valor do grupo 3 do endereco IP.
        ipPos4 = esp_ip4_addr4(&event->ip_info.ip);                    // Copia o valor do grupo 4 do endereco IP.
        s_retry_num = 0;                                               // Zera o numero de tentativas de conexao.
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);    // Ativa o bit de conectado no grupo de eventos.
    }
}

// static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
// {
//     if (event_id == WIFI_EVENT_AP_STACONNECTED)
//     {
//         wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
//         ESP_LOGI(TAG, "Estacao "MACSTR" entrou, AID=%d", MAC2STR(event->mac), event->aid);
//     } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
//     {
//         wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
//         ESP_LOGI(TAG, "Estacao "MACSTR" saiu, AID=%d", MAC2STR(event->mac), event->aid);
//     }
// }

// void wifi_init_softap(void)
// {
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_ap();
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
//     wifi_config_t wifi_config = {
//         .ap = {
//             .ssid = CONFIG_ESP_WIFI_SSID_AP,
//             .ssid_len = strlen(CONFIG_ESP_WIFI_SSID_AP),
//             .channel = CONFIG_ESP_WIFI_CHANNEL,
//             .password = CONFIG_ESP_WIFI_PASSWORD_AP,
//             .max_connection = CONFIG_ESP_MAX_STA_CONN,
//             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
//         },
//     };
//     // if (strlen(CONFIG_ESP_WIFI_PASSWORD) == 0)
//     // {
//     //     wifi_config.ap.authmode = WIFI_AUTH_OPEN;
//     // }
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());
// 	esp_wifi_connect();
//     ESP_LOGI(TAG, "Soft AP Ok: SSID:%s Pass:%s Canal:%d", CONFIG_ESP_WIFI_SSID_AP, CONFIG_ESP_WIFI_PASSWORD_AP, CONFIG_ESP_WIFI_CHANNEL);
// }

/**
 * @brief   Inicializador do WiFi no modo Station.
 */
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();                                                                                    // Cria instancia do grupo de eventos.
    ESP_ERROR_CHECK(esp_netif_init());                                                                                           // Verifica a inicializacao da interface.
    esp_netif_create_default_wifi_sta();                                                                                         // Cria instancia no modo Station.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();                                                                         // Carrega a estrutura de itens basicos de configuracao.
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                                                                                        // Carrega os itens que compoe a configuracao.
    esp_event_handler_instance_t instance_any_id;                                                                                // Cria instancia para o manipulador de: Id.
    esp_event_handler_instance_t instance_got_ip;                                                                                // Cria instancia para o manipulador de: IP.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));  // Verifica o registro de instancias para os eventos de: WiFi (Id).
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip)); // Verifica o registro de instancias para os eventos de: WiFi (IP).

    wifi_config_t wifi_config = // Carrega os itens que compoe a configuracao.
        {
            .sta = // Configuracao do Modulo Station.
            {
                .ssid = CONFIG_ESP_WIFI_SSID_STA,         // Nome do Hub AP (SSID) (Encontra-se no arquivo config.h).
                .password = CONFIG_ESP_WIFI_PASSWORD_STA, // Senha de acesso ao Hub AP (Encontra-se no arquivo config.h).
                                                          /*  Definir uma senha implica que a estacao se conectara a todos os modos de seguranca, incluindo WEP/WPA.
                                                              No entanto, esses modos sao obsoletos e nao sao aconselhaveis de serem usados. Caso seu ponto de acesso
                                                              nao suporte WPA2, esse modo pode ser ativado comentando a linha abaixo.*/
                .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                // .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
        };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));               // Verifica se foi aceito o ajuste para o modo Station.
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config)); // Verifica se foi aceito as configuracoes para o modo.
    ESP_ERROR_CHECK(esp_wifi_start());                               // Verifica se foi inicializado o modulo WiFi.
    ESP_LOGI(TAG, "wifi_init_sta finalizado.");                      // Envia mensagem ao Terminal (Informacao).

    /* Aguardando ate que a conexao seja estabelecida (WIFI_CONNECTED_BIT) ou falha na conexao pelo numero maximo
    de novas tentativas (WIFI_FAIL_BIT). Os bits sao definidos por event_handler() (veja acima). */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() retorna os bits antes do retorno da chamada, portanto, podemos testar qual evento realmente aconteceu. */
    if (bits & WIFI_CONNECTED_BIT) // Se houve conexao...
    {
        ESP_LOGI(TAG, "Conectado ao AP SSID:%s Senha:%s", CONFIG_ESP_WIFI_SSID_STA, CONFIG_ESP_WIFI_PASSWORD_STA); // Envia mensagem e dados ao Terminal (Informacao).
    }
    else if (bits & WIFI_FAIL_BIT) // Se falhou ao conectar...
    {
        ESP_LOGI(TAG, "Falhou ao conectar SSID:%s, Senha:%s", CONFIG_ESP_WIFI_SSID_STA, CONFIG_ESP_WIFI_PASSWORD_STA); // Envia mensagem e dados ao Terminal (Informacao).
    }
    else // Se ocorreu qualquer outra coisa...
    {
        ESP_LOGE(TAG, "Evento inesperado!"); // Envia mensagem ao Terminal (Informacao).
    }

    /* O evento nao sera processado apos o cancelamento do registro. */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip)); // Verifica a remocao do registro de instancias para os eventos de: WiFi (Id).
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));  // Verifica a remocao do registro de instancias para os eventos de: WiFi (IP).
    vEventGroupDelete(s_wifi_event_group);                                                                  // Remove o grupo de eventos (libera recursos).
}

/**
 * @brief   Inicializador da memoria flash interna.
 * @note    Memoria para os dados do WiFi (SSID, Senha, IP, etc).
 */
void nvs_init(void)
{
    esp_err_t ret = nvs_flash_init();                                             // Cria instancia da inicializacao da NVS RAM.
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) // Verifica qual foi o resultado... Se nao houver espaco ou nova versao...
    {
        ESP_ERROR_CHECK(nvs_flash_erase()); // Verifica se o apagar memoria funcionou.
        ret = nvs_flash_init();             // Inicializa a NVS RAM.
    }
    ESP_ERROR_CHECK(ret);                             // Verifica se houve erro(s).
    ESP_ERROR_CHECK(esp_netif_init());                // Verifica a inicializacao da interface WiFi.
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Verifica se foi criado um Loop de eventos.
}

// esp_err_t get_handler(httpd_req_t *req)
// {
//     const char resp[] = "<!DOCTYPE html><html><head><title>ESP-IDF Fita Led</title>
// <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>
// <body><style>*{font-family: Verdana;text-align: center;}</style>
// <h2>Color Picker</h2><form action=\"\"><label for=\"rgb\"><h3>Cor:</h3></label>
// <input type=\"color\" id=\"rgb\" name=\"rgb\" value=\"#0000ff\"><br><br><input type=\"submit\">
// </form></body></html>";

//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     return (ESP_OK);
// }

// esp_err_t get_handler_rgb(httpd_req_t *req)
// {
//     /**/
//     const char resp[] = "<!DOCTYPE html><html><head><title>ESP-IDF Fita Led</title>
// <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>
// <body><style>*{font-family: Verdana;text-align: center;}</style>
// <h2>Color Picker</h2><form action=\"\" method=\"get\"><label for=\"rgb\"><h3>Cor:</h3></label>
// <input type=\"color\" id=\"rgb\" name=\"rgb\" value=\"#0000ff\"><br><br><input type=\"submit\">
// </form></body></html>";

//     // httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     /**/

//     // Le a linha da URI e pega o host.
//     char *buf;
//     size_t buf_len;
//     // buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
//     // if (buf_len > 1)
//     // {
//     //     buf = (char *) malloc(buf_len);
//     //     if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK)
//     //     {
//     //         ESP_LOGI(TAG, "Host: %s", buf);
//     //     }
//     //     free(buf);
//     // }

//     // Le a linha da URI e pega o(s) parametro(s).
//     buf_len = httpd_req_get_url_query_len(req) + 1;
//     if (buf_len > 1)
//     {
//         char param[64]; // era 32
//         buf = (char *) malloc(buf_len);
//         if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
//         {
//             ESP_LOGI(TAG, "Dado na URL: %s", buf);
//             // char param[64]; // era 32
//             if (httpd_query_key_value(buf, "rgb", param, sizeof(param)) == ESP_OK)
//             {
//                 ESP_LOGI(TAG, "Valor RGB= %s", param);
//                 hex2dec(param);
//                 printf("Valor R= %d\n", valorR);
//                 printf("Valor G= %d\n", valorG);
//                 printf("Valor B= %d\n", valorB);
//             }
//         }
//         free(buf);
//     }
//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     get_handler(req);
//     return (ESP_OK);
// }

// /* Manipulador da estrutura da URI para metodo GET /uri */
// httpd_uri_t uri_get =
//     {
//     .uri = "/",
//     .method = HTTP_GET,
//     .handler = get_handler,
//     .user_ctx = NULL
//     };

// httpd_uri_t uri_get_rgb =
//     {
//     .uri = "/cor",
//     .method = HTTP_GET,
//     .handler = get_handler_rgb,
//     .user_ctx = NULL
//     };

/**
 * @brief   Manipulador do metodo GET para uso das saidas (GPIO e MP).
 * @note    Identifica os pares Query-Key e Query-Value.
 * @note    Sao utilizados: gpio?saida=____ e gpio?ang=____.
 */
static esp_err_t gpio_get_handler(httpd_req_t *req)
{


    
    char *buf;                                              // Reserva memoria para o Buffer.
    size_t buf_len;                                         // Variavel do tamanho do Buffer.
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1; // Obtem o comprimento da string do cabecalho, aloca memoria +1, para terminacao nula.
    if (buf_len > 1)                                        // Se houver dados no Buffer...
    {
        buf = (char *)malloc(buf_len);                                        // Aloca os dados conforme tamanho no Buffer.
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) // Se encontrar a String...
        {
            ESP_LOGI(TAG, "Found header => Host: %s", buf); // Envia a mensagem para o Terminal (Informacao).
        }
        free(buf); // Libera memoria alocada (Conserva recursos).
    }
    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1; // Obtem o comprimento da string do cabecalho, aloca memoria +1, para terminacao nula.
    if (buf_len > 1)                                                 // Se houver dados no Buffer...
    {
        buf = (char *)malloc(buf_len);                                                 // Aloca os dados conforme tamanho no Buffer.
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) // Se encontrar a String...
        {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf); // Envia a mensagem para o Terminal (Informacao).
        }
        free(buf); // Libera memoria alocada (Conserva recursos).
    }
    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1; // Obtem o comprimento da string do cabecalho, aloca memoria +1, para terminacao nula.
    if (buf_len > 1)                                                 // Se houver dados no Buffer...
    {
        buf = (char *)malloc(buf_len);                                                 // Aloca os dados conforme tamanho no Buffer.
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) // Se encontrar a String...
        {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf); // Envia a mensagem para o Terminal (Informacao).
        }
        free(buf); // Libera memoria alocada (Conserva recursos).
    }
    buf_len = httpd_req_get_url_query_len(req) + 1; // Obtem o comprimento da string do cabecalho, aloca memoria +1, para terminacao nula.
    if (buf_len > 1)                                // Se houver dados no Buffer...
    {
        buf = (char *)malloc(buf_len);                                // Aloca os dados conforme tamanho no Buffer.
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) // Se encontrar a String...
        {
            ESP_LOGI(TAG, "Found URL query => %s", buf);                                   // Envia a mensagem para o Terminal (Informacao).
            char param[32];                                                                // Aloca memoria para os parametros de recepcao.
            char vlrSaida[4];                                                              // Reserva memoria para a saida.
            if (httpd_query_key_value(buf, "saida", vlrSaida, sizeof(vlrSaida)) == ESP_OK) // Verifica o valor da Query, se for esta...
            {
                ESP_LOGI(TAG, "Parametro Saida= %s", vlrSaida); // Envia a mensagem para o Terminal (Informacao).
                vlrQuery1 = atol(vlrSaida);                     // Salva valor convertido (ASCII->INT).
            }

            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) // Verifica o valor da Query, se for esta...
            {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param); // Envia a mensagem para o Terminal (Informacao).
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) // Verifica o valor da Query, se for esta...
            {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param); // Envia a mensagem para o Terminal (Informacao).
            }
        }
        free(buf); // Libera memoria alocada (Conserva recursos).
    }
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1"); // Define alguns cabecalhos personalizados.
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2"); // Define alguns cabecalhos personalizados.
    const char *resp_str = (const char *)req->user_ctx;           // Define o contexto do usuario.
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);        // Envia a resposta com cabecalhos personalizados e corpo definido como a string.
    /**
     * @note Depois de enviar a resposta HTTP, os antigos cabecalhos de solicitacao HTTP sao perdidos.
     * */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) // Se o tamanho for zero, dados perdidos...
    {
        ESP_LOGI(TAG, "Request headers lost"); // Envia a mensagem para o Terminal (Informacao).
    }
    return (ESP_OK); // Retorna valor 'Ok'.
}

/**
 * @brief   Cria a estrutura da URI.
 * @note    Metodo GET. Uso no GPIO.
 * @return  Valor ASCII das entradas.
 */
static const httpd_uri_t gpio =
    {
        .uri = "/gpio",              // Caminho da URI.
        .method = HTTP_GET,          // Metodo utilizado.
        .handler = gpio_get_handler, // Nome do manipulador a ser usado.
        .user_ctx = vlrAscIn         // Retorno ao cliente.
};

/**
 * @brief   Inicializa o Servidor Web Basico.
 * @note    Se ativo, registra a(s) URI(s).
 * @return  O 'Status' se 'Ok' ou 'Nulo'.
 */
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;                                           // Configura manipulador do servidor como 'Nulo'.
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();                         // Cria instancia da estrutura basica de configuracao.
    config.lru_purge_enable = true;                                         // Desconecta clientes sem conexao automaticamente.
    config.max_uri_handlers = 10;                                           // Numero de URIs que podem ser registradas (ate 10).
    ESP_LOGI(TAG, "Iniciando o Server na porta: '%d'", config.server_port); // Envia mensagem com dado para o Terminal.
    if (httpd_start(&server, &config) == ESP_OK)                            // Se o servidor foi configurado e esta ativo...
    {
        httpd_register_uri_handler(server, &gpio);
        
        ESP_LOGI(TAG, "URIs Registradas."); // Envia mensagem para o Terminal.
        return (server);                    // Retorna valor 'Status' do servidor.
    }
    ESP_LOGI(TAG, "Falha ao iniciar o Server!"); // Envia mensagem para o Terminal.
    return (NULL);                               // Retorna valor 'Nulo' ou 'Vazio'.
}

/**
 * @brief   Sub Programa de Inicializacao do Servidor Web.
 */
void wifi_init(void)
{
    nvs_init();      // Inicializa a NVS RAM.
    wifi_init_sta(); // Inicializa o WiFi no modo Station.
    // wifi_init_softap();
    start_webserver(); // Inicializa o Servidor Web.
}

void printIp(char lin)
{
    char vlr[4];

    int2Asc(ipPos1, vlr, 3);
    lcdTexto(vlr, lin, 1);
    lcdCaracter('.', lin, 4);

    int2Asc(ipPos2, vlr, 3);
    lcdTexto(vlr, lin, 5);
    lcdCaracter('.', lin, 8);

    int2Asc(ipPos3, vlr, 3);
    lcdTexto(vlr, lin, 9);
    lcdCaracter('.', lin, 12);

    int2Asc(ipPos4, vlr, 3);
    lcdTexto(vlr, lin, 13);
    lcdCaracter(' ', lin, 16);
}

#endif
