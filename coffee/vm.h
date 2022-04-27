#ifndef COFFEE_H
#define COFFEE_H

#define CO_API extern

typedef struct Coffee Coffee;
typedef struct co_bytecode_s co_bytecode_t;

#define DEFAULT_STACK_SIZE 1024

enum {
    CO_TNONE = 0,
    CO_TNULL,
    CO_TNUMBER,
    CO_TSTRING,
    CO_TBOOLEAN,
    CO_TTABLE,
    CO_TUSERDATA,
    CO_TMAX
};

enum {
    CO_OP_RETURN = 0,
    CO_OP_ADD,
    CO_OP_SUB,
    CO_OP_MUL,
    CO_OP_DIV,
    CO_OP_MOD,

    CO_OP_MAX
};

#if defined(__cplusplus)
extern "C" {
#endif

CO_API Coffee *coffee_new(void);
CO_API void coffee_free(Coffee *vm);

CO_API void co_push_null(Coffee *vm);
CO_API void co_push_number(Coffee *vm, double number);
CO_API void co_push_string(Coffee *vm, const char *str);
CO_API void co_push_boolean(Coffee *vm, int boolean);
CO_API void co_push_value(Coffee *vm);

#if defined(__cplusplus)
}
#endif

#endif /* COFFEE_H */

#if defined(COFFEE_IMPLEMENTATION)

typedef struct bytecode_s bytecode_t;
typedef struct bean_s bean_t;

struct bytecode_s {
    int size;
    uint8_t *code;
};

struct bean_s {
    char type;
    union {
        double number;
        char *string;
        bool boolean;
        void *userdata;
    };
};

struct Coffee {
    struct {
        int size;
        int top;
        bean_t *beans; 
    } stack;

    struct {
        int size;
        int top;
        bean_t *beans;
    } locals;

    struct {
        int size;
        int top;
        bean_t *beans;
    } globals;

    struct {
        int size;
        int top;
        bean_t *beans;
    } constants;

    int pc;
    bytecode_t *bytecode;
};

Coffee *coffee_new(void) {
    Coffee *vm = (Coffee *)malloc(sizeof(Coffee));
    if (!vm) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memset(vm, 0, sizeof(Coffee));
    vm->stack.size = DEFAULT_STACK_SIZE;
    vm->stack.top = 0;
    vm->stack.beans = (bean_t *)malloc(sizeof(bean_t) * vm->stack.size);
    vm->locals.size = vm->locals.top = 0;
    vm->globals.size = vm->globals.top = 0;
    vm->constants.size = vm->constants.top = 0;
    vm->pc = 0;
    vm->bytecode = NULL;
    return vm;
}

void coffee_free(Coffee *vm) {
    if (!vm) return;
    if (vm->stack.beans) free(vm->stack.beans);
    free(vm);
}

#endif /* COFFEE_IMPLEMENTATION */