/******************************** INCLUDE FILES *******************************/

/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/
typedef int (*IntVoid_t) (void);
typedef int (*IntVoid_t) (void);
typedef int (*IntIntInt32_t) (int, int32_t*);

typedef struct _adc_t {
    IntVoid_t init;
    bool init_done;
    IntVoid_t channels_n_get;
    IntIntInt32_t ch_read;
} adc_t;

/***************************** INTERFACE FUNCTIONS ****************************/
adc_t *adc_get(void);
