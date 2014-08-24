/** Module Header **/
#include "isr_manager.h"

/** Std C Headers **/

/** BBMC Headers **/
#include "device_layer.h"
#include "system_layer.h"



/** External functions
 * 
 */
extern void isr_experiment (void);
extern void isr_goto (void);
extern void isr_rmpi (void);
extern void isr_systick (void);

extern void isr_gpio_killswitch(void);
extern void isr_gpio_pos_limit(void);



/** Internal Data Types 
 * 
 */

typedef struct
{
    unsigned int termination_flag;
    unsigned int iteration_counter;
    unsigned int termination_counter;
}
isr_state_t;



/** Internal Data
 * 
 */
static isr_state_t volatile   g_isr_state;

static isr_fp_t    const      g_isr_funcs[5] = 
                                                {
                                                    isr_experiment
                                                    isr_goto,
                                                    isr_rmpi,
                                                    isr_systick,
                                                    isr_gpio_killswitch,
                                                    isr_gpio_pos_limit,
                                                };



/** ISR state managemenent funtions 
 * 
 */

int 
isr_state_init (void)
{
    g_isr_state.termination_flag = 0;
    g_isr_state.iteration_counter = 0;
    g_isr_state.termination_counter = 0;
    
    return 0;
}

int 
isr_state_set (isr_state_field set_mode, int set_val)
{
    if(set_mode == TERM_COUNT)
    {
        g_isr_state.termination_counter = set_val;
    }
    
    else if(set_mode == TERM_FLAG)
    {
        g_isr_state.termination_flag = set_val;
    }
    
    else if(set_mode == ITER_COUNT)
    {
        g_isr_state.iteration_counter = set_val;
    }
    
    else
    {
        UARTPuts("error: contrl_state_set: set_mode argument is invalid\r\n", -1);
        return -1;
    }
    
    return 0;
}

int 
isr_state_get (isr_state_field get_mode)
{
    int ret_val = 0;
    
    if(set_mode == TERM_COUNT)
    {
        ret_val = g_isr_state.termination_counter;
    }
    
    else if(set_mode == TERM_FLAG)
    {
        ret_val = g_isr_state.termination_flag;
    }
    
    else if(set_mode == ITER_COUNT)
    {
        ret_val = g_isr_state.iteration_counter;
    }
    
    else
    {
        UARTPuts("error: contrl_state_get: get_mode argument is invalid\r\n", -1);
        return -1;
    }
    
    return ret_val;
}

int 
isr_state_print (const char* format)
{
    if (contrl_state == NULL)
    {
        UARTPuts("error: contrl_state_get: pointer argument is NULL\r\n", -1);
        return -1;
    }
    
    UARTprintf("\r\n%sController State is:\r\n", format);
    
    UARTprintf("\r\n%siteration counter is at:    %d", format, contrl_state->iteration_counter);
    UARTprintf("\r\n%stermination counter is at:  %d", format, contrl_state->termination_counter);
    UARTprintf("\r\n%stermination flag is :       %d", format, contrl_state->termination_flag);
    
    bbmc_cli_newlin(2);
    
    return 0;
}


/** ISR management functions
 * 
 */

int
isr_master_enable (void)
{
    return dev_intc_master_enable();
}

int
isr_master_disable (void)
{
    return dev_intc_master_disable();
}

int
isr_function_setup(void)
{
    return dev_intc_setup (&g_isr_funcs)
}


/** ISR return value configurer
 * 
 */

int 
isr_return_value(unsigned int cmnd_ret, int ret)
{
    if (ret == (cmnd_ret + ISR_RETURN_CLEAN))
    {
        UARTPuts("\r\nController has executed succesfully.\r\n", -1);
        return (RETURN_SUCCESS);
    }
    
    else if (ret == (cmnd_ret + ISR_RETURN_GPIO_LIM))
    {
        UARTPuts("\r\nWARNING: Controller has been terminated by position limit.\r\n", -1);
        return (RETURN_ERROR_GPIO_LIM);
    }
    
    else if (ret == (cmnd_ret + ISR_RETURN_KILLSW))
    {
        UARTPuts("\r\nWARNING: Controller has been terminated by SW killswitch.\r\n", -1);
        return (RETURN_ERROR_GPIO_KILL);
    }
    
    else if (ret == (cmnd_ret + ISR_RETURN_DEBUG))
    {
        UARTPuts("\r\nController has executed DEBUG (NULL) procedure.\r\n", -1);
        return (RETURN_DEBUG);
    }
    
    else
    {
        UARTPuts("\r\nwarning: execution has been terminated by unknown event", -1);
        UARTprintf("\r\nreturn value is: %d\r\n", ret);
        return (RETURN_ERROR_UNKNOWN);
    }
    
    return 0;
}








