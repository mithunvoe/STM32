#include <sys_err.h>
#include <kstdio.h>
#include <sys_bus_matrix.h>
#include <types.h>

void Error_Handler(Data_TypeDef *errHd)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  //kprintf("%s\n" get_can_err_message());
  /* USER CODE END Error_Handler_Debug */
  switch (errHd->p_type_t)
  {
  case /* constant-expression */ SYS_CAN_t:
    /* code */
    break;
  
  default:
    break;
  }
  while(1)
  {

  }
}

