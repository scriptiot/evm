#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>

#define DBG_TAG           "dfs.elm"
#define DBG_LVL           DBG_DEBUG
#include <rtdbg.h>

int mnt_init(void)
{
    rt_thread_delay(RT_TICK_PER_SECOND);

    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    }
    else {
        if (dfs_mkfs("elm", "sd0") == 0) {
            rt_kprintf("file system mfks done!\n");
            if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
            {
                rt_kprintf("file system initialization done!\n");
            }
            else {
                rt_kprintf("file system initialization fail, again!\n");
            }
        }
        else {
            rt_kprintf("file system mfks fail!\n");
        }
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);

static int first_reinit = 1;
static void reinit(void* params) {
  rt_tick_t t_start;
  rt_tick_t t_end;
  if (first_reinit) {
    first_reinit = 0;
    t_start = rt_tick_get();
    // 卸载之
    dfs_unmount("/");
    // 抹除整个分区
    //wm_flash_erase(USER_ADDR_START, USER_ADDR_END - USER_ADDR_START);
    // 重新格式化
    dfs_mkfs("elfs", "spi01");
    // 挂载
    dfs_mount("spi01", "/", "lfs2", 0, 0);
    t_end = rt_tick_get();
    LOG_I("time use %dms", t_end - t_start);
  }
  LOG_I("reinit DONE!");
}
MSH_CMD_EXPORT(reinit, clean all user data);

#endif

