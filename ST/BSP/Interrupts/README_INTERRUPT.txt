* In order to register your ISR handler in the RAM vector table, you will need to use the 
  BSP_IntVectSet() function.

* The handler template shown below will be use if you ISR needs to be Kernel aware, which means using
  some of the OS services in the ISR handler.

  static void MyISR_Handler(void) 
  {
      CPU_SR_ALLOC();


      CPU_CRITICAL_ENTER();
      OSIntEnter();                                               /* Tell OS that we are starting an ISR           */
      CPU_CRITICAL_EXIT();

      /* --------------- HANDLER YOUR ISR HERE --------------- */
 
      OSIntExit();                                                /* Tell OS that we are leaving the ISR           */
  }

* If your ISR handler does not need to be kernel aware then you do not need to use OSIntEnter()/OSIntExit().


      
