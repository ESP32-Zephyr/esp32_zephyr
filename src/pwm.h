#ifndef PWM_H
#define PWM_H

/******************************** INCLUDE FILES *******************************/

/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/
typedef int (*IntVoid_t)(void);
typedef int (*IntInt2xUint32_t)(int, uint32_t, uint32_t);
typedef int (*IntInt2xUint32Ptr_t)(int, uint32_t *, uint32_t *);

/**
 * @brief Structure representing PWM interface and state.
 *
 * Contains function pointers for initialization and duty cycle manipulation,
 * as well as an initialization completion flag.
 */
typedef struct _pwm_t {
    /**
     * @brief Pointer to the initialization function.
     *
     * Should be called before using other functions.
     */
    IntVoid_t init;

    /**
     * @brief Flag indicating if initialization is complete.
     */
    bool init_done;

    /**
     * @brief Function to set PWM duty cycle.
     *
     * @see IntInt2xUint32_t
     */
    IntInt2xUint32_t duty_cycle_set;

    /**
     * @brief Function to get PWM duty cycle.
     *
     * @see IntInt2xUint32Ptr_t
     */
    IntInt2xUint32Ptr_t duty_cycle_get;
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
