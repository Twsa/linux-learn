#ifndef _CONFIG_ITEM_H_
#define _CONFIG_ITEM_H_
#include "list.h"
#define MAX_LENGTH 16
#define __CONFIG_ITEM_TEST_

struct config_item {
    char *option;
    char *value;
    struct list_head item_list;
};
struct config_item *create_config_item(const char *opt ,const char *val);
void free_config_item(struct config_item *obj);

#ifdef __CONFIG_ITEM_TEST_
void config_item_test(void);
#endif
#endif