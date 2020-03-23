#include "string.h"
#include "stdlib.h"


#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

 //文件类型列表
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES","SMS"},		//NES/SMS文件
{"TXT","C","H"},	//文本文件
{"WAV","MP3","APE","FLAC"},//支持的音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
{"AVI"},			//视频文件
};
///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
FIL *file;	  		//文件1
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 *fatbuf;			//SD卡数据缓存区


//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;//错误的字符串.
 	for(i=0;i<5;i++)//得到后缀名
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}		   

//得到路径下的文件夹
//返回值:0,路径就是个卷标号.
//    其他,文件夹名字首地址
u8* exf_get_src_dname(u8* dpfn)
{
	u16 temp=0;
 	while(*dpfn!=0)
	{
		dpfn++;
		temp++;	
	}
	if(temp<4)return 0; 
	while((*dpfn!=0x5c)&&(*dpfn!=0x2f))dpfn--;	//追述到倒数第一个"\"或者"/"处 
	return ++dpfn;
}
//得到文件夹大小
//注意文件夹大小不要超过4GB.
//返回值:0,文件夹大小为0,或者读取过程中发生了错误.
//    其他,文件夹大小.
u32 exf_fdsize(u8 *fdname)
{
#define MAX_PATHNAME_DEPTH	512+1	//最大目标文件路径+文件名深度
	u8 res=0;	  
    DIR *fddir=0;		//目录
	FILINFO *finfo=0;	//文件信息
	u8 * pathname=0;	//目标文件夹路径+文件名
 	u16 pathlen=0;		//目标路径长度
	u32 fdsize=0;

	fddir=(DIR*)malloc(sizeof(DIR));//申请内存
 	finfo=(FILINFO*)malloc(sizeof(FILINFO));
   	if(fddir==NULL||finfo==NULL)res=100;
	if(res==0)
	{ 
 		pathname=(u8*)malloc(MAX_PATHNAME_DEPTH);	    
 		if(pathname==NULL)res=101;	   
 		if(res==0)
		{
			pathname[0]=0;	    
			strcat((char*)pathname,(const char*)fdname); //复制路径	
		    res=f_opendir(fddir,(const TCHAR*)fdname); 		//打开源目录
		    if(res==0)//打开目录成功 
			{														   
				while(res==0)//开始复制文件夹里面的东东
				{
			        res=f_readdir(fddir,finfo);						//读取目录下的一个文件
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//错误了/到末尾了,退出
			        if(finfo->fname[0]=='.')continue;     			//忽略上级目录
					if(finfo->fattrib&0X10)//是子目录(文件属性,0X20,归档文件;0X10,子目录;)
					{
 						pathlen=strlen((const char*)pathname);		//得到当前路径的长度
						strcat((char*)pathname,(const char*)"/");	//加斜杠
						strcat((char*)pathname,(const char*)finfo->fname);	//源路径加上子目录名字
 						//printf("\r\nsub folder:%s\r\n",pathname);	//打印子目录名
						fdsize+=exf_fdsize(pathname);				//得到子目录大小,递归调用
						pathname[pathlen]=0;						//加入结束符
					}else fdsize+=finfo->fsize;						//非目录,直接加上文件的大小
						
				} 
		    }	  
  			free(pathname);	     
		}
 	}
	free(fddir);    
	free(finfo);
	if(res)return 0;
	else return fdsize;
}	  







