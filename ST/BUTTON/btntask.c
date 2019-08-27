/* Includes ------------------------------------------------------------------*/
#include <lib_mem.h>
#include "button.h"
#include "bsp_pb.h"
#include "ipport.h"

/* Private typedef -----------------------------------------------------------*/
#ifdef BUTTON
Button_t Button;

/* Private function prototypes -----------------------------------------------*/
uint8_t Read_Btn1_Level(void);
void Btn1_Dowm_CallBack(void *btn);
void Btn1_Double_CallBack(void *btn);
void Btn1_Long_CallBack(void *btn);

/**
  * @brief  BtnTask
  * @param  thread not used
  * @retval None
  */
void BtnTask(void *p_arg)
{
  (void)p_arg;

  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
  Button_Create("Button", &Button, Read_Btn1_Level, GPIO_PIN_SET);
  Button_Attach(&Button, BUTTON_DOWM, Btn1_Dowm_CallBack);     //单击
  Button_Attach(&Button, BUTTON_DOUBLE, Btn1_Double_CallBack); //双击
  Button_Attach(&Button, BUTTON_LONG, Btn1_Long_CallBack);     //长按

  Get_Button_Event(&Button);

  while (DEF_TRUE)
  { /* Task body, always written as an infinite loop.       */
    Button_Process();
    osDelayTask(40);
  }
}

/**
  * @brief  Read_Btn1_Level
  * @param  thread not used
  * @retval None
  */
uint8_t Read_Btn1_Level(void)
{
  return BSP_PB_GetState(BUTTON_USER);
}

/**
  * @brief  Btn1_Dowm_CallBack
  * @param  thread not used
  * @retval None
  */
void Btn1_Dowm_CallBack(void *btn)
{
  TRACE_INFO("Button Single click!\n");
}

/**
  * @brief  Btn1_Double_CallBack
  * @param  thread not used
  * @retval None
  */
void Btn1_Double_CallBack(void *btn)
{
  TRACE_INFO("Button double click!\n");
}

/**
  * @brief  Btn1_Long_CallBack
  * @param  thread not used
  * @retval None
  */
void Btn1_Long_CallBack(void *btn)
{
  TRACE_INFO("Button Long press!\n");
}
#endif
