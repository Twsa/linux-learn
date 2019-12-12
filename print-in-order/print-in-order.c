#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
typedef struct {
    // User defined data may be declared here.
    
} Foo;

inline void printFirst(){
    printf("first");
}
inline void printSecond(){
    printf("second");
}
inline void printThird(){
    printf("third");
}
Foo* fooCreate() {
    Foo* obj = (Foo*) malloc(sizeof(Foo));
    
    // Initialize user defined data here.
    
    return obj;
}

void first(Foo* obj) {
    
    // printFirst() outputs "first". Do not change or remove this line.
    printFirst();
}

void second(Foo* obj) {
    
    // printSecond() outputs "second". Do not change or remove this line.
    printSecond();
}

void third(Foo* obj) {
    
    // printThird() outputs "third". Do not change or remove this line.
    printThird();
}

void fooFree(Foo* obj) {
    // User defined data may be cleaned up here.
    
}
int main()
{

}