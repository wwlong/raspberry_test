/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * open the door restful api
 * /api/v1/open_the_door
 */
/* Includes ------------------------------------------------------------------*/
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include "mongoose.h"
#include "raspberry_gpio_op.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UNUSED(x) (void)x
/* private constants --------------------------------------------------------*/
static const char *s_http_port = "9000";
static struct mg_serve_http_opts s_http_server_opts;
//同步信号量for sound output and GPIO control
sem_t entrance_sem_success;
sem_t entrance_sem_failed;
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void hello_world(struct mg_connection *nc, struct http_message *hm)
{
  UNUSED(hm);
    /* Send headers */
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
  mg_printf_http_chunk(nc, "{ \"result\" : \"hello_world\" }");
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

/*
 *  专门开启thread用于播放声音
 *  通过信号量同步
 *  识别成功,准备开门
 **/
void *th_prompt_tone_open_door(void *args)
{
  UNUSED(args);
  char thread_name[256] = {0};
  sprintf(thread_name, "%s", __FUNCTION__);
  prctl(PR_SET_NAME, thread_name);
  char *sound_output_cmd = "mpg321 ../sound_materials/water.mp3";
  while (1)
  {
    sem_wait(&entrance_sem_success);
    /*
     *  开门,点亮GPIO26
     * */
    GPIOExport(OPEN_DOOR_SIGNAL);
    GPIODirection(OPEN_DOOR_SIGNAL, OUT);
    GPIOWrite(OPEN_DOOR_SIGNAL, HIGH);
    /*
     *  点亮开门提示灯,点亮GPIO17
     * */
    GPIOExport(OPEN_DOOR_LED);
    GPIODirection(OPEN_DOOR_LED, OUT);
    GPIOWrite(OPEN_DOOR_LED, HIGH);
    /*
     *  开门提示音
     * */
    system(sound_output_cmd);
    /*
     *  关闭开门信号
     * */
    GPIOWrite(OPEN_DOOR_LED, LOW);
    GPIOUnexport(OPEN_DOOR_SIGNAL);
    /*
     *  关闭开门提示灯
     * */
    GPIOWrite(OPEN_DOOR_LED, LOW);
    GPIOUnexport(OPEN_DOOR_SIGNAL);
  }
}
/*
 *  专门开启thread用于播放声音
 *  通过信号量同步
 *  识别失败,报警
 **/
void *th_prompt_tone_warning(void *args)
{
  UNUSED(args);
  char thread_name[256] = {0};
  sprintf(thread_name, "%s", __FUNCTION__);
  prctl(PR_SET_NAME, thread_name);
  char *sound_output_cmd = "mpg321 ../sound_materials/didi_warning.mp3";
  while (1)
  {
    sem_wait(&entrance_sem_failed);
    /*
     *  点亮warning LED 
     * */

    GPIOExport(WARNING_LED);
    GPIODirection(WARNING_LED, OUT);
    GPIOWrite(WARNING_LED, HIGH);
    /*
     *  播放告警声
     * */
    system(sound_output_cmd);
  }
}
/*
 *  接收到了开门的指令
 * */
static void open_the_door(struct mg_connection *nc, struct http_message *hm)
{
  //UNUSED(hm);
  char result[8] = {0};
  int result_num = 0;
  printf("hm->message.p : %s\nhm->message.len : %lu\n", hm->message.p, hm->message.len);
  printf("hm->query_string.p : %s\nhm->query_string.len : %lu\n", hm->query_string.p, hm->query_string.len);
  /* Get form variables */
  mg_get_http_var(&hm->query_string, "result", result, sizeof(result));
  /*
     *  播放开门提示音
     *  释放播放提示音的信号量
     * */
  printf("result : %s\n", result);
  result_num = atoi(result);
  if(1 == result_num) {
    sem_post(&entrance_sem_success);
    /* Send headers */
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"result\": %s }", "open_success_response");
    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
  }
  else if(0 == result_num) {
    sem_post(&entrance_sem_failed);
    /* Send headers */
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"result\": \"%s\" }", "open_failed_response");
    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
  }
  else {
    /* Send headers */
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"result\": \"%s\" }", "invalid_query_params");
    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
  }
}

static void need_input_api(struct mg_connection *nc, struct http_message *hm){
    UNUSED(hm);
    mg_printf_http_chunk(nc, "{ \"result\": \"%s\" }", "\"stful_api_need\"");
    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
  struct http_message *hm = (struct http_message *)ev_data;
  switch (ev)
  {
  case MG_EV_HTTP_REQUEST:
    if (mg_vcmp(&hm->uri, "/api/v1/hello/world") == 0)
    {
      hello_world(nc, hm); /* Handle RESTful call */
      /*
            handle RESTful call 
        */
    }
    else if (0 == mg_vcmp(&hm->uri, "/api/v1/open_the_door"))
    {
      open_the_door(nc, hm);
    }
    else
    {
        need_input_api(nc, hm);
    }
    break;
  default:
    break;
  }
}
/*
  初始化
*/
int entrance_guard_init(void)
{
  if (0 != sem_init(&entrance_sem_success, 0, 0))
  {
    printf("sem_init failed\n");
    return -1;
  }
  if (0 != sem_init(&entrance_sem_failed, 0, 0))
  {
    printf("sem_init failed\n");
    return -1;
  }
  return 0;
}
/*解初始化,释放资源*/
int entrance_guard_deinit(void)
{
  if (0 != sem_destroy(&entrance_sem_success))
  {
    printf("sem_destroy failed\n");
    return -1;
  }
  if (0 != sem_destroy(&entrance_sem_failed))
  {
    printf("sem_destroy failed\n");
    return -1;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  int i;
  char *cp;
  const char *err_str;
  int ret;
#if MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  mg_mgr_init(&mgr, NULL);

  /* Use current binary directory as document root */
  if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL))
  {
    *cp = '\0';
    s_http_server_opts.document_root = argv[0];
  }

  /* Process command line options to customize HTTP server */
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-D") == 0 && i + 1 < argc)
    {
      mgr.hexdump_file = argv[++i];
    }
    else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc)
    {
      s_http_server_opts.document_root = argv[++i];
    }
    else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
    {
      s_http_port = argv[++i];
    }
    else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
    {
      s_http_server_opts.auth_domain = argv[++i];
    }
    else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc)
    {
      s_http_server_opts.global_auth_file = argv[++i];
    }
    else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc)
    {
      s_http_server_opts.per_directory_auth_file = argv[++i];
    }
    else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc)
    {
      s_http_server_opts.url_rewrites = argv[++i];
#if MG_ENABLE_HTTP_CGI
    }
    else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
    {
      s_http_server_opts.cgi_interpreter = argv[++i];
#endif
#if MG_ENABLE_SSL
    }
    else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc)
    {
      ssl_cert = argv[++i];
#endif
    }
    else
    {
      fprintf(stderr, "Unknown option: [%s]\n", argv[i]);
      exit(1);
    }
  }

  /*
    some initials for entrance guard
  */
  entrance_guard_init();

  /*
    create thread for open the door and sound 
  */
  pthread_t pthread_open_door;
  pthread_attr_t pthread_open_door_attr;
  ret = pthread_attr_init(&pthread_open_door_attr);
  if (ret != 0)
  {
    printf("pthread_attr_init failed\n");
    return -1;
  }
  int stack_size = 64 * 1024;
  ret = pthread_attr_setstacksize(&pthread_open_door_attr, stack_size);
  if (ret != 0)
  {
    printf("pthread_attr_setstacksize failed\n");
    return -1;
  }
  ret = pthread_create(&pthread_open_door, &pthread_open_door_attr, th_prompt_tone_open_door, NULL);
  if (ret != 0)
  {
    printf("pthread create th_prompt_tone_open_door failed\n");
    return -1;
  }
   /*
    create thread for warning and sound 
  */
  pthread_t pthread_warning;
  pthread_attr_t pthread_warning_attr;
  ret = pthread_attr_init(&pthread_warning_attr);
  if (ret != 0)
  {
    printf("pthread_attr_init failed\n");
    return -1;
  }
  stack_size = 64 * 1024;
  ret = pthread_attr_setstacksize(&pthread_warning_attr, stack_size);
  if (ret != 0)
  {
    printf("pthread_attr_setstacksize failed\n");
    return -1;
  }
  ret = pthread_create(&pthread_warning, &pthread_warning_attr, th_prompt_tone_warning, NULL);
  if (ret != 0)
  {
    printf("pthread create th_prompt_tone_warning failed\n");
    return -1;
  }
  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
  if (ssl_cert != NULL)
  {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL)
  {
    fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting RESTful server on port %s, serving %s\n", s_http_port,
         s_http_server_opts.document_root);
  for (;;)
  {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
