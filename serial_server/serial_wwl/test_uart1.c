/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "ms_hw_serial.h"
#include "json.h"
#include "raspberry_gpio_op.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct {
    int soil_moisture;
}soil_moisture_sensor;

/* Private define ------------------------------------------------------------*/
#define flag_dbg(format...)                          \
    {                                                \
        printf("%s -- %d:", __FUNCTION__, __LINE__); \
        printf("\e[1;31m");                          \
        printf(format);                              \
        printf(" \e[0m");                            \
    }
#define PUMP_WATER_GPIO 4
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* global variables ----------------------------------------------------------*/
sem_t sem_pump_water;
/* Private functions ---------------------------------------------------------*/

/*
 *  thread for pump water 
 * */
void *th_pump(void *args) {
    while(1)  {
        sem_wait(&sem_pump_water);
        GPIOWrite(PUMP_WATER_GPIO, HIGH);
        sleep(2);
        GPIOWrite(PUMP_WATER_GPIO, LOW);
    }
}
/*
 *  将数据保存到文件
 *  目前每一条直接保存
 *  之后改为缓存10K或者更多进行sync到文件
 * */
    int CreateDir(const char *sPathName)      
    {      
       char   DirName[256];      
       strcpy(DirName,   sPathName);      
       int   i,len   =   strlen(DirName);      
       if(DirName[len-1]!='/')      
       strcat(DirName,   "/");        
       len   =   strlen(DirName);       
       for(i=1;   i<len;   i++)      
       {      
          if(DirName[i]=='/')      
           {      
              DirName[i]   =   0;      
              if( access(DirName,   NULL)!=0   )      
               {      
                   if(mkdir(DirName,   0755)==-1)      
                     {       
                          perror("mkdir   error");       
                          return   -1;       
                     }      
               }      
              DirName[i]   =   '/';      
           }      
        }      
        return   0;      
     }       
int save_log_to_file(char *file_path, char *file_name, unsigned char *data, int length) {
    //std::string file_path1 = file_path;
    struct tm     *ptm;
    long ts;
    int y,m,d,h,n,s;

    ts = time(NULL);
    ptm = localtime(&ts);
    y = ptm->tm_year+1900;//年
    m = ptm->tm_mon+1;    //月
    d = ptm->tm_mday;     //日
    h = ptm->tm_hour;     //时
    n = ptm->tm_min;      //分
    s = ptm->tm_sec;      //秒
    char file_path1[256] = {0};
    sprintf(file_path1, "%s/%d/%d/", file_path, y, m);
//    file_path1 = file_path1 + "/" + y +"/"+m+"/";
    if(0 != access(file_path1, F_OK)) {
       CreateDir(file_path1); 
    }
    //file_path1 = file_path1 + file_name + "_" +d+".log";
    int offset = strlen(file_path1);
    sprintf(file_path1+offset,"%s_%d.log", file_name, d);
    printf("%s\n", file_path1);
    FILE *fp = fopen(file_path1, "a+");
    assert(fp);
    fwrite(data, length, 1, fp);
    fflush(fp);
    fclose(fp);

    return 0;
}
static int ms_fw_uart_init(tty_info_t **ptty1, unsigned int rate, char *port)
{
	*ptty1 = ms_hal_hw_uart_ready(port);// 1

	if(*ptty1 == NULL)
	{
		printf("ready_tty(0) error\n");
		return 1;
	}

	if(ms_hal_hw_uart_set_speed(*ptty1, rate, 0)>0)
	{
		printf("set_tty_speed() error\n");
		return -1;
	}
	
	if(ms_hal_hw_uart_set_parity(*ptty1,8,'N',1)>0)
	{
		printf("set_tty_parity() error\n");
		return -1;
	}
  
	
	return 0;
}
void usage(void) {
    printf(".eg:\n./serial 115200 ttyUSB0\n");
}

void init() {
    int ret = 0;
    ret = sem_init(&sem_pump_water, 0, 0);
    assert(0 == ret); 
    GPIOExport(PUMP_WATER_GPIO);
    GPIODirection(PUMP_WATER_GPIO, OUT);
    char *sensor_log_dir = "sensor_log";
    if(0 != access(sensor_log_dir, F_OK)) {
        ret = mkdir(sensor_log_dir, 0777);
        assert(0 == ret);
    }
}
int main(int argc , char **argv)
{
    static tty_info_t* sg_uart_handle;
	int ret = 0, i = 0;
	unsigned char b[2048] = {0};
    unsigned int timeout_ms = 100;
    Json::Value root;
    Json::Reader reader;

    soil_moisture_sensor soil_moisture1;
    if(argc < 2) {
        usage();
        return -1;
    }
	if(ms_fw_uart_init(&sg_uart_handle, atoi(argv[1]), argv[2]) != 0){
		printf("uart1 initialize failed!\n");
		return -1;
	}
    init();
    pthread_t thread_pump;
    ret = pthread_create(&thread_pump, NULL, th_pump, NULL);
    assert(ret == 0);
	while(1)
	{
        memset(b, 0, sizeof(b));
		ret = ms_hal_hw_uart_read(sg_uart_handle, b, sizeof(b), timeout_ms);
        if (ret > 0){
            printf("%s\r\n", b);
            /*
             *  对接收的json数据进行解析
             * */
            char *ptr = (char*)b;
            std::string buffer = ptr;
            std::cout << "buffer"<<buffer<<std::endl;

            if (!reader.parse(buffer, root, false))
            {
                flag_dbg("parse failed\n");
                //return -1;
                continue;
            }
            memset(&soil_moisture1, 0, sizeof(soil_moisture_sensor));
            soil_moisture1.soil_moisture = root["soil_moisture"].asInt();
            printf("soil_moisture value : %d\n", soil_moisture1.soil_moisture);
            if(soil_moisture1.soil_moisture > 900) {
                sem_post(&sem_pump_water);
            }
            /*
             *  保存传感器的数据
             * */
            struct tm     *ptm;
            long ts;
            int y,m,d,h,n,s;

            ts = time(NULL);
            ptm = localtime(&ts);
            y = ptm->tm_year+1900;//年
            m = ptm->tm_mon+1;    //月
            d = ptm->tm_mday;     //日
            h = ptm->tm_hour;     //时
            n = ptm->tm_min;      //分
            s = ptm->tm_sec;      //秒 
            
            char* dir_main_name = "./sensor_log/soil_moisture/soil_moisture1";
            char *file_name = "soil_moisture1";
            save_log_to_file(dir_main_name, file_name,  b, ret); 
                //getchar();
        } else {
			//ret = ms_hal_hw_uart_write(sg_uart_handle, a, strlen(a), timeout_ms);
			//printf("write number %d...\n", ret);
			//ret = 0;
		}
	}
	
	return 0;
}
