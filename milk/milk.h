#ifndef MILK_H
#define MILK_H

#define MILK_API extern

#define MI_TRUE 1
#define MI_FALSE 0

#define MI_OK 0
#define MI_ERROR -1

typedef struct Milk Milk;
typedef struct mi_config_s mi_config_t;

typedef char mi_bool;
typedef char mi_i8;
typedef unsigned char mi_u8;
typedef short mi_i16;
typedef unsigned short mi_u16;
typedef int mi_i32;
typedef unsigned int mi_u32;
typedef long int mi_i64;
typedef unsigned long int mi_u64;
typedef float mi_f32;
typedef double mi_f64;

typedef float mi_vec2[2];
typedef float mi_vec3[3];
typedef float mi_vec4[4];

#if defined(__cplusplus)
extern "C" {
#endif

MILK_API void mi_init();
MILK_API void mi_quit();

struct mi_window_s {
    mi_vec2 pos;
    mi_vec2 size;

    mi_f32 border_radius;

    mi_u32 flags;

    void *userdata;
};

MILK_API mi_bool mi_begin(const char *title, struct mi_window_s *conf);
MILK_API void mi_end(void);

struct mi_button_s {
    mi_vec2 size;
    mi_i32 image;

    mi_u32 flags;
};

MILK_API int mi_button(const char *label, struct mi_button_s *conf);
MILK_API void mi_image(int image, mi_vec2 size, int flags);

struct mi_slider_s {
    mi_vec2 size;
    mi_f32 min, max;
    mi_f32 step;

    mi_u32 flags;
};

MILK_API mi_bool mi_slider_float(const mi_i8 *label, mi_f32 *val, struct mi_slider_s *conf);
MILK_API mi_bool mi_slider_int(const mi_i8 *label, mi_i32 *val, struct mi_slider_s *conf);

#if defined(__cplusplus)
}
#endif

#endif /* MILK_H */