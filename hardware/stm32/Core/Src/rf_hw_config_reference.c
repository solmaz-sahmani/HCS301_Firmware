/*
 * Hardware configuration reference for STM32G431RB.
 *
 * RF receiver DATA -> PA6 / TIM3_CH1
 *
 * Configure TIM3:
 *   Prescaler   : selected so timer counter = 1 MHz
 *   Counter     : Up
 *   Period      : 0xFFFFFFFF if available, otherwise maximum timer period
 *   Channel 1   : Input Capture
 *   Polarity    : Both Edges
 *   Prescaler   : DIV1
 *   Filter      : small digital filter if the receiver is noisy
 *
 * Enable:
 *   TIM3 global interrupt
 *
 * In stm32g4xx_it.c:
 *
 * void TIM3_IRQHandler(void)
 * {
 *     HAL_TIM_IRQHandler(&htim3);
 * }
 *
 * PA6 must be configured as TIM3_CH1 alternate function.
 */
