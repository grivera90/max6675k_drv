/******************************************************************************
* Includes
*******************************************************************************/
#include "string.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

/* log related */
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG //ESP_LOG_INFO
#include "esp_log.h"

#include "max6675k.h"
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define delay_ms(x) vTaskDelay(x / portTICK_PERIOD_MS);
#define HIGH 1
#define LOW 0
#define GPIO_LED 2

/* spi definitions */
#define PIN_NUM_MOSI 	32
#define PIN_NUM_MISO 	35
#define PIN_NUM_CLK  	16
#define PIN_NUM_CS   	17
#define SPI_DEVICE		VSPI_HOST
/******************************************************************************
* Data types
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static const char *MODULE_NAME = "[main]";
int ret = MAX6675K_OK;

/* spi variables */
spi_device_handle_t spi;

/* max6675k variables */
max6675k_t 	max6675k;
uint16_t 	temp_raw;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void gpio_setup	(void);
static int  spi_setup	(void);
static int 	spi_receive	(uint16_t *rx_data);
/******************************************************************************
* app main
*******************************************************************************/
void app_main(void)
{
	gpio_setup();

	max6675k.spi_init = spi_setup;
	max6675k.spi_read = spi_receive;

	/* Init master spi */
	max6675k.spi_init();

    while (true) {

        gpio_set_level(GPIO_LED, HIGH);
        delay_ms(500);
        gpio_set_level(GPIO_LED, LOW);
        delay_ms(500);

        ret = max6675k_readtemperature(&max6675k);
        ESP_LOGD(MODULE_NAME, "temperature raw: %d, temperature in °C: %.3f[°C]", max6675k.temp_raw, max6675k.temperature);


    }
}
/******************************************************************************
* functions definitions
*******************************************************************************/
static void gpio_setup(void)
{
    gpio_config_t io_conf;

	/* led */
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << GPIO_LED);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(GPIO_LED, HIGH);
}

static int spi_setup(void){

	esp_err_t ret;

	spi_bus_config_t buscfg =
	{
        .miso_io_num 		= 	PIN_NUM_MISO,
        .sclk_io_num 		= 	PIN_NUM_CLK,
        .quadwp_io_num 		= 	-1,
        .quadhd_io_num		= 	-1,
        .max_transfer_sz 	=	1 					// Transferencia maxima de 1 byte
    };

    spi_device_interface_config_t devcfg =
    {
    	.clock_speed_hz = 5*1000*100, 	         	// Clock out at 500 KHz (5*1000*100)
        .mode 			= 0,                        // SPI mode 0
		.queue_size 	= 7,                        // We want to be able to queue 7 transactions at a time
    };

    // Initialize the SPI bus
    ret = spi_bus_initialize(SPI_DEVICE, &buscfg, 1);
    if( ret != ESP_OK)
    	return ret;

    // Attach the ATM90E36 to the SPI bus
    ret = spi_bus_add_device(SPI_DEVICE, &devcfg, &spi);
    if( ret != ESP_OK)
        return ret;

    return ret;
}

static int spi_receive(uint16_t *rx_data)
{

	spi_transaction_t transaction;
	esp_err_t ret = ESP_OK;
	uint32_t temp = 0;

	memset(&transaction, 0, sizeof(transaction));

	transaction.length 	= 16;
	transaction.flags 	= SPI_TRANS_USE_RXDATA;
	ret 				= spi_device_polling_transmit(spi, &transaction);

	temp 		= 	*(uint16_t*)transaction.rx_data;
	*rx_data 	= 	(uint16_t)((temp & 0xFF00) >> 8);
	*rx_data 	|= 	(uint16_t)((temp & 0x00FF) << 8);

	return ret;
}


