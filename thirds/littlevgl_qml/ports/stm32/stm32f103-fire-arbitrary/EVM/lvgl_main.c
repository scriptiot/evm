#include "lvgl_main.h"

extern void ecma_timer_poll(evm_t * e);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];                     /*Declare a buffer for 10 lines*/

void my_disp_flush(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
			LCD_Fast_DrawPoint(x, y, color_p->full);
            color_p++;
        }
    }
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}


void evm_lvgl_tick_inc(int x){
    lv_tick_inc(x);
}


bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
	static strType_XPT2046_Coordinate cinfo={-1,-1,-1,-1};

    /*Save the state and save the pressed coordinate*/
	if(XPT2046_TouchDetect() == TOUCH_PRESSED){
		data->state = LV_INDEV_STATE_PR;
		XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
		cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;
	} else {
		data->state = LV_INDEV_STATE_REL;
		cinfo.x = -1;
		cinfo.y = -1; 
		cinfo.pre_x = -1;
		cinfo.pre_y = -1;
	}

    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = cinfo.x;
    data->point.y = cinfo.y;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

typedef  FIL pc_file_t;
static lv_fs_res_t pcfs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode);
static lv_fs_res_t pcfs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t pcfs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t pcfs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos);
static lv_fs_res_t pcfs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

extern int modules_paths_count;
extern char* modules_paths[];

FIL lvgl_file;

static lv_fs_res_t pcfs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode)
{
    (void) drv; /*Unused*/

    const char * flags = "";

	char buf[128];
	sprintf(buf,  "0:/%s", fn);
	FRESULT res = f_open(&lvgl_file, buf, FA_READ | FA_OPEN_EXISTING);
    if(res != FR_OK) return LV_FS_RES_UNKNOWN;
    else {
		pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
        *fp = lvgl_file;
    }

    return LV_FS_RES_OK;
}


/**
 * Close an opened file
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_close(lv_fs_drv_t * drv, void * file_p)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    f_close(&lvgl_file);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    f_read(&lvgl_file, buf, btr, br);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/	
	f_lseek(&lvgl_file, pos);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void) drv; /*Unused*/
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *pos_p = (uint32_t)f_tell(&lvgl_file);
    return LV_FS_RES_OK;
}

void lvgl_image_driver_init(){
    lv_fs_drv_t pcfs_drv;                         /*A driver descriptor*/
    memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t));    /*Initialization*/

    pcfs_drv.file_size = sizeof(pc_file_t);       /*Set up fields...*/
    pcfs_drv.letter = 'P';
    pcfs_drv.open_cb = pcfs_open;
    pcfs_drv.close_cb = pcfs_close;
    pcfs_drv.read_cb = pcfs_read;
    pcfs_drv.seek_cb = pcfs_seek;
    pcfs_drv.tell_cb = pcfs_tell;
    lv_fs_drv_register(&pcfs_drv);
}


void lvgl_main(void)
{
    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
    disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/

    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

    lvgl_image_driver_init();
}

void lvgl_loop(evm_t * env)
{
    while(1){
        ecma_timer_poll(env);
        delay_ms(1);
        evm_lvgl_tick_inc(1);
        lv_task_handler();
    }
}
