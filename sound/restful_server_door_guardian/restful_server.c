/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * open the door restful api
 * /api/v1/open_the_door
 */
/* Includes ------------------------------------------------------------------*/
#include "mongoose.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UNUSED_ARGUMENT(x) (void)x
/* private constants --------------------------------------------------------*/
static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
//同步信号量for sound output and GPIO control
sem_t entrance_sem;
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void handle_sum_call(struct mg_connection *nc, struct http_message *hm) {
  char n1[100], n2[100];
  double result;

  /* Get form variables */
  mg_get_http_var(&hm->body, "n1", n1, sizeof(n1));
  mg_get_http_var(&hm->body, "n2", n2, sizeof(n2));

  /* Send headers */
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

  /* Compute the result and send it back as a JSON object */
  result = strtod(n1, NULL) + strtod(n2, NULL);
  mg_printf_http_chunk(nc, "{ \"result\": %lf }", result);
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

static void hello_world(struct mg_connection *nc, struct http_message *hm) {
  char n1[100], n2[100];
  double result;

  /* Get form variables */
  mg_get_http_var(&hm->body, "n1", n1, sizeof(n1));
  mg_get_http_var(&hm->body, "n2", n2, sizeof(n2));
  /* Send headers */
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
  result = 2 + 3;
  mg_printf_http_chunk(nc, "{ \"result\": %lf }", result);
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

/*
 *  专门开启thread用于播放声音
 *  通过信号量同步
 **/ 
void *th_prompt_tone_open_door(void *args)
{
    UNUSED_ARGUMENT(args);
    char thread_name[256] = {0};
    sprintf(thread_name, "%s", __FUNCTION__);
    prctl(PR_SET_NAME, thread_name);
    char *sound_output_cmd = "mpg321 /workspace/github_work/raspberry/sound/sound_materials/nokia-tune.mp3";
    while(1) {
        sem_wait(&entrance_sem);
        system(sound_output_cmd);
    }
}
/*
 *  接收到了开门的指令
 * */
static void open_the_door(struct mg_connection *nc, struct http_message *hm) {
  UNUSED_ARGUMENT(hm);
    /*
     *  播放开门提示音
     *  释放播放提示音的信号量
     * */    
    sem_post(&entrance_sem);
    /* Send headers */
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"result\": %s }", "ok");
    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
} 
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/api/v1/sum") == 0) {
        handle_sum_call(nc, hm); /* Handle RESTful call */
      } else if (mg_vcmp(&hm->uri, "/printcontent") == 0) {
        char buf[100] = {0};
        memcpy(buf, hm->body.p,
               sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
        printf("%s\n", buf);
      } 
      else if(mg_vcmp(&hm->uri, "/hello/world") == 0) {
        hello_world(nc, hm); /* Handle RESTful call */
        /*
            handle RESTful call 
        */ 
      }
      else if(0 == mg_vcmp(&hm->uri, "/api/v1/open_the_door")) {
        open_the_door(nc, hm);
      }
      else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}
/*
  初始化
*/
int entrance_guard_init(void) {
  if(0 != sem_init(&entrance_sem, 0, 0)){
    printf("sem_init failed\n");
    return -1;
  }
  return 0;
}
/*解初始化,释放资源*/
int entrance_guard_deinit(void) {
  if(0 != sem_destroy(&entrance_sem)) {
      printf("sem_destroy failed\n");
      return -1;
  }
  return 0;
}


int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  int i;
  char *cp;
  const char *err_str;
  int ret ;
#if MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  mg_mgr_init(&mgr, NULL);

  /* Use current binary directory as document root */
  if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
    *cp = '\0';
    s_http_server_opts.document_root = argv[0];
  }

  /* Process command line options to customize HTTP server */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
      mgr.hexdump_file = argv[++i];
    } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
      s_http_server_opts.document_root = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      s_http_port = argv[++i];
    } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
      s_http_server_opts.auth_domain = argv[++i];
    } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
      s_http_server_opts.global_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc) {
      s_http_server_opts.per_directory_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
      s_http_server_opts.url_rewrites = argv[++i];
#if MG_ENABLE_HTTP_CGI
    } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      s_http_server_opts.cgi_interpreter = argv[++i];
#endif
#if MG_ENABLE_SSL
    } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
      ssl_cert = argv[++i];
#endif
    } else {
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
  if(ret != 0) {
    printf("pthread_attr_init failed\n");
    return -1;
  }
  int stack_size = 64*1024;
  ret = pthread_attr_setstacksize(&pthread_open_door_attr, stack_size);
  if(ret != 0) {
    printf("pthread_attr_setstacksize failed\n");
    return -1;
  }
  ret = pthread_create(&pthread_open_door, &pthread_open_door_attr, th_prompt_tone_open_door, NULL);
  if(ret != 0) {
    printf("pthread create th_prompt_tone_open_door failed\n");
    return -1;
  }
  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
  if (ssl_cert != NULL) {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting RESTful server on port %s, serving %s\n", s_http_port,
         s_http_server_opts.document_root);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
