/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include <stdio.h>
#include <string.h>


/* Define ------------------------------------------------------------*/

// number of samples in average
#define NUMBER_OF_SAMPLES 100
// ADC bits
#define ADC_BITS 12
// ADC ring buffer size
#define ADC_BUF_LEN 6
// temperature sensor average slope [mV/*C]
#define AVG_SLOPE 2.5
// temperature sensor voltage at 25*C [mV]
#define V25 760
// internal reference voltage [V]
static const uint32_t VREFINT = 1210;


/* Variables ---------------------------------------------------------*/

// ADC ring buffer (1 sample for each channel)
uint16_t adc_buf[ADC_BUF_LEN];
// Output ring buffer (specified number of samples for each channel)
uint32_t out_buf[ADC_BUF_LEN * NUMBER_OF_SAMPLES];


/* Function prototypes -----------------------------------------------*/

void DMATransferComplete(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef welcome(HAL_StatusTypeDef hal_status);
HAL_StatusTypeDef goodbye(HAL_StatusTypeDef hal_status);
HAL_StatusTypeDef pampusik(HAL_StatusTypeDef hal_status);
uint16_t receive_msg(uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef send_msg(const char *msg, const char *pre_esc, const char *post_esc);
HAL_StatusTypeDef run_adc_poll(HAL_StatusTypeDef hal_status);
HAL_StatusTypeDef run_adc_poll_DMA(HAL_StatusTypeDef hal_status);
uint32_t convert2volt(uint16_t adc_value, uint16_t adc_ref);
uint32_t convert2celsius(uint16_t adc_value);


/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_usart2_tx;

/* Functions ---------------------------------------------------------*/

HAL_StatusTypeDef welcome(HAL_StatusTypeDef hal_status) {
	char msg[] = "Vitajte!";
	char pre_esc[] = "\x1b[2J\x1b[H\x1b[1m\x1b[4m\x1b[33;1m";
	char post_esc[] = "\x1b[0m";
	return send_msg(msg, pre_esc, post_esc);
}

HAL_StatusTypeDef goodbye(HAL_StatusTypeDef hal_status) {
	char msg[] = "Majte sa!";
	char pre_esc[] = "\x1b[4;0f\x1b[1m\x1b[4m\x1b[33;1m";
	char post_esc[] = "\x1b[0m\x1b[5;0f";
	return send_msg(msg, pre_esc, post_esc);
}

HAL_StatusTypeDef pampusik(HAL_StatusTypeDef hal_status) {
	uint8_t msg_ask[] = "Daj mi pampusik!";
	uint8_t msg_get[80];
	uint8_t msg_thanks[] = "Mnam!";
	uint8_t msg_stop[] = "Pche!";

	while (1) {
		HAL_UART_Transmit(&huart2, msg_ask, sizeof(msg_ask), 100);
		uint16_t msg_len = receive_msg(msg_get, sizeof(msg_get)-1);
		msg_get[msg_len] = '\0';
		if (strncmp((char*) msg_get, "s", sizeof(msg_get)) == 0) {
			return HAL_UART_Transmit(&huart2, msg_stop, sizeof(msg_stop), 100);
		}
		if (strncmp((char*) msg_get, "pampusik", sizeof(msg_get)) == 0) {
			HAL_UART_Transmit(&huart2, msg_thanks, sizeof(msg_thanks), 100);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			HAL_Delay(5000);
		}
	}
}

uint16_t receive_msg(uint8_t *pData, uint16_t Size) {
	uint16_t len = 0;
	while (Size > 0) {
		uint32_t timeout = (len == 0) ? (1000) : (10000);
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, pData, 1, timeout);
		if (status == HAL_OK) {
			HAL_UART_Transmit(&huart2, pData, 1, 10);
			pData++;
			Size--;
			len++;
			if ((pData[-1] == '\n') || (pData[-1] == '\r')) {
				HAL_UART_Transmit(&huart2, (uint8_t*) "\n", 1, 10);
				pData[-1] = '\0';
				break;
			}
		}
		else {
			break;
		}
	}
	return len;
}

HAL_StatusTypeDef send_msg(const char *msg, const char *pre_esc, const char *post_esc) {
	char message[200];
	strcpy(message, pre_esc);
	strcpy(message + strlen(message), msg);
	strcpy(message + strlen(message), post_esc);
	return HAL_UART_Transmit(&huart2, (uint8_t*) message, strlen(message), 100);
}

HAL_StatusTypeDef run_adc_poll(HAL_StatusTypeDef hal_status) {

	uint32_t adc_value;
	uint32_t volt_value;
	char adc_value_str[12];
	char volt_value_str[12];
	uint8_t msg_get[80];

	HAL_Delay(2000);

	char msg_start[] = "Prebieha meranie...";
	char msg_stop[] = "Meranie skoncilo.";
	char pre_esc[] = "\x1b[2;0f\x1b[33;1m";
	char post_esc[] = "\x1b[0m";
	send_msg(msg_start, pre_esc, post_esc);

	while (1) {
		uint16_t msg_len = receive_msg(msg_get, sizeof(msg_get)-1);
		msg_get[msg_len] = '\0';
		if (strncmp((char*) msg_get, "s", sizeof(msg_get)) == 0) {
			return send_msg(msg_stop, pre_esc, post_esc);
			break;
		}
		if (hal_status == HAL_OK)
		{
			hal_status = HAL_ADC_Start(&hadc1);
		}
		if (hal_status == HAL_OK)
		{
			hal_status = HAL_ADC_PollForConversion(&hadc1, 1000);
		}
		if (hal_status == HAL_OK) {
			adc_value = HAL_ADC_GetValue(&hadc1);
			volt_value = convert2volt(adc_value, 1000);
			snprintf(adc_value_str, sizeof(adc_value_str), "%4ld", adc_value);
			snprintf(volt_value_str, sizeof(volt_value_str), "%4ld ", volt_value);

			char message[200];
			strcpy(message, "adc_value: ");
			strcpy(message + strlen(message), adc_value_str);
			strcpy(message + strlen(message), "\t\tvolt_value: ");
			strcpy(message + strlen(message), volt_value_str);
			strcpy(message + strlen(message), "mV");
			send_msg(message, "\x1b[3;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
		}
		if (hal_status == HAL_OK) hal_status = HAL_ADC_Stop(&hadc1);
		HAL_Delay(2000);
	}
}

HAL_StatusTypeDef run_adc_poll_DMA(HAL_StatusTypeDef hal_status) {
	HAL_Delay(2000);

	char msg_start[] = "Prebieha meranie s DMA...";
	char msg_stop[] = "Meranie s DMA skoncilo.";
	char pre_esc[] = "\x1b[2;0f\x1b[33;1m";
	char post_esc[] = "\x1b[0m";
	send_msg(msg_start, pre_esc, post_esc);

	memset(out_buf, 0, sizeof(out_buf));

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_Base_Start(&htim1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN);
	}

	while (1) {
//		if (hal_status == HAL_OK) hal_status = HAL_ADC_Stop_DMA(&hadc1);
//		HAL_Delay(2000);
	}
}

uint32_t convert2volt(uint16_t adc_value, uint16_t adc_ref) {
	uint32_t out_value = ((uint32_t) adc_value * VREFINT) / (adc_ref);
	return out_value;
}

uint32_t convert2celsius(uint16_t volt_value) {
	uint32_t out_value = ((volt_value - V25) / AVG_SLOPE) + 25;
	return out_value;
}

uint32_t compute_Udiff(uint8_t channel1, uint8_t channel0) {
	return channel1 - channel0;
}

void DMATransferComplete(DMA_HandleTypeDef *hdma) {

  // Disable UART DMA mode
  huart2.Instance->CR3 &= ~USART_CR3_DMAT;

  // Toggle LD2
  // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

}

// Called when first half of buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  // HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

// Called when buffer is completely filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  uint32_t sample[6];
  for (int i=0; i<6; i++)
  {
	  sample[i] = convert2volt(adc_buf[i], adc_buf[4]);
  }
  sample[5] = convert2celsius(sample[5]);

  for (int i=0; i<6; i++)
  {
	  out_buf[i] = out_buf[i] + sample[i]/100;
  }

  char buffer[250];
  int i=0;
  sprintf(buffer, "V1: %4d mV \tV2: %4d mV \tV3: %4d mV \tV4: %4d mV \tVrefint: %4d mV \tTemp: %4d *C",
		  sample[i++], sample[i++], sample[i++], sample[i++], sample[i++], sample[i]);
  send_msg(buffer, "\x1b[3;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
}

extern "C" void multimeter_main() {
	  HAL_StatusTypeDef hal_status = HAL_OK;
	  HAL_DMA_RegisterCallback(&hdma_usart2_tx, HAL_DMA_XFER_CPLT_CB_ID,
	                           &DMATransferComplete);
	  hal_status = welcome(hal_status);
	  // hal_status = pampusik(hal_status);
	  // hal_status = run_adc_poll(hal_status);
	  hal_status = run_adc_poll_DMA(hal_status);
	  hal_status = goodbye(hal_status);
}
