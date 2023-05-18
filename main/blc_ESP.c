#include <stdio.h>
#include "geralv2r1.h"
#include "config.h"
#include "wifiv1r1.h"

#define CONFIG_METER1_GPIO GPIO_NUM_36

adc_channel_t gpio2adc(int gpio) {
#if CONFIG_IDF_TARGET_ESP32
	if (gpio == 32) return ADC1_GPIO32_CHANNEL;
	if (gpio == 33) return ADC1_GPIO33_CHANNEL;
	if (gpio == 34) return ADC1_GPIO34_CHANNEL;
	if (gpio == 35) return ADC1_GPIO35_CHANNEL;
	if (gpio == 36) return ADC1_GPIO36_CHANNEL;
	if (gpio == 37) return ADC1_GPIO37_CHANNEL;
	if (gpio == 38) return ADC1_GPIO38_CHANNEL;
	if (gpio == 39) return ADC1_GPIO39_CHANNEL;

#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
	if (gpio == 1) return ADC1_GPIO1_CHANNEL;
	if (gpio == 2) return ADC1_GPIO2_CHANNEL;
	if (gpio == 3) return ADC1_GPIO3_CHANNEL;
	if (gpio == 4) return ADC1_GPIO4_CHANNEL;
	if (gpio == 5) return ADC1_GPIO5_CHANNEL;
	if (gpio == 6) return ADC1_GPIO6_CHANNEL;
	if (gpio == 7) return ADC1_GPIO7_CHANNEL;
	if (gpio == 8) return ADC1_GPIO8_CHANNEL;
	if (gpio == 9) return ADC1_GPIO9_CHANNEL;
	if (gpio == 10) return ADC1_GPIO10_CHANNEL;

#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
	if (gpio == 0) return ADC1_GPIO0_CHANNEL;
	if (gpio == 1) return ADC1_GPIO1_CHANNEL;
	if (gpio == 2) return ADC1_GPIO2_CHANNEL;
	if (gpio == 3) return ADC1_GPIO3_CHANNEL;
	if (gpio == 4) return ADC1_GPIO4_CHANNEL;

#endif
	return -1;
}

static bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle) // Função para inicializar a calibração do ADC
{
	adc_cali_handle_t handle = NULL; // Handle de calibração
	esp_err_t ret = ESP_FAIL; // Código de retorno
	bool calibrated = false; // Indica se a calibração foi bem-sucedida

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED // Se o esquema de calibração "Curve Fitting" é suportado
	if (!calibrated) { // Se ainda não foi calibrado
		ESP_LOGI(TAG, "a versão do esquema de calibração é %s", "Curve Fitting"); // Registra o esquema de calibração usado
		adc_cali_curve_fitting_config_t cali_config = { // Configuração do esquema de calibração "Curve Fitting"
			.unit_id = unit,
			.atten = atten,
			.bitwidth = ADC_BITWIDTH_DEFAULT,
		};
		ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle); // Cria o esquema de calibração "Curve Fitting"
		if (ret == ESP_OK) { // Se a criação foi bem-sucedida
			calibrated = true; // Indica que a calibração foi bem-sucedida
		}
	}
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED // Se o esquema de calibração "Line Fitting" é suportado
	if (!calibrated) { // Se ainda não foi calibrado
		ESP_LOGI(TAG, "a versão do esquema de calibração é %s", "Line Fitting"); // Registra o esquema de calibração usado
		adc_cali_line_fitting_config_t cali_config = { // Configuração do esquema de calibração "Line Fitting"
			.unit_id = unit,
			.atten = atten,
			.bitwidth = ADC_BITWIDTH_DEFAULT,
		};
		ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle); // Cria o esquema de calibração "Line Fitting"
		if (ret == ESP_OK) { // Se a criação foi bem-sucedida
			calibrated = true; // Indica que a calibração foi bem-sucedida
		}
	}
#endif

	*out_handle = handle; // Armazena o handle de calibração em out_handle
	if (ret == ESP_OK) { // Se a criação do esquema de calibração foi bem-sucedida
		ESP_LOGI(TAG, "Calibrou com sucesso!"); // Registra que a calibração foi bem-sucedida
	} else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {ESP_LOGW(TAG, "eFuse não queimado, pule a calibração do software");   // Se a criação do esquema de calibração falhou ou não foi possível calibrar
	} else {
		ESP_LOGE(TAG, "argumento invalido ou nao tem memoria");
	}

	return calibrated;
} 


// ADC1 Calibration Init
// adc_cali_handle_t adc1_cali_handle = NULL; // Handle de calibração do ADC1
// bool do_calibration = adc_calibration_init(ADC_UNIT_1, ADC_ATTEN_DB_11, &adc1_cali_handle); // Tenta calibrar o ADC1
// if (do_calibration == false) { // Se a calibração falhar
// 	ESP_LOGE(TAG, "calibration fail"); // Registra um erro
// 	vTaskDelete(NULL); // Exclui a tarefa atual
// }

// // ADC1 Init
// adc_oneshot_unit_handle_t adc1_handle; // Handle do ADC1
// adc_oneshot_unit_init_cfg_t init_config1 = { // Configuração de inicialização do ADC1
// 	.unit_id = ADC_UNIT_1,
// };
// ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle)); // Inicializa o ADC1

// // ADC1 config
// adc_oneshot_chan_cfg_t config = { // Configuração do canal do ADC1
// 	.bitwidth = ADC_BITWIDTH_DEFAULT,
// 	.atten = ADC_ATTEN_DB_11,
// };
// adc_channel_t adc1_channel1 = gpio2adc(CONFIG_METER1_GPIO); // Converte o número do pino GPIO em um canal ADC
// ESP_LOGI(TAG, "CONFIG_METER1_GPIO=%d adc1_channel1=%d", CONFIG_METER1_GPIO, adc1_channel1); // Registra o número do pino GPIO e o canal ADC correspondente
// ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, adc1_channel1, &config)); // Configura o canal do ADC1



void app_main(void)
{
 
        // ADC1 Calibration Init
    adc_cali_handle_t adc1_cali_handle = NULL;   // Handle de calibração do ADC1
    bool do_calibration = adc_calibration_init(ADC_UNIT_1, ADC_ATTEN_DB_11, &adc1_cali_handle);// Tenta calibrar o ADC1
    if (do_calibration == false) {       // Se a calibração falhar
        ESP_LOGE(TAG, "calibracao falhou");  // Registra um erro
        vTaskDelete(NULL);          // Exclui a tarefa atual
    }

    // ADC1 Init
    adc_oneshot_unit_handle_t adc1_handle;  // Handle do ADC1
    adc_oneshot_unit_init_cfg_t init_config1 = {         // Configuração de inicialização do ADC1
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));      // Inicializa o ADC1


    // ADC1 config
    adc_oneshot_chan_cfg_t config = {       // Configuração do canal do ADC1
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_channel_t adc1_channel_t = gpio2adc(CONFIG_METER1_GPIO);     // Converte o número do pino GPIO em um canal ADC
    ESP_LOGI(TAG, "CONFIG_METER1_GPIO=%d ADC1_GPIO36_CHANNEL=%d", CONFIG_METER1_GPIO, ADC1_GPIO36_CHANNEL);      // Registra o número do pino GPIO e o canal ADC correspondente
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_GPIO36_CHANNEL, &config));    // Configura o canal do ADC1

    // Read ADC value
    uint32_t valorBin;  // Valor lido do ADC
    uint32_t gramas;    // Massa convertida em gramas
    uint32_t TARA;      // Valor de tara
    uint32_t converteMassa;     // Valor convertido

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_GPIO36_CHANNEL, &TARA));   // Lê o valor de tara do ADC

    while(1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_GPIO36_CHANNEL, &valorBin));   // Lê o valor do ADC
        converteMassa = valorBin - TARA;        // Subtrai o valor de tara
        gramas = converteMassa * 9.77517;        // Converte o valor em gramas usando uma constante de conversão
        printf("Massa: %ld g\n", gramas);        // Imprime a massa convertida na saída padrão
        vTaskDelay(pdMS_TO_TICKS(1000));
    }


}
