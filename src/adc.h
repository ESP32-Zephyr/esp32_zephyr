/****************************************************************************
 * **File:** adc.h
 *
 * **Description:** ADC interface definitions and functions.
 *
 * This header defines types and the interface for ADC (Analog to Digital Converter) module.
 ****************************************************************************/

#ifndef ADC_H
#define ADC_H

/******************************** INCLUDE FILES *******************************/

/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/

/**
 * Function pointer type for functions with signature: int func(void).
 */
typedef int (*IntVoid_t)(void);

/**
 * Function pointer type for functions with signature: int func(int, int32_t*).
 */
typedef int (*IntIntInt32_t)(int, int32_t*);

/**
 * ADC module abstraction struct.
 *
 * Provides function pointers to initialize the ADC, check initialization status,
 * get the number of channels and read ADC channel value.
 */
typedef struct _adc_t {
    IntVoid_t init;          /**< Function pointer to initialize ADC, returns int status */
    bool init_done;          /**< Flag indicating if initialization is complete */
    IntVoid_t channels_n_get;/**< Function pointer returning the number of ADC channels */
    IntIntInt32_t ch_read;   /**< Function pointer to read an ADC channel: params (channel, pointer to store value), returns int status */
} adc_t;

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * Get a pointer to the ADC interface instance.
 *
 * Provides access to the ADC abstraction functions and state.
 *
 * @return Pointer to an adc_t struct representing the ADC interface.
 */
adc_t *adc_get(void);

#endif /* ADC_H */
