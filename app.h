#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAIN_PRIORITY  5
#define INIT_PRIORITY  6	
//#define TRACER_PRIORITY  6	
#define LOGGER_PRIORITY 6

/* タスク周期の定義 */
//#define LINE_TRACER_PERIOD  (100 * 1000) /* ライントレースタスク:100msec周期 */
#define LOGGER_PERIOD  (100 * 1000) /* ログタスク:1sec周期 */

#ifndef STACK_SIZE
#define STACK_SIZE      4096
#endif /* STACK_SIZE */

#ifndef TOPPERS_MACRO_ONLY

void main_task(intptr_t exinf);
void logger_task(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
