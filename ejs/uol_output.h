#ifndef _UOL_BINARY_H
#define _UOL_BINARY_H
extern const char uol_binary_buf[];
#define EXPORT_FlashDevice_open    4    
#define EXPORT_FlashDevice_close    8    
#define EXPORT_FlashDevice_read    12    
#define EXPORT_FlashDevice_write    16    
#define EXPORT_FlashDevice_erase    20    
#define EXPORT_FlashDevice_write_block_size    24    
#define EXPORT_PinDevice_open    9    
#define EXPORT_PinDevice_close    13    
#define EXPORT_PinDevice_read    17    
#define EXPORT_PinDevice_write    21    
#define EXPORT_PinDevice_setCallback    25    
#define EXPORT_ADCDeivce_open    6    
#define EXPORT_ADCDeivce_close    10    
#define EXPORT_ADCDeivce_read    14    
#define EXPORT_DACDevice_open    5    
#define EXPORT_DACDevice_close    9    
#define EXPORT_DACDevice_write    13    
#define EXPORT_TimerDevice_open    8    
#define EXPORT_TimerDevice_start    12    
#define EXPORT_TimerDevice_stop    16    
#define EXPORT_TimerDevice_getId    20    
#define EXPORT_WatchDogDevice_open    4    
#define EXPORT_WatchDogDevice_close    8    
#define EXPORT_WatchDogDevice_intall_timeout    12    
#define EXPORT_WatchDogDevice_setup    16    
#define EXPORT_WatchDogDevice_feed    20    
#define EXPORT_WatchDogDevice_disable    24    
#define EXPORT_main_flashCreate    40    
#define EXPORT_main_timerCreate    44    
#define EXPORT_main_pinCreate    48    
#define EXPORT_main_adcCreate    52    
#define EXPORT_main_dacCreate    56    
#define EXPORT_main_watchdogCreate    60    
#endif
