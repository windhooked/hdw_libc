#define LOG_FILE __FILE__".log"
#define FILE_OPEN_ERROR -1

#define DEBUG 0
#define INFO  1
#define WARN  2
#define ERROR 3
#define FATAL 4

#define log(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__, ## args)
#define logd(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__,DEBUG, ## args)
#define loge(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__,ERROR, ## args)
#define logw(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__,WARN, ## args)
#define logf(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__,FATAL, ## args)
#define logi(args...) Logger_log(__FILE__,__FUNCTION__,__LINE__,INFO, ## args)

//int logger (char *file, char *func, int l, int level, char *fmt, ...);
//
typedef struct logger {
	char *log_file;
	int param_log_level;
	char *param_exclude_msg_source;
} Logger;


//int Logger_log (const char *file, char *func, int l, int level, char *fmt, ...);
