
#include "luat_base.h"
#include "luat_pwm.h"
#include "luat_log.h"
#include "hal_pwm.h"

//#ifdef SOC_FAMILY_STM32
#if 0
//------------------------------------------------------
// 在 RTT， rt_device_pwm 实际上是 stm32_pwm 结构的第一个属性
// 因此，暗戳戳的转成 (stm32_pwm *) 即可得到 channel



#else
//------------------------------------------------------
// 在 RTT， 用两个数字来确定 pwm 的 channel
// {I}{N}
//  - I: 表示在 pwm_devs 中的下标
//  - N: 表示在该设备的 channel
// 这两个数字通过 @channel 参数得到
//  - I: 十位
//  - N: 个位
// @return -1 打开失败。 0 打开成功
int luat_pwm_open(int channel, size_t period, size_t pulse) {
    if (period < 1 || period > 1000000)
        return -1;
    if (pulse > 100)
        pulse = 100;
    
    hal_pwm_duty_set(channel, period * pulse / 100, period);
    hal_pwm_freq_update(channel, period);
    hal_pwm_start(channel);
    return 0;
}

// @return -1 关闭失败。 0 关闭成功
int luat_pwm_close(int channel) {
    hal_pwm_stop(channel);
    return 0;
}

#endif
//------------------------------------------------------
