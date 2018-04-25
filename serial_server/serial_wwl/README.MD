## serial server for garden water system

### 保存传感器数据的目录格式
*   sensor_name 
    *   sensor_id
    *   yy
        *   mm
            *   dd
                *   sensor_id.log
                *   内部每一条都按照时间顺序保存为json格式

### 传感器数据保存为json格式
{
    "sensor":"name",
    "deviceId":"id",
    "key":value, //此处的value为String或者Int或者float ...
    "time":"YY:MM:DD HH:MM:SS" //该条数据记录的年月日,时分秒
}
