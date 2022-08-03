#ifndef _TEA_H
#define _TEA_H

#define TEAPI extern

#ifndef TEA_MALLOC
#define TEA_MALLOC malloc
#endif

#ifndef TEA_REALLOC
#define TEA_REALLOC realloc
#endif

#ifndef TEA_FREE
#define TEA_FREE free
#endif

#define TEA_TRUE 1
#define TEA_FALSE 0

#define TEA_ERR -1
#define TEA_OK 0

#define TEA_LOG(...)\
    tea_log(__LINE__, __PRETTY_FUNCTION__, __FILE__, __VA_ARGS__)

#define TEA_ASSERT(expr, ...)\
    if (!(expr)) {\
	fprintf(stderr, "Assertion failed at '%s':%d in %s: ", __PRETTY_FUNCTION__, __LINE__, __FILE__);\
	tea_abort(__VA_ARGS__);\
    }

#define TEA_PI 3.14159265
#define TEA_DEG2RAD(a) ((a) * TEA_PI / 180.0)
#define TEA_RAD2DEG(a) ((a) * 180.0 / TEA_PI)
#define TEA_MAX(a, b) ((a) > (b) ? (a) : (b))
#define TEA_MIN(a, b) ((a) < (b) ? (a) : (b))

enum {
    TEA_TEXTURE_STATIC = 0,
    TEA_TEXTURE_STREAM,
    TEA_TEXTURE_TARGET
};

enum {
    TEA_ALPHA = 0,
    TEA_RED,
    TEA_RG,
    TEA_RGB,
    TEA_RGBA
};

enum {
    TEA_PERSPECTIVE,
    TEA_MODELVIEW
};

typedef char te_i8;
typedef unsigned char te_u8;
typedef short te_i16;
typedef unsigned short te_u16;
typedef int te_i32;
typedef unsigned int te_u32;
typedef long int te_i64;
typedef unsigned long int te_u64;
typedef float te_f32;
typedef double te_f64;
typedef char te_bool;

typedef struct Tea Tea;
typedef struct te_config_s te_config_t;

typedef struct te_texture_s te_texture_t;
typedef struct te_tex_info_s te_tex_info_t;
typedef struct { te_u8 r, g, b, a; } te_color_t;
typedef struct { te_i32 x, y, w, h; } te_rect_t;
typedef struct te_shader_s te_shader_t;
typedef struct te_batch_s te_batch_t;

typedef float te_vec2[2];
typedef float te_vec3[3];
typedef float te_vec4[4];
typedef float te_mat4[16];

struct te_config_s {
    te_u32 flags;
    struct {
        te_i8 mag, min;
        te_bool es;
        te_u16 glsl;
    } gl;
    struct {
        te_u32 usage, size;
    } vbo;
    struct {
        te_u32 usage, size;
        te_bool setup_for_quads;
    } ibo;
};

#if defined(__cplusplus)
extern "C" {
#endif

TEAPI te_config_t tea_config(void);
TEAPI te_bool tea_init(te_config_t *config);
TEAPI void tea_quit(void);

TEAPI void tea_begin(void);
TEAPI void tea_end(void);

TEAPI void tea_add_point(te_vec2 p);
TEAPI void tea_add_line(te_vec2 p0, te_vec2 p1);
TEAPI void tea_add_rect(te_vec2 p, te_vec2 size);
TEAPI void tea_add_circle(te_vec2 p, te_f32 radius, te_i32 segments);
TEAPI void tea_add_triangle(te_vec2 p0, te_vec2 p1, te_vec2 p2);
TEAPI void tea_add_polygon(te_u32 n, te_vec2 *points);

TEAPI void tea_set_texture(te_texture_t *tex);
TEAPI void tea_set_target(te_texture_t *target);
TEAPI void tea_set_shader(te_shader_t *shader);

TEAPI void tea_set_clip(te_rect_t *rect);
TEAPI void tea_set_scissor(te_rect_t *rect);

/*=================================*
 *             Batch               *
 *=================================*/

TEAPI te_batch_t *tea_batch(te_u32 size);
TEAPI void tea_batch_destroy(te_batch_t *batch);

TEAPI void tea_batch_set(te_batch_t *batch);
TEAPI void tea_batch_flush(te_batch_t *batch);
TEAPI void tea_batch_resize(te_batch_t *batch, te_u32 size);
TEAPI void tea_batch_grow(te_batch_t *batch);

TEAPI void tea_batch_offset(te_batch_t *batch, te_u32 offset);
TEAPI te_u32 tea_batch_tell(te_batch_t *batch);
TEAPI void tea_batch_write(te_batch_t *batch, te_u32 size, const void *data);

TEAPI void tea_vertex2f(te_f32 x, te_f32 y);
TEAPI void tea_vertex3f(te_f32 x, te_f32 y, te_f32 z);
TEAPI void tea_vertex4f(te_f32 x, te_f32 y, te_f32 z, te_f32 w);
TEAPI void tea_vertex2fv(te_f32 *v);
TEAPI void tea_vertex3fv(te_f32 *v);
TEAPI void tea_vertex4fv(te_f32 *v);

TEAPI void tea_color3f(te_f32 r, te_f32 g, te_f32 b);
TEAPI void tea_color4f(te_f32 r, te_f32 g, te_f32 b, te_f32 a);
TEAPI void tea_color3ub(te_u8 r, te_u8 g, te_u8 b);
TEAPI void tea_color4ub(te_u8 r, te_u8 g, te_u8 b, te_u8 a);
TEAPI void tea_color3fv(te_f32 *v);
TEAPI void tea_color4fv(te_f32 *v);
TEAPI void tea_color3ubv(te_u8 *v);
TEAPI void tea_color4ubv(te_u8 *v);

TEAPI void tea_texcoord2f(te_f32 u, te_f32 v);
TEAPI void tea_texcoord2fv(te_f32 *v);

TEAPI void tea_normal3f(te_f32 x, te_f32 y, te_f32 z);
TEAPI void tea_normal3fv(te_f32 *v);

/*=================================*
 *             Matrix              *
 *=================================*/

TEAPI void tea_matrix_mode(te_u8 mode);
TEAPI te_f32* tea_get_matrix(te_u8 mode);
TEAPI void tea_load_identity(void);
TEAPI void tea_push_matrix(void);
TEAPI void tea_pop_matrix(void);

TEAPI void tea_load_matrix(const te_mat4 matrix);
TEAPI void tea_load_transpose_matrix(const te_mat4 matrix);
TEAPI void tea_mult_matrix(const te_mat4 matrix);
TEAPI void tea_mult_transpose_matrix(const te_mat4 matrix);
TEAPI void tea_ortho(te_f32 left, te_f32 right, te_f32 bottom, te_f32 top, te_f32 near, te_f32 far);
TEAPI void tea_frustum(te_f32 left, te_f32 right, te_f32 bottom, te_f32 top, te_f32 near, te_f32 far);
TEAPI void tea_perspective(te_f32 fovy, te_f32 aspect, te_f32 near, te_f32 far);

TEAPI void tea_translate(te_f32 x, te_f32 y, te_f32 z);
TEAPI void tea_scale(te_f32 x, te_f32 y, te_f32 z);
TEAPI void tea_rotate(te_f32 angle, te_f32 x, te_f32 y, te_f32 z);
TEAPI void tea_rotate_z(te_f32 angle);

/*=================================*
 *             Texture             *
 *=================================*/

TEAPI te_texture_t *tea_texture(te_u8 format, te_i32 width, te_i32 height, const void *data, te_u8 type);
TEAPI void tea_texture_destroy(te_texture_t *tex);

TEAPI void tea_texture_get_size(te_texture_t *tex, te_i32* width, te_i32 *height);

TEAPI void tea_texture_set_filter(te_texture_t *tex, te_i8 filter_min, te_i8 filter_mag);
TEAPI void tea_texture_set_wrap(te_texture_t *tex, te_i8 wrap_s, te_i8 wrap_t);
TEAPI void tea_texture_get_filter(te_texture_t *tex, te_u8* filter_min, te_u8* filter_mag);
TEAPI void tea_texture_get_wrap(te_texture_t *tex, te_u8* wrap_s, te_u8* wrap_t);

/*=================================*
 *             Shader              *
 *=================================*/

TEAPI te_shader_t *tea_shader(const te_i8 *vert, const te_i8 *frag);
TEAPI te_shader_t *tea_shader_from_gl(te_i32 count, te_u32 *shaders);
TEAPI void tea_shader_destroy(te_shader_t *shader);

TEAPI void tea_shader_set(te_shader_t *shader);

TEAPI te_i32 tea_shader_uniform_location(te_shader_t *shader, const te_i8 *name);

#define TEA_UNIFORM_X(X, T)\
TEAPI void tea_shader_set_uniform1##X(te_i32 location, T value);\
TEAPI void tea_shader_set_uniform2##X(te_i32 location, T v0, T v1);\
TEAPI void tea_shader_set_uniform3##X(te_i32 location, T v0, T v1, T v2);\
TEAPI void tea_shader_set_uniform4##X(te_i32 location, T v0, T v1, T v2, T v3);\
TEAPI void tea_shader_set_uniform1##X##v(te_i32 location, te_i32 val, const T *v);\
TEAPI void tea_shader_set_uniform2##X##v(te_i32 location, te_i32 val, const T *v);\
TEAPI void tea_shader_set_uniform3##X##v(te_i32 location, te_i32 val, const T *v);\
TEAPI void tea_shader_set_uniform4##X##v(te_i32 location, te_i32 val, const T *v)

TEA_UNIFORM_X(f, te_f32);
TEA_UNIFORM_X(i, te_i32);

TEAPI void tea_shader_set_uniform_matrix2fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m);
TEAPI void tea_shader_set_uniform_matrix3fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m);
TEAPI void tea_shader_set_uniform_matrix4fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m);

/*=================================*
 *              Debug              *
 *=================================*/

TEAPI void tea_log(te_i32 line, const te_i8* func, const te_i8* file, const te_i8 *fmt, ...);
TEAPI void tea_abort(const te_i8 *fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* _TEA_H */
