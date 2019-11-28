#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define RECOVERY_KEY 115        //该键值在input.h中的定义
#define key_path "/dev/input/event2"

#define PRESS_TIME_MS 5000  //长按3s时间

typedef enum {
    BUTTON_EVENT_RECOVERY
}button_event_t;

typedef void (*button_event_cb)(button_event_t ev, void *userdata);

typedef struct {
    char *dev;
    button_event_cb cb;
    void *userdata;
}button_config_t;

typedef struct button* button_handle_t;

struct button{
    pthread_t pid;
    int fd;
    button_event_cb cb;
    void *userdata;
};

typedef enum {
    PHY_BUTTON_CODE_RECOVERY = 115,
}phy_button_code_t;

typedef struct {
    int val;
    struct timeval last_time;
    bool long_pressed;
}phy_button_t;

static bool is_long_pressed(struct timeval *now, struct timeval *before, int interval) {
    int64_t expire = now->tv_sec * 1000 + now->tv_usec / 1000 - (before->tv_sec * 1000 + before->tv_usec / 1000);
    if (expire >= interval) {
        printf("the expire time is %ld\n",expire);
        return true;
    }
    return false;
}



int button_run2(button_handle_t self, int ms) {
    
    fd_set rfd;
    struct timeval tv;
    struct input_event ev;
    int ret;

    phy_button_t buttons;
    memset(&buttons, 0, sizeof(phy_button_t));

	while (1) {
        tv.tv_sec = 0;
        tv.tv_usec = ms * 1000;
        FD_ZERO(&rfd);
        FD_SET(self->fd, &rfd);

        ret = select(self->fd + 1, &rfd, NULL, NULL, &tv);
        if (ret == 0) {
            struct timeval now;
            gettimeofday(&now, NULL);
            
            if (buttons.val == 1 && is_long_pressed(&now, &buttons.last_time, PRESS_TIME_MS)) {
                self->cb(BUTTON_EVENT_RECOVERY, self->userdata);
                buttons.long_pressed = true;
                buttons.last_time = now;
            }
        } 
        if ((ret > 0) && FD_ISSET(self->fd, &rfd)) {
            memset(&ev, 0, sizeof(ev));
            read(self->fd, &ev, sizeof(ev));

            if (ev.type == 1 && ev.code == PHY_BUTTON_CODE_RECOVERY) {
           //     printf("type: %d, code: %d, value: %d\n", ev.type, ev.code, ev.value);
           //     printf("buttons.val: %d ev.value: %d,\n", buttons.val, ev.value);
                        //按键状态发生改变
                if(buttons.val == 0 && ev.value == 1) {
                        //按键按下，开始计时
                    gettimeofday(&buttons.last_time, NULL);
                    printf("last_time.tv_sec:%ld\nlast_time.tv_usec:%ld\n",buttons.last_time.tv_sec,buttons.last_time.tv_usec);

                } else if (buttons.val == 1 && ev.value == 0) {
                    //按键松开
                    if(!buttons.long_pressed){
                        self->cb(BUTTON_EVENT_RECOVERY, self->userdata);
                    }
                    buttons.long_pressed = false;
                }   
                buttons.val = ev.value;
//                printf("***buttons.val: %d ev.value: %d,\n", buttons.val, ev.value);

            }
        }
    }
      return 0;
}


int button_run(button_handle_t self) {
    return button_run2(self, 100);
}

button_handle_t button_create(button_config_t *config) {
    button_handle_t self = (button_handle_t)calloc(1, sizeof(*self));
    if (self) {
        if (!config->dev) {
            config->dev = key_path;
        }
        self->fd = open(config->dev, O_RDONLY);
        assert(self->fd > 0);
        self->cb = config->cb;
        self->userdata = config->userdata;
    }
    return self;
}

void button_destroy(button_handle_t self) {
    if (!self) return;
    close(self->fd);
    free(self);
}

void button_cb(button_event_t ev, void *userdata) {
    printf("BUTTON_EVENT_RECOVERY\n" );

    if (ev == BUTTON_EVENT_RECOVERY) {
    printf("##############   key demo   ###########\n");
    printf("####BUTTON RECOVERY under pressed!#####\n");
    printf("###the time of under-pressed is 5s!!###\n");
    printf("#######################################\n");
    }
}




int main(void) 
{
    printf("start key test!!\n");

    button_config_t config = {
        .dev = key_path, // event2
        .cb = button_cb
    };
    button_handle_t button = button_create(&config);
//  wait_timeout_sencond(5);
    while (1) {
        button_run(button);
    }
    button_destroy(button);
    return 0;
}



