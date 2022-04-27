#ifndef _TEA_H
#define _TEA_H

#define TEAPI extern
#define TEA_VER "0.1"

#define TEA_OK 0
#define TEA_ERR -1

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

/* boolean */
#define TEA_TRUE 1
#define TEA_FALSE 0

/* data types */
#define TEA_BYTE           0x1400
#define TEA_UNSIGNED_BYTE  0x1401
#define TEA_SHORT          0x1402
#define TEA_UNSIGNED_SHORT 0x1403
#define TEA_INT            0x1404
#define TEA_UNSIGNED_INT   0x1405
#define TEA_FLOAT          0x1406
#define TEA_2_BYTES        0x1407
#define TEA_3_BYTES        0x1408
#define TEA_4_BYTES        0x1409
#define TEA_DOUBLE         0x140A

/* Primitives */
#define TEA_POINTS         0x0000
#define TEA_LINES          0x0001
#define TEA_LINE_LOOP      0x0002
#define TEA_LINE_STRIP     0x0003
#define TEA_TRIANGLES      0x0004
#define TEA_TRIANGLE_STRIP 0x0005
#define TEA_TRIANGLE_FAN   0x0006
#define TEA_QUADS          0x0007
#define TEA_QUAD_STRIP     0x0008
#define TEA_POLYGON        0x0009

/* Matrix mode */
#define TEA_MATRIX_MODE 0x0BA0
#define TEA_MODELVIEW   0x1700
#define TEA_PROJECTION  0x1701
#define TEA_TEXTURE     0x1702

/* Polygon */
#define TEA_POINT 0x1B00
#define TEA_LINE  0x1B01
#define TEA_FILL  0x1B02
#define TEA_CW    0x0900
#define TEA_CCW   0x0901
#define TEA_FRONT 0x0404
#define TEA_BACK  0x0405
#define TEA_EDGE_FLAG      0x0B43
#define TEA_CULL_FACE      0x0B44
#define TEA_CULL_FACE_MODE 0x0B45
#define TEA_FRONT_FACE     0x0B46

/* Depth buffer */
#define TEA_NEVER         0x0200
#define TEA_LESS          0x0201
#define TEA_EQUAL         0x0202
#define TEA_LEQUAL        0x0203
#define TEA_GREATER       0x0204
#define TEA_NOTEQUAL      0x0205
#define TEA_GEQUAL        0x0206
#define TEA_ALWAYS        0x0207
#define TEA_DEPTH_TEST    0x0B71

/* Blending */
#define TEA_BLEND         0x0BE2
#define TEA_BLEND_SRC     0x0BE1
#define TEA_BLEND_DST     0x0BE0
#define TEA_ZERO          0x0
#define TEA_ONE           0x1
#define TEA_SRC_COLOR     0x0300
#define TEA_ONE_MINUS_SRC_COLOR 0x0301
#define TEA_SRC_ALPHA     0x0302
#define TEA_ONE_MINUS_SRC_ALPHA 0x0303
#define TEA_DST_ALPHA     0x0304
#define TEA_ONE_MINUS_DST_ALPHA 0x0305
#define TEA_DST_COLOR     0x0306
#define TEA_ONE_MINUS_DST_COLOR 0x0307
#define TEA_SRC_ALPHA_SATURATE 0x0308

/* Buffers, Pixels Drawing/Reading */
#define TEA_NONE          0x0
#define TEA_LEFT          0x0406
#define TEA_RIGHT         0x0407
#define TEA_FRONT_LEFT    0x0400
#define TEA_FRONT_RIGHT   0x0401
#define TEA_BACK_LEFT     0x0402
#define TEA_BACK_RIGHT    0x0403
#define TEA_RED          0x1903
#define TEA_GREEN        0x1904
#define TEA_BLUE         0x1905
#define TEA_ALPHA        0x1906
#define TEA_LUMINANCE    0x1909
#define TEA_LUMINANCE_ALPHA 0x190A

#define TEA_READ_BUFFER  0x0C02
#define TEA_DRAW_BUFFER  0x0C01
#define TEA_DOUBLEBUFFER 0x0C32

#define TEA_STEREO      0x0C33
#define TEA_BITMAP      0x1A00
#define TEA_COLOR       0x1800
#define TEA_DEPTH       0x1801
#define TEA_STENCIL     0x1802
#define TEA_DITHER      0x0BD0
#define TEA_RGB         0x1907
#define TEA_RGBA        0x1908

/* bgra */
#define TEA_BGR  0x80E0
#define TEA_BGRA 0x80E1

/* Clear buffer bits */
#define TEA_DEPTH_BUFFER_BIT   0x00000100
#define TEA_ACCUM_BUFFER_BIT   0x00000200
#define TEA_STENCIL_BUFFER_BIT 0x00000400
#define TEA_COLOR_BUFFER_BIT   0x00004000

/* Texture mapping */
#define TEA_TEXTURE_1D				0x0DE0
#define TEA_TEXTURE_2D				0x0DE1
#define TEA_TEXTURE_WRAP_S			0x2802
#define TEA_TEXTURE_WRAP_T			0x2803
#define TEA_TEXTURE_MAG_FILTER			0x2800
#define TEA_TEXTURE_MIN_FILTER			0x2801
#define TEA_TEXTURE_ENV_COLOR			0x2201
#define TEA_TEXTURE_GEN_S			0x0C60
#define TEA_TEXTURE_GEN_T			0x0C61
#define TEA_TEXTURE_GEN_R			0x0C62
#define TEA_TEXTURE_GEN_Q			0x0C63
#define TEA_TEXTURE_GEN_MODE			0x2500
#define TEA_TEXTURE_BORDER_COLOR			0x1004
#define TEA_TEXTURE_WIDTH			0x1000
#define TEA_TEXTURE_HEIGHT			0x1001
#define TEA_TEXTURE_BORDER			0x1005
#define TEA_TEXTURE_COMPONENTS			0x1003
#define TEA_TEXTURE_RED_SIZE			0x805C
#define TEA_TEXTURE_GREEN_SIZE			0x805D
#define TEA_TEXTURE_BLUE_SIZE			0x805E
#define TEA_TEXTURE_ALPHA_SIZE			0x805F
#define TEA_TEXTURE_LUMINANCE_SIZE		0x8060
#define TEA_TEXTURE_INTENSITY_SIZE		0x8061
#define TEA_NEAREST_MIPMAP_NEAREST		0x2700
#define TEA_NEAREST_MIPMAP_LINEAR		0x2702
#define TEA_LINEAR_MIPMAP_NEAREST		0x2701
#define TEA_LINEAR_MIPMAP_LINEAR			0x2703
#define TEA_OBJECT_LINEAR			0x2401
#define TEA_OBJECT_PLANE				0x2501
#define TEA_EYE_LINEAR				0x2400
#define TEA_EYE_PLANE				0x2502
#define TEA_SPHERE_MAP				0x2402
#define TEA_DECAL				0x2101
#define TEA_MODULATE				0x2100
#define TEA_NEAREST				0x2600
#define TEA_REPEAT				0x2901
#define TEA_CLAMP				0x2900
#define TEA_S					0x2000
#define TEA_T					0x2001
#define TEA_R					0x2002
#define TEA_Q					0x2003

#define TEA_CLAMP_TO_EDGE			0x812F /* 1.2 */
#define TEA_CLAMP_TO_BORDER			0x812D /* 1.3 */

/* Texture 3D */
#define TEA_TEXUTRE_3D     0x806F
#define TEA_TEXTURE_DEPTH  0x8071
#define TEA_TEXTURE_WRAP_R 0x8072

/* Cube map texture */
#define TEA_TEXTURE_CUBE_MAP            0x8513
#define TEA_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define TEA_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define TEA_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define TEA_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define TEA_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define TEA_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define TEA_MAX_CUBE_MAP_TEXTURE_SIZE   0x851C

/* Texture array */
#define TEA_TEXTURE_1D_ARRAY 0x8C18
#define TEA_TEXTURE_2D_ARRAY 0x8C1A

/* Multitexture */
#define TEA_TEXTURE0              0x84C0 /* use "TEA_TEXTURE0 + i" */
#define TEA_ACTIVE_TEXTURE        0x84E0
#define TEA_MAX_TEXTURE_UNITS     0x84E1

/* Framebuffer */
#define TEA_COLOR_ATTACHMENT0     0x8CE0
#define TEA_DEPTH_ATTACHMENT      0x8D00
#define TEA_STENCIL_ATTACHMENT    0x8D20
#define TEA_FRAMEBUFFER           0x8D40
#define TEA_RENDERBUFFER          0x8D41
#define TEA_RENDERBUFFER_WIDTH    0x8D42
#define TEA_RENDERBUFFER_HEIGHT   0x8D43

/* Vertex buffer */
#define TEA_ARRAY_BUFFER                 0x8892
#define TEA_ARRAY_BUFFER_BINDING         0x8894
#define TEA_ELEMENT_ARRAY_BUFFER         0x8893
#define TEA_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define TEA_VERTEX_ARRAY_BUFFER_BINDING  0x8896
#define TEA_NORMAL_ARRAY_BUFFER_BINDING  0x8897
#define TEA_COLOR_ARRAY_BUFFER_BINDING   0x8898
#define TEA_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define TEA_STATIC_DRAW                  0x88E4
#define TEA_DYNAMIC_DRAW                 0x88E8
#define TEA_BUFFER_SIZE                  0x8764
#define TEA_BUFFER_USAGE                 0x8765
#define TEA_READ_ONLY                    0x88B8
#define TEA_WRITE_ONLY                   0x88B9
#define TEA_READ_WRITE                   0x88BA

#define TEA_STREAM_DRAW                 0x88E0
#define TEA_STREAM_READ                 0x88E1
#define TEA_STREAM_COPY                 0x88E2
#define TEA_STATIC_DRAW                 0x88E4
#define TEA_STATIC_READ                 0x88E5
#define TEA_STATIC_COPY                 0x88E6
#define TEA_DYNAMIC_DRAW                0x88E8
#define TEA_DYNAMIC_READ                0x88E9
#define TEA_DYNAMIC_COPY                0x88EA

/* Shader */
#define TEA_FRAGMENT_SHADER   0x8B30
#define TEA_VERTEX_SHADER     0x8B31

/* Vertex array */
#define TEA_VERTEX_ARRAY 0x8074
#define TEA_NORMAL_ARRAY 0x8075
#define TEA_COLOR_ARRAY  0x8076
#define TEA_INDEX_ARRAY  0x8077
#define TEA_TEXTURE_COORD_ARRAY 0x8078

#ifndef TEA_MALLOC
#define TEA_MALLOC malloc
#endif
#ifndef TEA_FREE
#define TEA_FREE free
#endif
#ifndef TEA_REALLOC
#define TEA_REALLOC realloc
#endif

typedef unsigned char te_bool;
typedef char te_byte;
typedef unsigned char te_ubyte;
typedef short te_short;
typedef unsigned short te_ushort;
typedef int te_int;
typedef unsigned int te_uint;
typedef float te_float;
typedef double te_double;
typedef int te_sizei;
typedef int te_enum;

typedef void te_void;

typedef struct Tea Tea;
typedef struct te_config_s te_config_t;

struct te_config_s {
    te_uint flags;
    struct {
        te_byte mag, min;
        te_bool es;
        te_ushort glsl;
    } gl;
    struct {
        te_uint usage, size;
    } vbo;
    struct {
        te_uint usage, size;
        te_bool setup_for_quads;
    } ibo;
};

typedef unsigned int te_texture_t;
typedef unsigned int te_fbo_t;
typedef unsigned int te_rbo_t;

//typedef struct te_buffer_s te_buffer_t;
typedef unsigned int te_buffer_t;
typedef struct {
    te_uint handle, usage;
    te_uint offset, size;
} te_buffer_internal_t;

//typedef struct te_vao_s te_vao_t;
typedef unsigned int te_vao_t;
typedef struct te_vertex_format_s te_vertex_format_t;
typedef struct te_attrib_s te_attrib_t;

typedef struct te_batch_s te_batch_t;
typedef struct te_vertex_s te_vertex_t;

struct te_attrib_s {
    te_ubyte offset, tag;
    te_ubyte size, stride;
    te_uint type;
};

#define TEA_MAX_ATTRIBS 16
struct te_vertex_format_s {
    te_vao_t handle;
    te_ubyte count;
    te_ushort stride;
    te_attrib_t attribs[TEA_MAX_ATTRIBS];
};

typedef unsigned int te_shader_t;
typedef unsigned int te_program_t;

#if defined(__cplusplus)
extern "C" {
#endif

TEAPI te_config_t tea_config(te_void);
TEAPI te_int tea_init(te_config_t *config);
TEAPI te_void tea_quit(te_void);

/*=====================================*
 *           Tea Batch Render          *
 *=====================================*/

/*=====================================*
 *           Immediate mode            *
 *=====================================*/

TEAPI te_void tea_set_drawmode(te_enum mode);
TEAPI te_void tea_set_vao(te_vao_t vao);
TEAPI te_void tea_set_vertex(te_vertex_t *vertex);
TEAPI te_void tea_set_framebuffer(te_fbo_t fbo);
TEAPI te_void tea_set_program(te_program_t program);

TEAPI te_void tea_begin(te_void);
TEAPI te_void tea_end(te_void);
TEAPI te_void tea_draw(te_void);

TEAPI te_void tea_vertex2f(te_float x, te_float y);
TEAPI te_void tea_vertex3f(te_float x, te_float y, te_float z);
TEAPI te_void tea_vertex4f(te_float x, te_float y, te_float z, te_float w);
TEAPI te_void tea_vertex2fv(te_float *v);
TEAPI te_void tea_vertex3fv(te_float *v);
TEAPI te_void tea_vertex4fv(te_float *v);

TEAPI te_void tea_color3f(te_float r, te_float g, te_float b);
TEAPI te_void tea_color4f(te_float r, te_float g, te_float b, te_float a);
TEAPI te_void tea_color3ub(te_ubyte r, te_ubyte g, te_ubyte b);
TEAPI te_void tea_color4ub(te_ubyte r, te_ubyte g, te_ubyte b, te_ubyte a);
TEAPI te_void tea_color3fv(te_float *v);
TEAPI te_void tea_color4fv(te_float *v);
TEAPI te_void tea_color3ubv(te_ubyte *v);
TEAPI te_void tea_color4ubv(te_ubyte *v);

TEAPI te_void tea_texcoord2f(te_float u, te_float v);
TEAPI te_void tea_texcoord2fv(te_float *v);

TEAPI te_void tea_normal3f(te_float x, te_float y, te_float z);
TEAPI te_void tea_normal3fv(te_float *v);

/*=====================================*
 *                Batch                *
 *=====================================*/

TEAPI te_batch_t* tea_batch(te_uint size);
TEAPI te_void tea_free_batch(te_batch_t *batch);

TEAPI te_void tea_write_batch_data(te_batch_t *batch, te_int size, te_void *data);
TEAPI te_void tea_write_batch_subdata(te_batch_t *batch, te_int offset, te_int size, te_void *data);

TEAPI te_void tea_clear_batch(te_batch_t *batch);
TEAPI te_void tea_flush_batch(te_batch_t *batch);

TEAPI te_void tea_resize_batch(te_batch_t *batch, te_uint size);

struct te_batch_s {
    te_buffer_t handle;
    te_void *data;
    te_int top;
    te_int size;
};

struct te_state_s {
    te_vao_t vao;
    te_buffer_t vbo, ibo;
    te_texture_t tex;
    te_fbo_t fbo;
    te_program_t program;
    te_float *world;
    te_float *modelview;
};

struct te_node_s {
    te_texture_t tex;
    te_int start, count;
};

/*=====================================*
 *           Vertex Format             *
 *=====================================*/

enum {
    TEA_ATTRIB_BYTE,
    TEA_ATTRIB_UBYTE,
    TEA_ATTRIB_INT,
    TEA_ATTRIB_FLOAT,

    TEA_ATTRIB_VEC2,
    TEA_ATTRIB_VEC3,
    TEA_ATTRIB_VEC4,

    TEA_ATTRIB_VEC2I,
    TEA_ATTRIB_VEC3I,
    TEA_ATTRIB_VEC4I
};

enum {
    TEA_ATTRIB_POSITION = 0,
    TEA_ATTRIB_POSITION_3D,
    TEA_ATTRIB_COLOR,
    TEA_ATTRIB_TEXCOORD,
    TEA_ATTRIB_NORMAL
};

TEAPI te_vertex_format_t* tea_vertex_format(te_void);
TEAPI te_void tea_free_vertex_format(te_vertex_format_t *format);

TEAPI te_void tea_begin_vertex_format(te_vertex_format_t *format);
TEAPI te_void tea_vertex_format_add_attrib(te_enum attrib);
TEAPI te_void tea_end_vertex_format(te_vertex_format_t *format);


/*=====================================*
 *               Vertex                *
 *=====================================*/

TEAPI te_vertex_t* tea_vertex(te_vertex_format_t *format, te_int vertices);
TEAPI te_void tea_free_vertex(te_vertex_t *vertex);

TEAPI te_void tea_begin_vertex(te_vertex_t *vertex);
TEAPI te_void tea_end_vertex(te_vertex_t *vertex);
TEAPI te_void tea_flush_vertex(te_vertex_t *vertex);

TEAPI te_void tea_draw_vertex(te_vertex_t *vertex, te_enum mode);
TEAPI te_void tea_draw_vertex_part(te_vertex_t *vertex, te_enum mode, te_int first, te_int count);

TEAPI te_bool tea_vertex_is_empty(te_vertex_t *vertex);

TEAPI te_int tea_vertex_offset(te_vertex_t *vertex);
TEAPI te_int tea_vertex_size(te_vertex_t *vertex);
TEAPI te_int tea_vertex_vertices(te_vertex_t *vertex);
TEAPI te_int tea_vertex_vertice_offset(te_vertex_t *vertex);

TEAPI te_void tea_vertex_grow(te_vertex_t *vertex);

/*=====================================*
 *               OpenGL                *
 *=====================================*/

/*=====================================*
 *                Misc                 *
 *=====================================*/

TEAPI te_void(*tea_glClearColor)(te_float r, te_float g, te_float b, te_float a);
TEAPI te_void(*tea_glClearDepth)(te_float depth);
TEAPI te_void(*tea_glClear)(te_enum mask);

TEAPI te_void(*tea_glCullFace)(te_uint c);
TEAPI te_void(*tea_glFrontFace)(te_uint c);

TEAPI te_void(*tea_glScissor)(te_int x, te_int y, te_uint w, te_uint h);

TEAPI te_void(*tea_glEnable)(te_uint cap);
TEAPI te_void(*tea_glDisable)(te_uint cap);

/* Blend functions */
TEAPI te_void(*tea_glBlendFunc)(te_uint sfactor, te_uint dfactor);
TEAPI te_void(*tea_glBlendFuncSeparate)(te_uint srcRGB, te_uint dstRGB, te_uint srcAlpha, te_uint dstAlpha);
TEAPI te_void(*tea_glBlendEquation)(te_uint mode);
TEAPI te_void(*tea_glBlendEquationSeparate)(te_uint modeRGB, te_uint modeAlpha);
TEAPI te_void(*tea_glBlendColor)(te_float r, te_float g, te_float b, te_float a);

/* Depth functions */
TEAPI te_void(*tea_glDepthFunc)(te_uint func);
TEAPI te_void(*tea_glDepthMask)(te_bool mask);
TEAPI te_void(*tea_glDepthRange)(te_double n, te_double f);

/* Stencil functions */
TEAPI te_void(*tea_glStencilFunc)(te_uint func, te_uint ref, te_uint mask);
TEAPI te_void(*tea_glStencilMask)(te_uint mask);
TEAPI te_void(*tea_glStencilOp)(te_uint fail, te_uint zfail, te_uint zpass);

/*=====================================*
 *           Transformation            *
 *=====================================*/

TEAPI te_void(*tea_glViewport)(te_int x, te_int y, te_int w, te_int h);
TEAPI te_void(*tea_glMatrixMode)(te_uint mode);
TEAPI const te_float*(*tea_glGetMatrix)(te_uint mode);
TEAPI te_void(*tea_glPushMatrix)(te_void);
TEAPI te_void(*tea_glPopMatrix)(te_void);
TEAPI te_void(*tea_glLoadIdentity)(te_void);

#define TEA_MATRIX_X(X, T)\
TEAPI te_void(*tea_glLoadMatrix##X##)(const T *m);\
TEAPI te_void(*tea_glLoadTransposeMatrix##X##)(const T *m);\
TEAPI te_void(*tea_glMultMatrix##X##)(const T *m);\
TEAPI te_void(*tea_glMult_tranposeMatrix##X##)(const T *m);\
TEAPI te_void(*tea_glTranslate##X##)(T x, T y, T z);\
TEAPI te_void(*tea_glScale##X##)(T x, T y, T z);\
TEAPI te_void(*tea_glRotate##X##)(T angle, T x, T y, T z);\
TEAPI te_void(*tea_glOrtho##X##)(T left, T right, T bottom, T top, T near, T far);\
TEAPI te_void(*tea_glFrustum##X##)(T left, T right, T bottom, T top, T near, T far)

TEA_MATRIX_X(f, te_float);
TEA_MATRIX_X(d, te_double);

#define tea_glOrtho tea_glOrthod
#define tea_glFrustum te_glFrustumd
TEAPI te_void(*tea_glPerspective)(te_double fovy, te_double aspect, te_double near, te_double far);

/*=====================================*
 *               Texture               *
 *=====================================*/

#define tea_texture tea_texture2D

TEAPI te_texture_t tea_texture1D(const te_byte *data, te_uint width, te_uint format);
TEAPI te_texture_t tea_texture2D(const te_ubyte *data, te_uint width, te_uint height, te_uint format);
TEAPI te_void tea_free_texture(te_texture_t tex);

TEAPI te_texture_t tea_white_texture(te_void);

TEAPI te_void(*tea_glGenTextures)(te_sizei n, te_texture_t *textures);
TEAPI te_void(*tea_glDeleteTexture)(te_sizei n, const te_texture_t *textures);

TEAPI te_bool(*tea_glIsTexture)(te_texture_t tex);

TEAPI te_void(*tea_glBindTexture)(te_enum target, te_texture_t texture);
TEAPI te_void(*tea_glTexImage1D)(te_enum target, te_int level, te_int internalFormat, te_sizei width, te_int border, te_enum format, te_enum type, const te_void *data);
TEAPI te_void(*tea_glTexImage2D)(te_enum target, te_int level, te_int internalFormat, te_sizei width, te_sizei height, te_int border, te_enum format, te_enum type, const te_void *data);
TEAPI te_void(*tea_glTexImage3D)(te_enum target, te_int level, te_int internalFormat, te_sizei width, te_sizei height, te_sizei depth, te_int border, te_enum format, te_enum type, const te_void *data);

TEAPI te_void(*tea_glTexSubimage1D)(te_enum target, te_int level, te_int xoffset, te_sizei width, te_enum format, te_enum type, const te_void *data);
TEAPI te_void(*tea_glTexSubimage2D)(te_enum target, te_int level, te_int xoffset, te_int yoffset, te_sizei width, te_sizei height, te_enum format, te_enum type, const te_void *data);
TEAPI te_void(*tea_glTexSubimage3D)(te_enum target, te_int level, te_int xoffset, te_int yoffset, te_int zoffset, te_sizei width, te_sizei height, te_sizei depth, te_enum format, te_enum type, const te_void *data);

//TEAPI te_void tea_glTexParameteri(te_uint param, te_int value);
#define TEAGL_API extern
TEAPI te_void(*tea_glTexParameteri)(te_enum target, te_enum param, te_int value);
TEAPI te_void(*tea_glTexParameteriv)(te_enum target, te_enum param, te_int *values);
TEAPI te_void(*tea_glTexParameterf)(te_uint param, te_float value);
TEAPI te_void(*tea_glTexParameterfv)(te_uint param, te_float *values);

TEAPI te_void tea_bind_texture(te_uint target, te_texture_t tex);

TEAPI te_void tea_tex_image1D(te_uint target, te_int level, te_sizei width, const te_void* pixels);
TEAPI te_void tea_tex_image2D(te_uint target, te_int level, te_sizei width, te_sizei height, const te_void *pixels);
TEAPI te_void tea_tex_image3D(te_uint target, te_int level, te_sizei width, te_sizei height, te_sizei depth, const te_void *pixels);

TEAPI te_void tea_tex_subimage1D(te_uint target, te_int level, te_int xoffset, te_sizei width, const te_void *pixels);
TEAPI te_void tea_tex_subimage2D(te_uint target, te_int level, te_int xoffset, te_int yoffset, te_sizei width, te_sizei height, const te_void *pixels);
TEAPI te_void tea_tex_subimage3D(te_uint target, te_int level, te_int xoffset, te_int yoffset, te_int zoffset, te_sizei width, te_sizei height, te_sizei depth, const te_void *pixels);

TEAPI te_void tea_tex_parameteri(te_uint param, te_int value);
TEAPI te_void tea_tex_parameteriv(te_uint param, te_int *value);
TEAPI te_void tea_tex_parameterf(te_uint param, te_float value);
TEAPI te_void tea_tex_parameterfv(te_uint param, te_float *value);

/*=====================================*
 *             Framebuffer             *
 *=====================================*/

TEAPI te_fbo_t tea_fbo(te_void);
TEAPI te_void tea_free_fbo(te_fbo_t fbo);

TEAPI te_void tea_bind_fbo(te_uint target, te_fbo_t fbo);

TEAPI te_void(*tea_glGenFramebuffers)(te_sizei n, te_fbo_t *framebuffers);
TEAPI te_void(*tea_glDeleteFramebuffers)(te_sizei n, te_fbo_t *framebuffers);

TEAPI te_void(*tea_glBindFramebuffer)(te_enum target, te_fbo_t framebuffer);
TEAPI te_void(*tea_glBlitFramebuffer)(te_enum target, te_int srcX0, te_int srcY0, te_int srcX1, te_int srcY1, te_int dstX0, te_int dstY0, te_int dstX1, te_int dstY1, te_enum mask, te_enum filter);

TEAPI te_void(*tea_glFramebufferTexture)(te_enum target, te_enum attachment, te_texture_t texture, te_int level);
TEAPI te_void(*tea_glFramebufferTexture1D)(te_enum target, te_enum attachment, te_enum textarget, te_texture_t texture, te_int level);
TEAPI te_void(*tea_glFramebufferTexture2D)(te_enum target, te_enum attachment, te_enum textarget, te_texture_t texture, te_int level);
TEAPI te_void(*tea_glFramebufferTexture3D)(te_enum target, te_enum attachment, te_enum textarget, te_texture_t texture, te_int level, te_int layer);

TEAPI te_bool(*tea_glIsFramebuffer)(te_fbo_t fbo);

/* Renderbuffer */
TEAPI te_void(*tea_glGenRenderbuffers)(te_sizei n, te_rbo_t *renderbuffers);
TEAPI te_void(*tea_glDeleteRenderbuffers)(te_sizei n, te_rbo_t *renderbuffers);

TEAPI te_void(*tea_glBindRenderbuffer)(te_enum target, te_rbo_t renderbuffer);

TEAPI te_bool(*tea_glIsRenderbuffer)(te_rbo_t renderbuffer);

TEAPI te_void tea_fbo_texture(te_uint target, te_uint attachment, te_texture_t texture);
TEAPI te_void tea_fbo_renderbuffer(te_uint target, te_uint attachment, te_rbo_t rbo);

TEAPI te_rbo_t tea_rbo(te_void);
TEAPI te_void tea_free_rbo(te_rbo_t rbo);

/*=====================================*
 *               Buffer                *
 *=====================================*/

TEAPI te_void(*tea_glGenBuffers)(te_sizei n, te_buffer_t *buffers);
TEAPI te_void(*tea_glDeleteBuffers)(te_sizei n, te_buffer_t *buffers);

TEAPI te_void(*tea_glBindBuffer)(te_enum target, te_buffer_t buffer);

TEAPI te_void(*tea_glBufferData)(te_enum target, te_uint size, const te_void *data, te_uint usage);
TEAPI te_void(*tea_glBufferSubData)(te_enum target, te_uint offset, te_uint size, const te_void *data);

TEAPI te_void*(*tea_glMapBuffer)(te_enum target, te_enum access);
TEAPI te_bool(*tea_glUnmapBuffer)(te_enum target);

TEAPI te_void tea_gen_buffers(te_uint size, te_buffer_t *buffers);
TEAPI te_void tea_delete_buffers(te_uint size, te_buffer_t *buffers);

//TEAPI te_buffer_t *tea_buffer(te_uint size);
TEAPI te_buffer_t tea_buffer(te_void);
TEAPI te_void tea_free_buffer(te_buffer_t *buffer);

TEAPI te_void tea_bind_buffer(te_enum target, te_buffer_t buffer);

TEAPI te_void tea_buffer_data(te_enum target, te_uint size, const te_void *data, te_uint usage);
TEAPI te_void tea_buffer_subdata(te_enum target, te_uint offset, te_uint size, const te_void *data);

TEAPI te_void* tea_map_buffer(te_enum target, te_enum access);
TEAPI te_bool tea_unmap_buffer(te_enum target);
/*
TEAPI te_void tea_lock_buffer(te_uint target);
TEAPI te_void tea_unlock_buffer(te_uint target);
TEAPI te_void tea_seek_buffer(te_uint target, te_uint offset);
TEAPI te_uint tea_tell_buffer(te_uint target);
TEAPI te_void tea_read_buffer(te_uint target, te_void *data, te_uint size);
TEAPI te_void tea_write_buffer(te_uint target, const te_void *data, te_uint size);
*/

/*=====================================*
 *            Vertex Array             *
 *=====================================*/

TEAPI te_void tea_gen_vaos(te_uint size, te_vao_t *vao);
TEAPI te_void tea_delete_vaos(te_uint size, te_vao_t *vao);

TEAPI te_vao_t tea_vao(te_void);
TEAPI te_void tea_free_vao(te_vao_t *vao);

TEAPI te_void(*tea_glGenVertexArrays)(te_sizei n, te_vao_t *arrays);
TEAPI te_void(*tea_glDeleteVertexArrays)(te_sizei n, te_vao_t *arrays);

TEAPI te_void(*tea_glBindVertexArray)(te_vao_t array);
TEAPI te_bool(*tea_glIsVertexArray)(te_vao_t array);

TEAPI te_void(*tea_glEnableVertexAttribArray)(te_uint index);
TEAPI te_void(*tea_glDisableVertexAttribArray)(te_uint index);
TEAPI te_void(*tea_glVertexAttribPointer)(te_uint index, te_int size, te_enum type, te_bool normalized, te_sizei stride, const te_void *pointer);

TEAPI te_void(*tea_glDrawArrays)(te_uint mode, te_int first, te_int count);
TEAPI te_void(*tea_glDrawElements)(te_uint mode, te_int count, te_uint type, const te_void *indices);

TEAPI te_void tea_bind_vao(te_vao_t vao);

TEAPI te_void tea_vao_attach_vertex(te_vertex_t *vertex);

TEAPI te_void tea_enable_attrib(te_uint index);
TEAPI te_void tea_disable_attrib(te_uint index);

TEAPI te_void tea_vertex_attrib_pointer(te_uint index, te_int size, te_enum type, te_bool normalized, te_sizei stride, const te_void *pointer);

TEAPI te_void tea_draw_arrays(te_uint mode, te_int first, te_int count);
TEAPI te_void tea_draw_elements(te_uint mode, te_int count, te_uint type, const te_void *indices);

/*=====================================*
 *               Shader                *
 *=====================================*/

//TEAPI te_shader_t tea_shader(const te_byte *vert, const te_byte *frag);
//TEAPI te_void tea_free_shader(te_shader_t shader);

//TEAPI te_void tea_use_shader(te_shader_t shader);

TEAPI te_shader_t tea_shader(te_enum type, const te_byte *source);
TEAPI te_void tea_delete_shader(te_shader_t shader);

TEAPI te_program_t tea_program(te_uint count, te_shader_t *shaders);
TEAPI te_void tea_free_program(te_program_t *program);

TEAPI te_void tea_use_program(te_program_t program);

TEAPI te_void(*tea_glUseProgram)(te_uint program);
TEAPI te_int(*tea_glGetUniformLocation)(te_program_t program, const te_byte *name);

#define TEA_UNIFORM_X(X, T)\
TEAPI te_void(*tea_glUniform1##X##)(te_int location, T v);\
TEAPI te_void(*tea_glUniform2##X##)(te_int location, T v0, T v1);\
TEAPI te_void(*tea_glUniform3##X##)(te_int location, T v0, T v1, T v2);\
TEAPI te_void(*tea_glUniform4##X##)(te_int location, T v0, T v1, T v2, T v3);\
TEAPI te_void(*tea_glUniform1##X##v)(te_int location, te_int value, const T *v);\
TEAPI te_void(*tea_glUniform2##X##v)(te_int location, te_int value, const T *v);\
TEAPI te_void(*tea_glUniform3##X##v)(te_int location, te_int value, const T *v);\
TEAPI te_void(*tea_glUniform4##X##v)(te_int location, te_int value, const T *v)

TEA_UNIFORM_X(f, te_float);
TEA_UNIFORM_X(i, te_int);

TEAPI te_void(*tea_glUniformMatrix2fv)(te_int location, te_int count, te_bool transpose, const te_float *m);
TEAPI te_void(*tea_glUniformMatrix3fv)(te_int location, te_int count, te_bool transpose, const te_float *m);
TEAPI te_void(*tea_glUniformMatrix4fv)(te_int location, te_int count, te_bool transpose, const te_float *m);

/*=====================================*
 *                Debug                *
 *=====================================*/

TEAPI te_void tea_abort(const te_byte *fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* _TEA_H */
