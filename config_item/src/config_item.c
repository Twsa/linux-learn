#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"
#include "config_item.h"

/*
* 一个节点的结构体数据的内存释放
*/
void free_config_item(struct config_item *obj) {
    free(obj->option);
    free(obj->value);
    free(obj);
    obj->option = obj->value = NULL;
    obj = NULL;
}

/*
* 创建一个包含 struct head的结构体对象的创建
*/
struct config_item *create_config_item(const char *opt ,const char *val) {
    struct config_item *item = NULL;
    /*
    * 创建数据的内存块  
    */
    item = (struct config_item *)malloc(sizeof(struct config_item));
    item->option = (char *)malloc(sizeof(opt) * strlen(opt));
    item->value = (char *)malloc(sizeof(val) * strlen(val));
    /*
    * 对初始化的内存块做赋值操作
    */
    strcpy(item->option, opt);
    strcpy(item->value, val);
    return item;
}

#ifdef __CONFIG_ITEM_TEST_
void config_item_test(void) {
    /*
    * 定义一个头节点
    */
    LIST_HEAD(item_head);
    /*
    * 插入节点
    */
    struct config_item *item1 = create_config_item("item1", "1");
    list_add_tail(&item1->item_list, &item_head);
    struct config_item *item2 = create_config_item("item2", "2");
    list_add_tail(&item2->item_list, &item_head);
    /*
    * 遍历每一个节点
    */
    list_for_each_entry(item1, &item_head, item_list) {
        printf("%s = %s\n", item1->option, item1->value);
    }
}
#endif