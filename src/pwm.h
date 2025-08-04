/****************************************************************************
 * **File:** pwm.h
 *
 * **Description:** PWM interface definitions and functions.
 *
 * This header defines types and the interface for PWM module.
 ****************************************************************************/

#ifndef PWM_H
#define PWM_H

/******************************** INCLUDE FILES *******************************/

/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/
typedef int (*IntVoid_t)(void);
typedef int (*IntInt2xUint32_t)(int, uint32_t, uint32_t);
typedef int (*IntInt2xUint32Ptr_t)(int, uint32_t *, uint32_t *);
typedef void (*Void2xUint32Ptr_t)(uint32_t *, uint32_t *);

/**
 * @brief Structure representing PWM interface and state.
 *
 * Contains function pointers for initialization and duty cycle manipulation,
 * as well as an initialization completion flag.
 */
typedef struct _pwm_t {
    IntVoid_t init; /**< Pointer to the initialization function. */
    IntVoid_t channels_n_get; /**< Function pointer returning the number of ADC channels */
    bool init_done; /**< Flag indicating if initialization is complete.*/
    IntInt2xUint32_t duty_cycle_set; /**< Function to set PWM duty cycle. */
    IntInt2xUint32Ptr_t duty_cycle_get; /**< Function to get PWM duty cycle. */
    Void2xUint32Ptr_t periods_interval_get; /**< Retrieve max/min period intervals in ns */
} pwm_t;


/***************************** INTERFACE FUNCTIONS ****************************/
/**
 * @brief Retrieve the PWM interface instance.
 *
 * Returns a pointer to a static or singleton PWM structure.
 *
 * @return pwm_t* Pointer to the PWM interface.
 */
pwm_t *pwm_get(void);

#endif /* PWM_H */
