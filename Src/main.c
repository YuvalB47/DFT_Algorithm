#include <stdio.h>
#include <complex.h>
#include "STM32L476xx.h"
#include "signals.h"
#include "uart.h"
#include "arm_math.h"
#include "systick.h"


#define OFFSET1		5
#define OFFSET2		-20

complex float X[HZ_5_SIG_LEN];
float spectrum[HZ_5_SIG_LEN];  // Magnitude spectrum

extern float _5hz_signal[HZ_5_SIG_LEN];

float g_in_sig_sample;
float g_imp_rsp_sample;

uint32_t g_before, g_after, g_time_taken;
float g_secounds,g_milisecounds_func;
const float SINGLE_CYCLE = 0.00000025;
const int SEC_TO_MSEC = 1000;

static void pseudo_dly(int dly);
static void fpu_enable(void);

void serial_plot_all(void);
void serial_plot_x(void);
void calculate_spectrum(void);
void calculate_dft(void);

int main()
{
	/*Enable FPU */
	fpu_enable();

	/*Initialize the uart*/
	uart2_tx_init();

	/*Initialize systick counter*/
	systick_counter_init();

	g_before = SysTick->VAL;
	calculate_dft();
	calculate_spectrum();
	g_after = SysTick->VAL;
	serial_plot_x();

	/*compute time taken*/
	g_time_taken = g_before - g_after;

	g_secounds = g_time_taken * SINGLE_CYCLE;
	g_milisecounds_func = g_secounds * SEC_TO_MSEC;

    while(1)
    {
    }
}

void serial_plot_x(void)
{
	for(int j = 0; j < HZ_5_SIG_LEN; j++)
	{
		printf("%f,",OFFSET2 + _5hz_signal[j]);
		printf("%f\r\n", spectrum[j]);
		pseudo_dly(9000);

	}
}

void calculate_spectrum(void)
{
    for (int k = 0; k < HZ_5_SIG_LEN; ++k) {
        spectrum[k] = cabs(X[k]);
    }
}

void calculate_dft(void)
{
    for (int k = 0; k < HZ_5_SIG_LEN; ++k) {
        X[k] = 0;

        for (int n = 0; n < HZ_5_SIG_LEN; ++n) {
            X[k] += _5hz_signal[n] * cexp(-I * 2 * M_PI * k * n / HZ_5_SIG_LEN);
        }
    }
}


static void pseudo_dly(int dly)
{
	for(int i =0 ; i < dly; i++){}
}

static void fpu_enable(void)
{
	/*Enable FPU */
	SCB->CPACR |=((3UL << 10*2) | (3UL << 11*2));

}
