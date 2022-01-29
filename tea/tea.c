#include "tea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GET_GL(name)\
(gl()->procs[TEA_GL_##name])

#define CALL_GL(name)\
((GL##name##Proc)GET_GL(name))

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#endif

#if defined(_WIN32)
    #include <windows.h>
    #ifndef WINDOWS_LEAN_AND_MEAN
        #define WINDOWS_LEAN_AND_MEAN 1
    #endif
    static HMODULE s_glsym;
#else
    #include <dlfcn.h>
    static te_void *s_glsym;
    #ifndef RTLD_LAZY
        #define RTLD_LAZY 0x00001
        #endif
    #ifndef RTLD_GLOBAL
        #define RTLD_GLOBAL 0x00100
    #endif
#endif

#define tea() (&s_tea_ctx)
#define gl() (&tea()->gl)
#define state() (&tea()->state)
#define base() (&tea()->base)

#define TEA_GL_VERSION 0x1F02
#define TEA_GL_SHADING_LANGUAGE_VERSION 0x8B8C

enum {
    TEA_PROC_OVERRIDE = (1 << 0),
    TEA_PROC_RET_ON_DUP = (1 << 1),
};

enum {
    /* Miscellaenous */
    TEA_GL_ClearColor = 0,
    TEA_GL_ClearDepth,
    TEA_GL_Clear,
    TEA_GL_BlendFunc,
    TEA_GL_LogicOp,
    TEA_GL_CullFace,
    TEA_GL_FrontFace,
    TEA_GL_PolygonMode,
    TEA_GL_Scissor,
    TEA_GL_DrawBuffer,
    TEA_GL_ReadBuffer,
    TEA_GL_Enable,
    TEA_GL_Disable,

    TEA_GL_EnableClientState,
    TEA_GL_DisableClientState,

    TEA_GL_GetBooleanv,
    TEA_GL_GetDoublev,
    TEA_GL_GetFloatv,
    TEA_GL_GetIntegerv,
    TEA_GL_GetError,
    TEA_GL_GetString,

    TEA_GL_GetStringi, /* 3.0 */

    /* Depth */
    TEA_GL_DepthFunc,
    TEA_GL_DepthMask,
    TEA_GL_DepthRange,

    /* Transformation */
    TEA_GL_Viewport,
    TEA_GL_MatrixMode,
    TEA_GL_PushMatrix,
    TEA_GL_PopMatrix,
    TEA_GL_LoadIdentity,

    TEA_GL_LoadMatrixd,
    TEA_GL_MultMatrixd,
    TEA_GL_Rotated,
    TEA_GL_Scaled,
    TEA_GL_Translated,

    TEA_GL_LoadMatrixf,
    TEA_GL_MultMatrixf,
    TEA_GL_Rotatef,
    TEA_GL_Scalef,
    TEA_GL_Translatef,

    TEA_GL_Ortho,
    TEA_GL_Frustum,

    TEA_GL_Orthof, /* GL ES */
    TEA_GL_Frustumf, /* GL ES */

    TEA_GL_LoadTransposeMatrixd,
    TEA_GL_MultTransposeMatrixd,
    TEA_GL_LoadTransposeMatrixf,
    TEA_GL_MultTransposeMatrixf,

    /* Vertex arrays */
    TEA_GL_VertexPointer,
    TEA_GL_NormalPointer,
    TEA_GL_ColorPointer,
    TEA_GL_TexCoordPointer,
    TEA_GL_IndexPointer,
    TEA_GL_EdgeFlagPointer,

    TEA_GL_DrawArrays,
    TEA_GL_DrawElements,

    /* Texture mapping */
    TEA_GL_TexParameterf,
    TEA_GL_TexParameteri,
    TEA_GL_TexParameterfv,
    TEA_GL_TexParameteriv,

    TEA_GL_GetTexParameteriv,
    TEA_GL_GetTexParameterfv,

    TEA_GL_GenTextures,
    TEA_GL_DeleteTextures,
    TEA_GL_BindTexture,
    TEA_GL_IsTexture,

    TEA_GL_TexImage1D,
    TEA_GL_TexImage2D,
    TEA_GL_TexSubImage1D,
    TEA_GL_TexSubImage2D,
    TEA_GL_CopyTexImage1D,
    TEA_GL_CopyTexImage2D,
    TEA_GL_CopyTexSubImage1D,
    TEA_GL_CopyTexSubImage2D,

    TEA_GL_TexImage3D,
    TEA_GL_TexSubImage3D,
    TEA_GL_CopyTexSubImage3D,

    /* GL_ARB_vertex_buffer_object */
    TEA_GL_BindBuffer,
    TEA_GL_DeleteBuffers,
    TEA_GL_GenBuffers,
    TEA_GL_BufferData,
    TEA_GL_BufferSubData,
    TEA_GL_MapBuffer,
    TEA_GL_UnmapBuffer,

    /* GL_ARB_vertex_program */
    TEA_GL_VertexAttribPointer,
    TEA_GL_EnableVertexAttribArray,
    TEA_GL_DisableVertexAttribArray,

    /* GL_ARB_vertex_shader */
    TEA_GL_BindAttribLocation,
    TEA_GL_GetAttribLocation,
    TEA_GL_GetActiveAttrib,

    /* GL_EXT_framebuffer_object */
    TEA_GL_GenFramebuffers,
    TEA_GL_DeleteFramebuffers,
    TEA_GL_BindFramebuffer,
    TEA_GL_CheckFramebufferStatus,
    TEA_GL_FramebufferTexture2D,
    TEA_GL_FramebufferRenderbuffer,
    TEA_GL_GenerateMipmap,
    TEA_GL_BlitFramebuffer,
    TEA_GL_IsFramebuffer,

    TEA_GL_GenRenderbuffers,
    TEA_GL_DeleteRenderbuffers,
    TEA_GL_BindRenderbuffer,
    TEA_GL_RenderbufferStorage,
    TEA_GL_RenderbufferStorageMultisample,
    TEA_GL_IsRenderbuffer,

    /* GL_ARB_shader_objects */
    TEA_GL_CreateProgram,
    TEA_GL_DeleteProgram,
    TEA_GL_UseProgram,
    TEA_GL_CreateShader,
    TEA_GL_DeleteShader,
    TEA_GL_ShaderSource,
    TEA_GL_CompileShader,
    TEA_GL_GetShaderiv,
    TEA_GL_GetShaderInfoLog,
    TEA_GL_AttachShader,
    TEA_GL_DetachShader,
    TEA_GL_LinkProgram,
    TEA_GL_GetProgramiv,
    TEA_GL_GetProgramInfoLog,
    TEA_GL_GetUniformLocation,
    TEA_GL_GetActiveUniform,
    TEA_GL_Uniform1f,
    TEA_GL_Uniform2f,
    TEA_GL_Uniform3f,
    TEA_GL_Uniform4f,
    TEA_GL_Uniform1i,
    TEA_GL_Uniform2i,
    TEA_GL_Uniform3i,
    TEA_GL_Uniform4i,
    TEA_GL_Uniform1fv,
    TEA_GL_Uniform2fv,
    TEA_GL_Uniform3fv,
    TEA_GL_Uniform4fv,
    TEA_GL_Uniform1iv,
    TEA_GL_Uniform2iv,
    TEA_GL_Uniform3iv,
    TEA_GL_Uniform4iv,
    TEA_GL_UniformMatrix2fv,
    TEA_GL_UniformMatrix3fv,
    TEA_GL_UniformMatrix4fv,

    /* GL_ARB_vertex_array_object */
    TEA_GL_BindVertexArray,
    TEA_GL_DeleteVertexArrays,
    TEA_GL_GenVertexArrays,

    TEA_GL_PROC_COUNT,
};

enum {
    TEA_HAS_VBO = (1 << 0),
    TEA_HAS_VAO = (1 << 1),
    TEA_HAS_SHADER = (1 << 2)
};

/* Miscellaneous */
typedef te_void(*GLClearColorProc)(te_float, te_float, te_float, te_float);
typedef te_void(*GLClearProc)(te_uint);
typedef te_void(*GLBlendFuncProc)(te_uint, te_uint);
typedef te_void(*GLLogicOpProc)(te_uint);
typedef te_void(*GLCullFaceProc)(te_uint);
typedef te_void(*GLFrontFaceProc)(te_uint);
typedef te_void(*GLPolygonModeProc)(te_uint, te_uint);
typedef te_void(*GLScissorProc)(te_int, te_int, te_int, te_int);
typedef te_void(*GLDrawBufferProc)(te_uint);
typedef te_void(*GLReadBufferProc)(te_uint);
typedef te_void(*GLEnableProc)(te_uint);
typedef te_void(*GLDisableProc)(te_uint);

typedef te_void(*GLEnableClientStateProc)(te_uint); /* 1.1 */
typedef te_void(*GLDisableClientStateProc)(te_uint); /* 1.1 */

typedef te_void(*GLGetBooleanvProc)(te_uint, te_bool*);
typedef te_void(*GLGetDoublevProc)(te_uint, te_double*);
typedef te_void(*GLGetFloatvProc)(te_uint, te_float*);
typedef te_void(*GLGetIntegervProc)(te_uint, te_int*);
typedef te_void(*GLGetErrorProc)(te_void);
typedef const te_ubyte*(*GLGetStringProc)(te_uint);

typedef const te_ubyte*(*GLGetStringiProc)(te_uint, te_uint); /* 3.0 */

/* Depth buffer */
typedef te_void(*GLClearDepthProc)(te_float);
typedef te_void(*GLDepthFuncProc)(te_uint);
typedef te_void(*GLDepthMaskProc)(te_bool);
typedef te_void(*GLDepthRangeProc)(te_double, te_double);

/* Transformation */
typedef te_void(*GLViewportProc)(te_int, te_int, te_int, te_int);
typedef te_void(*GLMatrixModeProc)(te_uint);
typedef te_void(*GLPushMatrixProc)(te_void);
typedef te_void(*GLPopMatrixProc)(te_void);
typedef te_void(*GLLoadIdentityProc)(te_void);
typedef te_void(*GLLoadMatrixfProc)(const te_float*);
typedef te_void(*GLLoadMatrixdProc)(const te_double*);
typedef te_void(*GLMultMatrixfProc)(const te_float*);
typedef te_void(*GLMultMatrixdProc)(const te_double*);
typedef te_void(*GLOrthoProc)(te_double, te_double, te_double, te_double, te_double, te_double);
typedef te_void(*GLFrustumProc)(te_double, te_double, te_double, te_double, te_double, te_double);
typedef te_void(*GLTranslatefProc)(te_float, te_float, te_float);
typedef te_void(*GLRotatefProc)(te_float, te_float, te_float, te_float);
typedef te_void(*GLScalefProc)(te_float, te_float, te_float);
typedef te_void(*GLTranslatedProc)(te_double, te_double, te_double);
typedef te_void(*GLRotatedProc)(te_double, te_double, te_double, te_double);
typedef te_void(*GLScaledProc)(te_double, te_double, te_double);

typedef te_void(*GLLoadTransposeMatrixdProc)(const te_double[16]); /* 1.3 */
typedef te_void(*GLLoadTransposeMatrixfProc)(const te_float[16]); /* 1.3 */
typedef te_void(*GLMultTransposeMatrixdProc)(const te_double[16]); /* 1.3 */
typedef te_void(*GLMultTransposeMatrixfProc)(const te_float[16]); /* 1.3 */

/* Vertex Arrays */
typedef te_void(*GLVertexPointerProc)(te_int, te_uint, te_int, const te_void*);
typedef te_void(*GLColorPointerProc)(te_int, te_uint, te_int, const te_void*);
typedef te_void(*GLTexCoordPointerProc)(te_int, te_uint, te_int, const te_void*);
typedef te_void(*GLNormalPointerProc)(te_uint, te_int, const te_void*);
typedef te_void(*GLIndexPointerProc)(te_uint, te_int, const te_void*);
typedef te_void(*GLEdgeFlagPointerProc)(te_int, te_int, const te_void*);

typedef te_void(*GLDrawArraysProc)(te_uint, te_int, te_int);
typedef te_void(*GLDrawElementsProc)(te_uint, te_int, te_uint, const te_void*);

/* Texture mapping */
typedef te_void(*GLTexParameterfProc)(te_uint, te_uint, te_float);
typedef te_void(*GLTexParameteriProc)(te_uint, te_uint, te_int);
typedef te_void(*GLTexParameterfvProc)(te_uint, te_uint, const te_float*);
typedef te_void(*GLTexParameterivProc)(te_uint, te_uint, const te_int*);

typedef te_void(*GLGetTexParameterfProc)(te_uint, te_uint, te_float*);
typedef te_void(*GLGetTexParameteriProc)(te_uint, te_uint, te_int*);
typedef te_void(*GLGetTexParameterfvProc)(te_uint, te_uint, te_float*);
typedef te_void(*GLGetTexParameterivProc)(te_uint, te_uint, te_int*);

typedef te_void(*GLGenTexturesProc)(te_uint, te_uint*);
typedef te_void(*GLDeleteTexturesProc)(te_uint, te_uint*);
typedef te_void(*GLBindTextureProc)(te_uint, te_uint);
typedef te_void(*GLTexImage1DProc)(te_uint, te_int, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLTexImage2DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLTexImage3DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLTexSubImage1DProc)(te_uint, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLTexSubImage2DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLTexSubImage3DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_int, te_uint, te_uint, const te_void*);
typedef te_void(*GLCopyTexImage1DProc)(te_uint, te_int, te_uint, te_int, te_int, te_int, te_int);
typedef te_void(*GLCopyTexImage2DProc)(te_uint, te_int, te_uint, te_int, te_int, te_int, te_int, te_int);
typedef te_void(*GLCopyTexSubImage1DProc)(te_uint, te_int, te_int, te_int, te_int, te_int);
typedef te_void(*GLCopyTexSubImage2DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_int, te_int);
typedef te_void(*GLCopyTexSubImage3DProc)(te_uint, te_int, te_int, te_int, te_int, te_int, te_int, te_int, te_int, te_int);

/* GL_ARB_vertex_buffer_object */
typedef te_void(*GLBindBufferProc)(te_uint, te_uint);
typedef te_void(*GLBufferDataProc)(te_uint, te_uint, const te_void*, te_uint);
typedef te_void(*GLBufferSubDataProc)(te_uint, te_uint, te_uint, const te_void*);
typedef te_void(*GLGenBuffersProc)(te_uint, te_uint*);
typedef te_void(*GLDeleteBuffersProc)(te_uint, te_uint*);
typedef te_void*(*GLMapBufferProc)(te_uint, te_uint);
typedef te_uint(*GLUnmapBufferProc)(te_uint);

/* GL_ARB_vertex_array_object */
typedef te_void(*GLGenVertexArraysProc)(te_uint, te_uint*);
typedef te_void(*GLBindVertexArrayProc)(te_uint);
typedef te_void(*GLDeleteVertexArraysProc)(te_uint, te_uint*);

/* GL_ARB_vertex_array_program */
typedef te_void(*GLVertexAttribPointerProc)(te_uint, te_int, te_uint, te_int, te_int, const te_void*);
typedef te_void(*GLEnableVertexAttribArrayProc)(te_uint);
typedef te_void(*GLDisableVertexAttribArrayProc)(te_uint);

/* GL_EXT_framebuffer_object */
typedef te_bool(*GLIsRenderbufferProc)(te_uint);
typedef te_void(*GLBindRenderbufferProc)(te_uint, te_uint);
typedef te_void(*GLDeleteRenderbuffersProc)(te_uint, te_uint*);
typedef te_void(*GLGenRenderbuffersProc)(te_uint, te_uint*);
typedef te_void(*GLRenderbufferStorageProc)(te_uint, te_uint, te_uint, te_uint);
typedef te_void(*GLGetRenderbufferParameterivProc)(te_uint, te_uint, te_int*);

typedef te_bool(*GLIsFramebufferProc)(te_uint);
typedef te_void(*GLBindFramebufferProc)(te_uint, te_uint);
typedef te_void(*GLDeleteFramebuffersProc)(te_uint, te_uint*);
typedef te_void(*GLGenFramebuffersProc)(te_uint, te_uint*);
typedef te_void(*GLFramebufferRenderbufferProc)(te_uint, te_uint, te_uint, te_uint);
typedef te_void(*GLFramebufferTexture1DProc)(te_uint, te_uint, te_uint, te_uint, te_int);
typedef te_void(*GLFramebufferTexture2DProc)(te_uint, te_uint, te_uint, te_uint, te_int);
typedef te_void(*GLFramebufferTexture3DProc)(te_uint, te_uint, te_uint, te_uint, te_int, te_int);
typedef te_void(*GLFramebufferTextureLayerProc)(te_uint, te_uint, te_uint, te_int, te_int);
typedef te_uint(*GLCheckFramebufferStatusProc)(te_uint);
typedef te_void(*GLGetFramebufferAttachmentParameterivProc)(te_uint, te_uint, te_uint, te_int*);
typedef te_void(*GLBlitFramebufferProc)(te_int, te_int, te_int, te_int, te_int, te_int, te_int, te_int, te_uint);
typedef te_void(*GLGenerateMipmapProc)(te_uint);

/* GL_ARB_shader_objects */
typedef te_void(*GLDeleteShaderProc)(te_uint);
typedef te_uint(*GLCreateShaderProc)(te_uint);
typedef te_void(*GLShaderSourceProc)(te_uint, te_int, const te_byte**, const te_int*);
typedef te_void(*GLCompileShaderProc)(te_uint);
typedef te_uint(*GLGetShaderivProc)(te_uint, te_uint, te_int*);
typedef te_uint(*GLGetShaderInfoLogProc)(te_uint, te_int, te_int*, te_byte*);

typedef te_uint(*GLCreateProgramProc)(te_void);
typedef te_void(*GLDeleteProgramProc)(te_uint);
typedef te_void(*GLAttachShaderProc)(te_uint, te_uint);
typedef te_void(*GLDetachShaderProc)(te_uint, te_uint);
typedef te_void(*GLLinkProgramProc)(te_uint);
typedef te_void(*GLUseProgramProc)(te_uint);

typedef te_void(*GLGetProgramivProc)(te_uint, te_uint, te_int*);
typedef te_void(*GLGetProgramInfoLogProc)(te_uint, te_int, te_int*, te_byte*);
typedef te_void(*GLGetActiveUniformProc)(te_uint, te_uint, te_int, te_int*, te_int*, te_int*, te_byte*);
typedef te_int(*GLGetUniformLocationProc)(te_uint, const te_byte*);

#define GL_UNIFORM_X(X, T)\
typedef te_void(*GLUniform1##X##Proc)(te_int, T);\
typedef te_void(*GLUniform2##X##Proc)(te_int, T, T);\
typedef te_void(*GLUniform3##X##Proc)(te_int, T, T, T);\
typedef te_void(*GLUniform4##X##Proc)(te_int, T, T, T, T);\
typedef te_void(*GLUniform1##X##vProc)(te_int, te_int value, const T*);\
typedef te_void(*GLUniform2##X##vProc)(te_int, te_int value, const T*);\
typedef te_void(*GLUniform3##X##vProc)(te_int, te_int value, const T*);\
typedef te_void(*GLUniform4##X##vProc)(te_int, te_int value, const T*)

GL_UNIFORM_X(f, te_float);
GL_UNIFORM_X(i, te_int);

typedef te_void(*GLUniformMatrix2fvProc)(te_int, te_int, te_bool, const te_float*);
typedef te_void(*GLUniformMatrix3fvProc)(te_int, te_int, te_bool, const te_float*);
typedef te_void(*GLUniformMatrix4fvProc)(te_int, te_int, te_bool, const te_float*);

/* GL_ARB_vertex_shader */
typedef te_int(*GLGetAttribLocationProc)(te_uint prog, const te_byte* name);
typedef te_void(*GLGetActiveAttribProc)(te_uint prog, te_uint index, te_int bufSize, te_int* length, te_int* size, te_uint* type, te_byte* name);
typedef te_void(*GLBindAttribLocationProc)(te_uint prog, te_uint index, const te_byte* name);

typedef struct te_proc_s te_proc_t;

struct te_proc_s {
    te_ubyte tag;
    const te_byte *names[3];
};

struct te_vertex_s {
    te_vertex_format_t* format;
    te_vao_t vao;
    te_buffer_t vbo;
    te_uint offset, size;
    te_void *data;
};

static struct {
    te_byte mag, min;
} s_gl_max_ver;
struct te_gl_s {
    struct {
        te_ubyte major, minor;
        te_ushort glsl;
        te_bool es;
    } version;
    te_uint extensions;
    te_void* procs[TEA_GL_PROC_COUNT];
};

#define MAX_MATRIX_STACK 32
struct matrix_stack_s {
    te_float m[MAX_MATRIX_STACK][16];
    te_ubyte top;
};

struct te_base_s {
    te_program_t program;
    te_vertex_format_t *format;

    te_vao_t vao;
    te_buffer_t vbo, ibo;
};

struct te_state_s {
    te_uint clear_flags;
    te_texture_t texture;
    te_shader_t shader;
    struct {
        te_vertex_format_t *format;
        te_vertex_t *ptr;
        te_enum mode;
        te_float pos[4];
        te_float color[4];
        te_float texcoord[2];
        te_float normal[3];
    } vertex;
    struct {
        struct matrix_stack_s stack[3];
        te_uint mode;
        te_float *ptr;
    } matrix;
};

struct Tea {
    struct te_gl_s gl;
    struct te_state_s state;
    struct te_base_s base;
};

static Tea s_tea_ctx;
static te_vertex_t s_default_vertex;
static te_texture_t s_white_texture;

static te_vertex_format_t* s_default_format_2d;
static te_vertex_format_t* s_default_format_2d_z;
static te_vertex_format_t* s_default_format_3d;

static te_bool s_load_gl(te_void);
static te_void s_setup_gl(te_void);
static te_void s_close_gl(te_void);

static te_void* s_get_proc(const te_byte *name);
static te_buffer_t* s_get_buffer_safe(te_uint target);

te_void s_enable_vertex_format(te_vertex_format_t *format);
te_void s_disable_vertex_format(te_vertex_format_t *format);

#define BUFFER_SIZE 1000
te_config_t tea_config(te_void) {
    te_config_t conf = {0};
    memset(&conf, 0, sizeof(te_config_t));
    conf.vbo.size = 4*BUFFER_SIZE;
    conf.vbo.usage = TEA_DYNAMIC_DRAW;
    conf.ibo.size = 6*BUFFER_SIZE;
    conf.ibo.usage = TEA_DYNAMIC_DRAW;
    return conf;
}

te_int tea_init(te_config_t *config) {
    TEA_ASSERT(config != NULL, "config is NULL");
    memset(tea(), 0, sizeof(Tea));
    gl()->version.major = config->gl.mag;
    gl()->version.minor = config->gl.min;
    gl()->version.es = config->gl.es;

    if (s_load_gl()) {
        s_setup_gl();
        s_close_gl();
    } else
        TEA_ASSERT(0, "failed to initialize OpenGL");

    state()->vertex.color[3] = 1.f;
    state()->vertex.pos[2] = 0.f;
    state()->vertex.pos[3] = 1.f;
    tea_matrix_mode(TEA_MODELVIEW);
    tea_load_identity();
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();

    state()->clear_flags = TEA_COLOR_BUFFER_BIT | TEA_DEPTH_BUFFER_BIT;
    s_default_format_2d = tea_vertex_format();
    s_default_format_2d_z = tea_vertex_format();
    s_default_format_3d = tea_vertex_format();

    te_vertex_format_t *format = s_default_format_2d_z;
    tea_begin_vertex_format(format);
    tea_vertex_format_add_attrib(TEA_ATTRIB_POSITION_3D);
    tea_vertex_format_add_attrib(TEA_ATTRIB_COLOR);
    tea_vertex_format_add_attrib(TEA_ATTRIB_TEXCOORD);
    tea_end_vertex_format(format);

    format = s_default_format_2d;
    tea_begin_vertex_format(format);
    tea_vertex_format_add_attrib(TEA_ATTRIB_POSITION);
    tea_vertex_format_add_attrib(TEA_ATTRIB_COLOR);
    tea_vertex_format_add_attrib(TEA_ATTRIB_TEXCOORD);
    tea_end_vertex_format(format);

    format = s_default_format_3d;
    tea_begin_vertex_format(format);
    tea_vertex_format_add_attrib(TEA_ATTRIB_POSITION_3D);
    tea_vertex_format_add_attrib(TEA_ATTRIB_COLOR);
    tea_vertex_format_add_attrib(TEA_ATTRIB_TEXCOORD);
    tea_vertex_format_add_attrib(TEA_ATTRIB_NORMAL);
    tea_end_vertex_format(format);

    base()->format = s_default_format_2d_z;
    state()->vertex.ptr = &s_default_vertex;

    base()->vao = tea_vao();
    te_vertex_t *vertex = &s_default_vertex;
    memset(vertex, 0, sizeof(*vertex));
    //memcpy(&vertex->format, base()->format, sizeof(*format));
    vertex->vao = base()->vao;
    vertex->format = base()->format;
    vertex->size = config->vbo.size;
    base()->vbo = tea_buffer();
    vertex->data = malloc(vertex->size);
    te_vao_t vao = vertex->vao;
    TEA_ASSERT(vertex->data != NULL, "Failed to alloc memory for default vertex data\n");
    tea_bind_vao(vao);
    tea_bind_buffer(TEA_ARRAY_BUFFER, base()->vbo);
    s_enable_vertex_format(vertex->format);
    tea_buffer_data(TEA_ARRAY_BUFFER, vertex->size, NULL, TEA_DYNAMIC_DRAW);
    tea_bind_vao(0);
    base()->ibo = tea_buffer();

    vertex->vbo = base()->vbo;
    state()->vertex.format = base()->format;


    const te_byte white_pixel_data[] = {
        255, 255, 255, 255
    };
    s_white_texture = tea_texture2D(white_pixel_data, 1, 1, TEA_RGBA);
    return TEA_OK;
}

te_void tea_quit(te_void) {
    te_vertex_t *vertex = &s_default_vertex;
    tea_free_buffer(&vertex->vbo);
    tea_free_texture(s_white_texture);
}

te_void tea_begin(te_ubyte mode) {
    TEA_ASSERT(mode >= TEA_POINTS && mode <= TEA_TRIANGLE_FAN, "invalid draw mode");
    te_vertex_t *vertex = state()->vertex.ptr ? state()->vertex.ptr : &s_default_vertex;

    state()->vertex.mode = mode;
    tea_begin_vertex(vertex);
}

te_void tea_end(te_void) {
    te_vertex_t *vertex = state()->vertex.ptr;
    tea_end_vertex(vertex);
    tea_draw_vertex(vertex, state()->vertex.mode);
}

static te_void s_tea_push_vertex(te_void) {
    te_vertex_t *vertex = state()->vertex.ptr;
    te_byte *data = vertex->data;
    te_float *ptr = (te_float*)(data + vertex->offset);
    te_vertex_format_t *format = vertex->format;
    ptr[0] = state()->vertex.pos[0];
    ptr[1] = state()->vertex.pos[1];
    ptr[2] = state()->vertex.pos[2];
    ptr[3] = state()->vertex.pos[3];
    te_uint i = format->attribs[0].size;
    ptr[i] = state()->vertex.color[0];
    ptr[i+1] = state()->vertex.color[1];
    ptr[i+2] = state()->vertex.color[2];
    ptr[i+3] = state()->vertex.color[3];
    ptr[i+4] = state()->vertex.texcoord[0];
    ptr[i+5] = state()->vertex.texcoord[1];
    ptr[i+6] = state()->vertex.normal[0];
    ptr[i+7] = state()->vertex.normal[1];
    ptr[i+8] = state()->vertex.normal[2];
    vertex->offset += format->stride;
}

te_void tea_vertex2f(te_float x, te_float y) {
    te_float p[] = {x, y};
    tea_vertex2fv(p);
}

te_void tea_vertex3f(te_float x, te_float y, te_float z) {
    te_float p[] = {x, y, z};
    tea_vertex3fv(p);
}

te_void tea_vertex4f(te_float x, te_float y, te_float z, te_float w) {
    te_float p[] = {x, y, z, w};
    tea_vertex4fv(p);
}

te_void tea_vertex2fv(te_float *v) {
    state()->vertex.pos[0] = v[0];
    state()->vertex.pos[1] = v[1];
    s_tea_push_vertex();
}

te_void tea_vertex3fv(te_float *v) {
    state()->vertex.pos[0] = v[0];
    state()->vertex.pos[1] = v[1];
    state()->vertex.pos[2] = v[2];
    s_tea_push_vertex();
}

te_void tea_vertex4fv(te_float *v) {
    state()->vertex.pos[0] = v[0];
    state()->vertex.pos[1] = v[1];
    state()->vertex.pos[2] = v[2];
    state()->vertex.pos[3] = v[3];
    s_tea_push_vertex();
}

te_void tea_color3f(te_float r, te_float g, te_float b) {
    te_float c[] = {r, g, b};
    tea_color3fv(c);
}

te_void tea_color3ub(te_ubyte r, te_ubyte g, te_ubyte b) {
    te_float c[] = {r / 255.0f, g / 255.0f, b / 255.0f};
    tea_color3fv(c);
}

te_void tea_color4f(te_float r, te_float g, te_float b, te_float a) {
    te_float c[] = {r, g, b, a};
    tea_color4fv(c);
}

te_void tea_color4ub(te_ubyte r, te_ubyte g, te_ubyte b, te_ubyte a) {
    te_float c[] = {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    tea_color4fv(c);
}

te_void tea_color3fv(te_float *v) {
    state()->vertex.color[0] = v[0];
    state()->vertex.color[1] = v[1];
    state()->vertex.color[2] = v[2];
}

te_void tea_color4fv(te_float *v) {
    state()->vertex.color[0] = v[0];
    state()->vertex.color[1] = v[1];
    state()->vertex.color[2] = v[2];
    state()->vertex.color[3] = v[3];
}

te_void tea_texcoord2f(te_float x, te_float y) {
    te_float c[] = {x, y};
    tea_texcoord2fv(c);
}

te_void tea_texcoord2fv(te_float *v) {
    state()->vertex.texcoord[0] = v[0];
    state()->vertex.texcoord[1] = v[1];
}

te_void tea_normal3f(te_float x, te_float y, te_float z) {
    te_float n[] = {x, y, z};
    tea_normal3fv(n);
}

te_void tea_normal3fv(te_float *v) {
    state()->vertex.normal[0] = v[0];
    state()->vertex.normal[1] = v[1];
    state()->vertex.normal[2] = v[2];
}

/*=====================================*
 *                Misc                 *
 *=====================================*/

te_void tea_clear_color (te_float r, te_float g, te_float b, te_float a) {
    CALL_GL(ClearColor)(r, g, b, a);
}

te_void tea_clear_depth(te_float depth) {
    CALL_GL(ClearDepth)(depth);
}

te_void tea_clear_flags(te_uint flags) {
    state()->clear_flags = flags;
}

te_void tea_clear(te_void) {
    CALL_GL(Clear)(state()->clear_flags);
}

te_void tea_cull_face(te_uint c) {
    CALL_GL(CullFace)(c);
}
te_void tea_front_face(te_uint c) {
    CALL_GL(FrontFace)(c);
}

te_void tea_scissor(te_int x, te_int y, te_uint w, te_uint h) {
    CALL_GL(Scissor)(x, y, w, h);
}

te_void tea_enable(te_uint cap) {
    CALL_GL(Enable)(cap);
}
te_void tea_disable(te_uint cap) {
    CALL_GL(Disable)(cap);
}

/*=====================================*
 *           Transformation            *
 *=====================================*/

te_void tea_viewport(te_int x, te_int y, te_int w, te_int h) { CALL_GL(Viewport)(x, y, w, h); }
te_void tea_matrix_mode(te_uint mode) { CALL_GL(MatrixMode)(mode); }
const te_float *tea_get_matrix(te_uint mode) {
    mode -= TEA_MODELVIEW;
    struct matrix_stack_s *stack = &state()->matrix.stack[mode];
    return stack->m[stack->top];
}
te_void tea_push_matrix(te_void) { CALL_GL(PushMatrix)(); }
te_void tea_pop_matrix(te_void) { CALL_GL(PopMatrix)(); }
te_void tea_load_identity(te_void) { CALL_GL(LoadIdentity)(); }

te_void tea_load_matrixf(const te_float *m) { CALL_GL(LoadMatrixf)(m); }
te_void tea_load_transpose_matrixf(const te_float *m) { CALL_GL(LoadTransposeMatrixf)(m); }
te_void tea_mult_matrixf(const te_float *m) { CALL_GL(MultMatrixf)(m); }
te_void tea_mult_transpose_matrixf(const te_float *m) { CALL_GL(MultTransposeMatrixf)(m); }
te_void tea_translatef(te_float x, te_float y, te_float z) { CALL_GL(Translatef)(x, y, z); }
te_void tea_rotatef(te_float angle, te_float x, te_float y, te_float z) { CALL_GL(Rotatef)(angle, x, y, z); }
te_void tea_scalef(te_float x, te_float y, te_float z) { CALL_GL(Scalef)(x, y, z); }
te_void tea_orthof(te_float left, te_float right, te_float bottom, te_float top, te_float near, te_float far) {
    CALL_GL(Ortho)(left, right, bottom, top, near, far);
}
te_void tea_frustumf(te_float left, te_float right, te_float bottom, te_float top, te_float near, te_float far) {
    CALL_GL(Frustum)(left, right, bottom, top, near, far);
}

te_void tea_load_matrixd(const te_double *m) { CALL_GL(LoadMatrixd)(m); }
te_void tea_load_transpose_matrixd(const te_double *m) { CALL_GL(LoadTransposeMatrixd)(m); }
te_void tea_mult_matrixd(const te_double *m) { CALL_GL(MultMatrixd)(m); }
te_void tea_mult_transpose_matrixd(const te_double *m) { CALL_GL(MultTransposeMatrixd)(m); }
te_void tea_translated(te_double x, te_double y, te_double z) { CALL_GL(Translated)(x, y, z); }
te_void tea_rotated(te_double angle, te_double x, te_double y, te_double z) { CALL_GL(Rotated)(angle, x, y, z); }
te_void tea_scaled(te_double x, te_double y, te_double z) { CALL_GL(Scaled)(x, y, z); }
te_void tea_orthod(te_double left, te_double right, te_double bottom, te_double top, te_double near, te_double far) {
    CALL_GL(Ortho)(left, right, bottom, top, near, far);
}
te_void tea_frustumd(te_double left, te_double right, te_double bottom, te_double top, te_double near, te_double far) {
    CALL_GL(Frustum)(left, right, bottom, top, near, far);
}

te_void tea_perspective(te_double fovy, te_double aspect, te_double zNear, te_double zFar) {
    te_double ymax = zNear * tan(fovy * TEA_PI / 360.0);
    te_double ymin = -ymax;
    te_double xmin = ymin * aspect;
    te_double xmax = ymax * aspect;
    tea_frustumd(xmin, xmax, ymin, ymax, zNear, zFar);
}

/***************
 * internal
 ***************/

#define CLONE_MATRIX(dest, src)\
    for (i = 0; i < 16; i++) dest[i] = src[i]

#define TRANSPOSE_MATRIX(dest, src)\
    for (i = 0; i < 4; i++)\
        for (j = 0; j < 4; j++)\
            dest[i * 4 + j] = src[j * 4 + i]

/* auxiliar */
static te_void s_clone_matrixf(te_float *dst, const te_float *src) {
    te_int i;
    CLONE_MATRIX(dst, src);
}

static te_void s_clone_matrixd(te_float *dst, const te_float *src) {
    te_int i;
    CLONE_MATRIX(dst, src);
}

static te_void s_transpose_matrixf(te_float *dst, const te_float *src) {
    te_int i, j;
    TRANSPOSE_MATRIX(dst, src);
}

static te_void s_transpose_matrixd(te_double *dst, const te_double *src) {
    te_int i, j;
    TRANSPOSE_MATRIX(dst, src);
}

static te_void s_mult_matrixf(te_float *dest, const te_float *a, const te_float *b) {
    te_int i, j, k;
    te_float tmp[16];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[i * 4 + j] = 0.0f;
            for (k = 0; k < 4; k++) {
                tmp[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    s_clone_matrixf(dest, tmp);
}

static te_void s_mult_matrixd(te_double *dest, const te_double *a, const te_double *b) {
    te_int i, j, k;
    te_double tmp[16];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[i * 4 + j] = 0.0f;
            for (k = 0; k < 4; k++) {
                tmp[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    s_clone_matrixd(dest, tmp);
}

static te_void s_tea_matrix_mode(te_uint mode) {
    te_ubyte m = mode - TEA_MODELVIEW;
    TEA_ASSERT(m < 3, "Invalid matrix mode");
    state()->matrix.mode = mode;
    struct matrix_stack_s *stack = &state()->matrix.stack[m];
    state()->matrix.ptr = stack->m[stack->top];
}

static te_void s_tea_push_matrix(te_void) {
    struct matrix_stack_s *stack = &state()->matrix.stack[state()->matrix.mode - TEA_MODELVIEW];
    TEA_ASSERT(stack->top < MAX_MATRIX_STACK - 1, "Matrix stack overflow");
    stack->top++;
    s_clone_matrixf(stack->m[stack->top], state()->matrix.ptr);
}

static te_void s_tea_pop_matrix(te_void) {
    struct matrix_stack_s *stack = &state()->matrix.stack[state()->matrix.mode - TEA_MODELVIEW];
    TEA_ASSERT(stack->top > 0, "Matrix stack underflow");
    stack->top--;
    state()->matrix.ptr = stack->m[stack->top];
}

static te_void s_tea_load_identity(te_void) {
    for (te_ubyte i = 0; i < 16; i++)
        state()->matrix.ptr[i] = 0.f;
    state()->matrix.ptr[0] = 1.f;
    state()->matrix.ptr[5] = 1.f;
    state()->matrix.ptr[10] = 1.f;
    state()->matrix.ptr[15] = 1.f;
}

/* float */
static te_void s_tea_load_matrixf(const te_float *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_int i;
    CLONE_MATRIX(state()->matrix.ptr, m);
}

static te_void s_tea_load_transpose_matrixf(const te_float *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_int i, j;
    TRANSPOSE_MATRIX(state()->matrix.ptr, m);
}

static te_void s_tea_mult_matrixf(const te_float *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_float *ptr = state()->matrix.ptr;
    s_mult_matrixf(ptr, m, ptr);
}

static te_void s_tea_mult_transpose_matrixf(const te_float *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_float tmp[16];
    s_transpose_matrixf(tmp, m);
    s_mult_matrixf(state()->matrix.ptr, tmp, state()->matrix.ptr);
}

static te_void s_tea_translatef(te_float x, te_float y, te_float z) {
   te_float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, z, 1.0f
    };
    s_tea_mult_matrixf(m);
}

static te_void s_tea_scalef(te_float x, te_float y, te_float z) {
    te_float m[16] = {
        x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    s_tea_mult_matrixf(m);
}

static te_void s_tea_rotatef(te_float angle, te_float x, te_float y, te_float z) {
    te_float c = cosf(TEA_DEG2RAD(angle));
    te_float s = sinf(TEA_DEG2RAD(angle));
    te_float nc = 1.f - c;
    te_float len = x*x + y*y + z*z;
    if (len > 0.f) {
        te_float rlen = 1.f / sqrtf(len);
        x *= rlen;
        y *= rlen;
        z *= rlen;
    }
    te_float m[16] = {
        x*x*nc + c,   y*x*nc + z*s, z*x*nc - y*s, 0.f,
        x*y*nc - z*s, y*y*nc + c,   z*y*nc + x*s, 0.f,
        x*z*nc + y*s, y*z*nc - x*s, z*z*nc + c,   0.f,
        0.f,         0.f,         0.f,         1.f
    };
    s_mult_matrixf(state()->matrix.ptr, m, state()->matrix.ptr);
}

static te_void s_tea_orthof(te_float left, te_float right, te_float bottom, te_float top, te_float near, te_float far) {
    te_float *ptr = state()->matrix.ptr;
    ptr[0] = 2.f / (right - left);
    ptr[1] = 0.f;
    ptr[2] = 0.f;
    ptr[3] = 0.f;

    ptr[4] = 0.f;
    ptr[5] = 2.f / (top - bottom);
    ptr[6] = 0.f;
    ptr[7] = 0.f;

    ptr[8] = 0.f;
    ptr[9] = 0.f;
    ptr[10] = -2.f / (far - near);
    ptr[11] = 0.f;

    ptr[12] = -(right + left) / (right - left);
    ptr[13] = -(top + bottom) / (top - bottom);
    ptr[14] = -(far + near) / (far - near);
    ptr[15] = 1.f;
}

static te_void s_tea_frustumf(te_float left, te_float right, te_float bottom, te_float top, te_float near, te_float far) {
    te_float m[16];
    te_float a = (right + left) / (right - left);
    te_float b = (top + bottom) / (top - bottom);
    te_float c = -(far + near) / (far - near);
    te_float d = -(2.f * far * near) / (far - near);
    m[0] = 2.f * near / (right - left);
    m[1] = 0.f;
    m[2] = 0.f;
    m[3] = 0.f;

    m[4] = 0.f;
    m[5] = 2.f * near / (top - bottom);
    m[6] = 0.f;
    m[7] = 0.f;

    m[8] = a;
    m[9] = b;
    m[10] = c;
    m[11] = -1.f;

    m[12] = 0.f;
    m[13] = 0.f;
    m[14] = d;
    m[15] = 0.f;
    s_tea_mult_matrixf(m);
}

/* double */
static te_void s_tea_load_matrixd(const te_double *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_int i;
    CLONE_MATRIX(state()->matrix.ptr, (te_float)m);
}

static te_void s_tea_load_transpose_matrixd(const te_double *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_int i, j;
    TRANSPOSE_MATRIX(state()->matrix.ptr, (te_float)m);
}

static te_void s_tea_mult_matrixd(const te_double *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_float *ptr = state()->matrix.ptr;
    te_float tmp[16];
    te_int i;
    CLONE_MATRIX(tmp, ptr);
    s_mult_matrixf(ptr, m, ptr);
}

static te_void s_tea_mult_transpose_matrixd(const te_double *m) {
    TEA_ASSERT(m != NULL, "Invalid matrix");
    te_float tmp[16];
    te_int i, j;
    TRANSPOSE_MATRIX(tmp, m);
    s_mult_matrixf(state()->matrix.ptr, tmp, state()->matrix.ptr);
}

static te_void s_tea_translated(te_double x, te_double y, te_double z) {
    te_float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, z, 1.0f
    };
    s_tea_mult_matrixf(m);
}

static te_void s_tea_scaled(te_double x, te_double y, te_double z) {
    te_float m[16] = {
        x, 0.f, 0.f, 0.f,
        0.f, y, 0.f, 0.f,
        0.f, 0.f, z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    s_tea_mult_matrixf(m);
}

static te_void s_tea_rotated(te_double angle, te_double x, te_double y, te_double z) {
    te_double c = cos(TEA_DEG2RAD(angle));
    te_double s = sin(TEA_DEG2RAD(angle));
    te_double nc = 1.0 - c;
    te_double len = x * x + y * y + z * z;
    if (len > 0.0) {
        te_double rlen = 1.f / sqrt(len);
        x *= rlen;
        y *= rlen;
        z *= rlen;
    }
    te_float m[16] = {
        x*x*nc + c,   y*x*nc + z*s, z*x*nc - y*s, 0.0,
        x*y*nc - z*s, y*y*nc + c,   z*y*nc + x*s, 0.0,
        x*z*nc + y*s, y*z*nc - x*s, z*z*nc + c,   0.0,
        0.0,         0.0,         0.0,         1.0
    };
    s_mult_matrixf(state()->matrix.ptr, m, state()->matrix.ptr);
}

static te_void s_tea_orthod(te_double left, te_double right, te_double bottom, te_double top, te_double near, te_double far) {
    s_tea_orthof(left, right, bottom, top, near, far);
}

static te_void s_tea_frustumd(te_double left, te_double right, te_double bottom, te_double top, te_double near, te_double far) {
    s_tea_frustumf((te_float)left, (te_float)right, (te_float)bottom, (te_float)top, (te_float)near, (te_float)far);
}

/*=====================================*
 *               Texture               *
 *=====================================*/

te_texture_t tea_texture1D(const te_byte *data, te_uint width, te_uint format) {
    TEA_ASSERT(width > 0, "Invalid texture width");
    TEA_ASSERT(data != NULL, "Invalid texture data");
    te_texture_t texture = {0};
    CALL_GL(GenTextures)(1, &texture);
    CALL_GL(BindTexture)(TEA_TEXTURE_1D, texture);
    CALL_GL(TexImage1D)(TEA_TEXTURE_1D, 0, format, width, 0, format, TEA_UNSIGNED_BYTE, data);
    CALL_GL(BindTexture)(TEA_TEXTURE_1D, 0);

    return texture;
}

te_texture_t tea_texture2D(const te_ubyte *data, te_uint width, te_uint height, te_uint format) {
    TEA_ASSERT(width > 0 && height > 0, "Invalid texture size");
    te_texture_t texture = 0;
    CALL_GL(GenTextures)(1, &texture);
    CALL_GL(BindTexture)(TEA_TEXTURE_2D, texture);
    CALL_GL(TexParameteri)(TEA_TEXTURE_2D, TEA_TEXTURE_WRAP_S, TEA_CLAMP_TO_EDGE);
    CALL_GL(TexParameteri)(TEA_TEXTURE_2D, TEA_TEXTURE_WRAP_T, TEA_CLAMP_TO_EDGE);
    CALL_GL(TexParameteri)(TEA_TEXTURE_2D, TEA_TEXTURE_MIN_FILTER, TEA_NEAREST);
    CALL_GL(TexParameteri)(TEA_TEXTURE_2D, TEA_TEXTURE_MAG_FILTER, TEA_NEAREST);
    CALL_GL(TexImage2D)(TEA_TEXTURE_2D, 0, format, width, height, 0, format, TEA_UNSIGNED_BYTE, data);
    CALL_GL(BindTexture)(TEA_TEXTURE_2D, 0);

    return texture;
}

te_void tea_free_texture(te_texture_t tex) {
    CALL_GL(DeleteTextures)(1, &tex);
}

te_texture_t tea_white_texture(te_void) {
    return s_white_texture;
}

te_void tea_bind_texture(te_uint target, te_texture_t tex) {
    CALL_GL(BindTexture)(target, tex);
}

te_void tea_tex_image1D(te_uint target, te_int level, te_sizei width, const te_void* pixels) {
    CALL_GL(TexImage1D)(target, level, TEA_RGBA, width, 0, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}
te_void tea_tex_image2D(te_uint target, te_int level, te_sizei width, te_sizei height, const te_void *pixels) {
    CALL_GL(TexImage2D)(target, level, TEA_RGBA, width, height, 0, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}
te_void tea_tex_image3D(te_uint target, te_int level, te_sizei width, te_sizei height, te_sizei depth, const te_void *pixels) {
    CALL_GL(TexImage3D)(target, level, TEA_RGBA, width, height, depth, 0, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}

te_void tea_tex_subimage1D(te_uint target, te_int level, te_int xoffset, te_sizei width, const te_void *pixels) {
    CALL_GL(TexSubImage1D)(target, level, xoffset, width, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}
te_void tea_tex_subimage2D(te_uint target, te_int level, te_int xoffset, te_int yoffset, te_sizei width, te_sizei height, const te_void *pixels) {
    CALL_GL(TexSubImage2D)(target, level, xoffset, yoffset, width, height, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}
te_void tea_tex_subimage3D(te_uint target, te_int level, te_int xoffset, te_int yoffset, te_int zoffset, te_sizei width, te_sizei height, te_sizei depth, const te_void *pixels) {
    //CALL_GL(TexSubImage3D)(target, level, xoffset, yoffset, zoffset, width, height, depth, TEA_RGBA, TEA_UNSIGNED_BYTE, pixels);
}

te_void tea_tex_parameteri(te_uint param, te_int value) {
    CALL_GL(TexParameteri)(TEA_TEXTURE_2D, param, value);
}
te_void tea_tex_parameteriv(te_uint param, te_int *value) {
    CALL_GL(TexParameteriv)(TEA_TEXTURE_2D, param, value);
}
te_void tea_tex_parameterf(te_uint param, te_float value) {
    CALL_GL(TexParameterf)(TEA_TEXTURE_2D, param, value);
}
te_void tea_tex_parameterfv(te_uint param, te_float *value) {
    CALL_GL(TexParameterfv)(TEA_TEXTURE_2D, param, value);
}

/*=====================================*
 *             Framebuffer             *
 *=====================================*/

te_fbo_t tea_fbo(te_void) {
    te_fbo_t fbo = 0;
    CALL_GL(GenFramebuffers)(1, &fbo);
    return fbo;
}
te_void tea_free_fbo(te_fbo_t fbo) {
    if (fbo != 0) CALL_GL(DeleteFramebuffers)(1, &fbo);
}

te_void tea_bind_fbo(te_uint target, te_fbo_t fbo) {
    CALL_GL(BindFramebuffer)(target, fbo);
}

te_void tea_fbo_texture(te_uint target, te_uint attachment, te_texture_t texture) {
    CALL_GL(FramebufferTexture2D)(target, attachment, TEA_TEXTURE_2D, texture, 0);
}
te_void tea_fbo_renderbuffer(te_uint target, te_uint attachment, te_rbo_t rbo) {
    CALL_GL(FramebufferRenderbuffer)(target, attachment, TEA_RENDERBUFFER, rbo);
}

/* Renderbuffer */
te_rbo_t tea_rbo(te_void);
te_void tea_free_rbo(te_rbo_t rbo);

/*=====================================*
 *               Buffer                *
 *=====================================*/

static te_void VertexPointer(te_uint size, te_uint type, te_uint stride, const te_void *pointer) {
    CALL_GL(VertexPointer)(size, type, stride, pointer);
}
static te_void ColorPointer(te_uint size, te_uint type, te_uint stride, const te_void *pointer) {
    CALL_GL(ColorPointer)(size, type, stride, pointer);
}
static te_void TexCoordPointer(te_uint size, te_uint type, te_uint stride, const te_void *pointer) {
    CALL_GL(TexCoordPointer)(size, type, stride, pointer);
}
static te_void NormalPointer(te_uint size, te_uint type, te_uint stride, const te_void *pointer) {
    CALL_GL(NormalPointer)(type, stride, pointer);
}

struct {
    te_ubyte size, stride;
    te_ushort type;
    te_uint client;
    te_void(*func)(te_uint, te_uint, te_uint, const te_void*);
} _attribInternal[] = {
    {2, 8, TEA_FLOAT, TEA_VERTEX_ARRAY, VertexPointer},
    {3, 12, TEA_FLOAT, TEA_VERTEX_ARRAY, VertexPointer},
    {4, 16, TEA_FLOAT, TEA_COLOR_ARRAY, ColorPointer},
    {2, 8, TEA_FLOAT, TEA_TEXTURE_COORD_ARRAY, TexCoordPointer},
    {3, 12, TEA_FLOAT, TEA_NORMAL_ARRAY, NormalPointer},
};

te_void tea_gen_buffers(te_uint size, te_buffer_t *buffers) {
    CALL_GL(GenBuffers)(size, buffers);
}

te_void tea_delete_buffers(te_uint size, te_buffer_t *buffers) {
    CALL_GL(DeleteBuffers)(size, buffers);
}

te_buffer_t tea_buffer(te_void) {
    te_buffer_t buffer = 0;
    tea_gen_buffers(1, &buffer);
    return buffer;
}

te_void tea_free_buffer(te_buffer_t *buffer) {
    if (!buffer)
        return;
    tea_delete_buffers(1, buffer);
}

te_void tea_bind_buffer(te_enum target, te_buffer_t buffer) {
    CALL_GL(BindBuffer)(target, buffer);
}

te_void tea_buffer_data(te_enum target, te_uint size, const te_void *data, te_uint usage) {
    CALL_GL(BufferData)(target, size, data, usage);
}
te_void tea_buffer_subdata(te_enum target, te_uint offset, te_uint size, const te_void *data) {
    CALL_GL(BufferSubData)(target, offset, size, data);
}

te_void* tea_map_buffer(te_enum target, te_enum access) {
    return CALL_GL(MapBuffer)(target, access);
}

te_bool tea_unmap_buffer(te_enum target) {
    return CALL_GL(UnmapBuffer)(target);
}

/*=====================================*
 *            Vertex Array             *
 *=====================================*/

#if 0
te_void s_enable_format(te_format_t *format) {
    te_int i;
    if (gl()->extensions & TEA_HAS_VBO &&
        gl()->extensions & TEA_HAS_VAO) {
        for (i = 0; i < format->count; i++) {
            te_attrib_t *attrib = &format->attribs[i];
            CALL_GL(EnableVertexAttribArray)(i);
            CALL_GL(VertexAttribPointer)(i, attrib->size, attrib->type, TEA_FALSE, format->stride, (te_void*)attrib->offset);
        }
    } else {
        for (int i = 0; i < format->count; i++) {
            te_attrib_t *attrib = &format->attribs[i];
            CALL_GL(EnableClientState)(_attribInternal[attrib->tag].client);
            _attribInternal[attrib->tag].func(attrib->size, attrib->type, format->stride, (te_void*)attrib->offset);
        }
    }
}

te_void s_disable_format(te_format_t *format) {
    te_int i;
    if (gl()->extensions & TEA_HAS_VBO &&
        gl()->extensions & TEA_HAS_VAO) {
        for (i = 0; i < format->count; i++)
            CALL_GL(DisableVertexAttribArray)(i);
    } else {
        for (i = 0; i < format->count; i++)
            CALL_GL(DisableClientState)(_attribInternal[format->attribs[i].tag].client);
    }
}

te_void tea_bind_format(te_format_t *format) {
    TEA_ASSERT(format != NULL, "invalid format");
    s_disable_format(&state()->vao.ptr->format);
    s_enable_format(format);
    memcpy(&state()->vao.ptr->format, format, sizeof(*format));
}
#endif

te_void tea_gen_vaos(te_uint size, te_vao_t *vao) {
    CALL_GL(GenVertexArrays)(size, vao);
}

te_void tea_delete_vaos(te_uint size, te_vao_t *vao) {
    CALL_GL(DeleteVertexArrays)(size, vao);
}

te_vao_t tea_vao(te_void) {
    te_vao_t vao = 0;
    tea_gen_vaos(1, &vao);
    return vao;
}
te_void tea_free_vao(te_vao_t *vao) {
    if (!vao)
        return;
    tea_delete_vaos(1, vao);
}

typedef te_void(*TeaBindVAOProc)(te_vao_t vao);
static TeaBindVAOProc s_tea_bind_vao;

static te_void s_tea_bind_vao1(te_vao_t vao) {
#if 0
    vao = vao ? vao : &s_default_vao;
    te_format_t *format = &state()->vao.ptr->format;
    for (te_int i = 0; i < format->count; i++) {
        te_attrib_t *attrib = &format->attribs[i];
        CALL_GL(DisableClientState)(_attribInternal[attrib->tag].client);
    }
    format = &vao->format;
    for (te_uint i = 0; i < format->count; i++) {
        te_attrib_t *attrib = &format->attribs[i];
        CALL_GL(EnableClientState)(_attribInternal[attrib->tag].client);
    }
    state()->vao.ptr = vao;
#endif
}

static te_void s_tea_bind_vao2(te_vao_t vao) {
#if 0
    vao = vao ? vao : &s_default_vao;
    for (te_uint i = 0; i < state()->vao.ptr->format.count; i++) {
        CALL_GL(DisableVertexAttribArray)(i);
    }

    for (te_uint i = 0; i < vao->format.count; i++)
        CALL_GL(EnableVertexAttribArray)(i);

    for (te_uint i = 0; i < 2; i++) {
        if (vao->buffers[i])
            CALL_GL(BindBuffer)(TEA_ARRAY_BUFFER+i, vao->buffers[i]->handle);
        else
            CALL_GL(BindBuffer)(TEA_ARRAY_BUFFER+i, 0);
    }
    if (vao->buffers[0]) {
        te_format_t *format = &vao->format;
        for (te_uint i = 0; i < format->count; i++) {
            te_attrib_t *attrib = &format->attribs[i];
            CALL_GL(VertexAttribPointer)(i, attrib->size, attrib->type, TEA_FALSE, format->stride, (te_void*)attrib->offset);
        }
    }
    state()->vao.ptr = vao;
#endif
}

static te_void s_tea_bind_vao3(te_vao_t vao) {
#if 0
    vao = vao ? vao : &s_default_vao;
    CALL_GL(BindVertexArray)(vao->handle);
    state()->vao.ptr = vao;
#endif
    CALL_GL(BindVertexArray)(vao);
}

te_void tea_bind_vao(te_vao_t vao) {
    s_tea_bind_vao(vao);
}

te_void tea_enable_attrib(te_uint index) {
    CALL_GL(EnableVertexAttribArray)(index);
}

te_void tea_disable_attrib(te_uint index) {
    CALL_GL(DisableVertexAttribArray)(index);
}

te_void tea_vertex_attrib_pointer(te_uint index, te_int size, te_enum type, te_bool normalized, te_sizei stride, const te_void *pointer) {
    CALL_GL(VertexAttribPointer)(index, size, type, normalized, stride, pointer);
}

te_void tea_draw_arrays(te_uint mode, te_int first, te_int count) {
    CALL_GL(DrawArrays)(mode, first, count);
}

te_void tea_draw_elements(te_uint mode, te_int count, te_uint type, const te_void *indices) {
    CALL_GL(DrawElements)(mode, count, type, indices);
}

/*=====================================*
 *               Shader                *
 *=====================================*/
#define TEA_COMPILE_STATUS 0x8B81
#define TEA_LINK_STATUS 0x8B82

const te_byte *shader_type_names[] = {
    "Fragment",
    "Vertex"
};

te_shader_t tea_shader(te_enum type, const te_byte *source) {
    TEA_ASSERT(type >= TEA_FRAGMENT_SHADER, "Invalid shader type\n");
    TEA_ASSERT(source != NULL, "%s shader source is NULL\n", shader_type_names[type - TEA_FRAGMENT_SHADER]);
    te_uint shader = CALL_GL(CreateShader)(type);
    CALL_GL(ShaderSource)(shader, 1, &source, NULL);
    CALL_GL(CompileShader)(shader);
    te_int status;
    //const char *shader_type_str = type == TEA_VERTEX_SHADER ? "vertex" : "fragment";
    CALL_GL(GetShaderiv)(shader, TEA_COMPILE_STATUS, &status);
    if (status == 0) {
        char log[1024];
        CALL_GL(GetShaderInfoLog)(shader, 1024, NULL, log);
        TEA_ASSERT(0, "%s shader compile error: %s", shader_type_names[type - TEA_FRAGMENT_SHADER], log);
    }
    return shader;
}

te_void tea_delete_shader(te_shader_t shader) {
    if (!shader)
        return;
    CALL_GL(DeleteShader)(shader);
}

te_program_t tea_program(te_uint count, te_shader_t *shaders) {
    TEA_ASSERT(shaders != NULL, "Shaders cannot be NULL\n");
    te_program_t program = CALL_GL(CreateProgram)();
    for (te_int i = 0; i < count; i++)
        CALL_GL(AttachShader)(program, shaders[i]);

    CALL_GL(LinkProgram)(program);
    te_int status;
    CALL_GL(GetProgramiv)(program, TEA_LINK_STATUS, &status);
    if (status == 0) {
        char log[1024];
        CALL_GL(GetProgramInfoLog)(program, 1024, NULL, log);
        TEA_ASSERT(0, "program link error: %s", log);
    }

    return program;
}

te_void tea_free_program(te_program_t *program) {
    if (!program)
        return;

    CALL_GL(DeleteProgram)(*program);
    *program = 0;
}

te_void tea_use_program(te_program_t program) {
    CALL_GL(UseProgram)(program);
}

te_int tea_get_uniform_location(te_program_t shader, const te_byte *name) {
    if (gl()->extensions & TEA_HAS_SHADER)
        return CALL_GL(GetUniformLocation)(shader, name);
    return -1;
}

#define TEA_UNIFORM_IMPL_X(X, T)\
te_void tea_uniform1##X(te_int location, T v) { CALL_GL(Uniform1##X)(location, v); }\
te_void tea_uniform2##X(te_int location, T v0, T v1) { CALL_GL(Uniform2##X)(location, v0, v1); }\
te_void tea_uniform3##X(te_int location, T v0, T v1, T v2) { CALL_GL(Uniform3##X)(location, v0, v1, v2); }\
te_void tea_uniform4##X(te_int location, T v0, T v1, T v2, T v3) { CALL_GL(Uniform4##X)(location, v0, v1, v2, v3); }\
te_void tea_uniform1##X##v(te_int location, te_int value, const T *v) { CALL_GL(Uniform1##X##v)(location, value, v); }\
te_void tea_uniform2##X##v(te_int location, te_int value, const T *v) { CALL_GL(Uniform2##X##v)(location, value, v); }\
te_void tea_uniform3##X##v(te_int location, te_int value, const T *v) { CALL_GL(Uniform3##X##v)(location, value, v); }\
te_void tea_uniform4##X##v(te_int location, te_int value, const T *v) { CALL_GL(Uniform4##X##v)(location, value, v); }

TEA_UNIFORM_IMPL_X(f, te_float);
TEA_UNIFORM_IMPL_X(i, te_int);

te_void tea_uniform_matrix2fv(te_int location, te_int count, te_bool transpose, const te_float *m) {
    CALL_GL(UniformMatrix2fv)(location, count, transpose, m);
}
te_void tea_uniform_matrix3fv(te_int location, te_int count, te_bool transpose, const te_float *m) {
    CALL_GL(UniformMatrix3fv)(location, count, transpose, m);
}
te_void tea_uniform_matrix4fv(te_int location, te_int count, te_bool transpose, const te_float *m) {
    CALL_GL(UniformMatrix4fv)(location, count, transpose, m);
}

/*=====================================*
 *               Format                *
 *=====================================*/

te_void s_enable_vertex_format(te_vertex_format_t *format) {
    te_int i;
    for (i = 0; i < format->count; i++) {
        te_attrib_t *attrib = &format->attribs[i];
        CALL_GL(EnableVertexAttribArray)(i);
        CALL_GL(VertexAttribPointer)(i, attrib->size, attrib->type, TEA_FALSE, format->stride, (te_void*)attrib->offset);
    }
}

te_void s_disable_vertex_format(te_vertex_format_t *format) {
    te_int i;
    for (i = 0; i < format->count; i++)
            CALL_GL(DisableVertexAttribArray)(i);
}

te_vertex_format_t* tea_vertex_format(te_void) {
    te_vertex_format_t *format = (te_vertex_format_t*)malloc(sizeof *format);
    TEA_ASSERT(format != NULL, "Cannot alloc memory for vertex format\n");
    memset(format, 0, sizeof *format);
    //format->handle = tea_vao();
    return format;
}

te_void tea_free_vertex_format(te_vertex_format_t *format) {
    if (!format)
        return;
    //tea_free_vao(&format->handle);
    free(format);
}

te_void tea_begin_vertex_format(te_vertex_format_t *format) {
    TEA_ASSERT(format != NULL, "Format cannot be NULL\n");
    TEA_ASSERT(state()->vertex.format == NULL, "You must dettach the other vertex format first, use 'tea_end_vertex_format(te_vertex_format_t*)'\n");
    //tea_bind_vao(format->handle);
    //s_disable_vertex_format(format);
    format->count = 0;
    format->stride = 0;
    state()->vertex.format = format;
}

te_void tea_vertex_format_add_attrib(te_enum tag) {
    te_vertex_format_t *format = state()->vertex.format;
    TEA_ASSERT(format != NULL, "You must attach an vertex format first, use 'tea_begin_vertex_format(te_vertex_format_t*)'\n");
    TEA_ASSERT(format->count < TEA_MAX_ATTRIBS, "too many attributes");
    te_attrib_t *attrib = &format->attribs[format->count];
    attrib->tag = tag;
    attrib->type = _attribInternal[tag].type;
    attrib->size = _attribInternal[tag].size;
    attrib->stride = _attribInternal[tag].stride;
    attrib->offset = format->stride;

    format->count++;
    format->stride += attrib->stride;
}

te_void tea_end_vertex_format(te_vertex_format_t *format) {
    TEA_ASSERT(format != NULL, "Format cannot be NULL\n");
    state()->vertex.format = NULL;
    //s_enable_vertex_format(format);
    //tea_bind_vao(0);
}

/*=====================================*
 *               Vertex                *
 *=====================================*/

te_vertex_t* tea_vertex(te_vertex_format_t *format, te_int vertices) {
    te_vertex_t *vertex = (te_vertex_t*)malloc(sizeof(*vertex));
    TEA_ASSERT(vertex != NULL, "Cannot alloc memory for te_vertex_t\n");
    memset(vertex, 0, sizeof(*vertex));
    format = format ? format : base()->format;
    vertex->format = format;
    vertex->vao = tea_vao();
    vertex->vbo = tea_buffer();
    vertex->size = (vertices * vertex->format->stride);
    vertex->data = malloc(vertex->size);
    te_vao_t vao = vertex->vao;
    TEA_ASSERT(vertex->data != NULL, "Cannot alloc memory for vertex data\n");
    tea_bind_vao(vao);
    tea_bind_buffer(TEA_ARRAY_BUFFER, vertex->vbo);
    s_enable_vertex_format(vertex->format);
    tea_buffer_data(TEA_ARRAY_BUFFER, vertex->size, NULL, TEA_DYNAMIC_DRAW);
    tea_bind_vao(0);
    
    return vertex;
}

te_void tea_free_vertex(te_vertex_t *vertex) {
    if (!vertex)
        return;

    tea_free_vao(&vertex->vao);
    tea_free_buffer(&vertex->vbo);
    free(vertex);
}

te_void tea_begin_vertex(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    state()->vertex.ptr = vertex;
    vertex->offset = 0;
}

te_void tea_end_vertex(te_vertex_t *vertex) {
    tea_flush_vertex(vertex);
    state()->vertex.ptr = &s_default_vertex;
}

te_void tea_flush_vertex(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    te_vertex_format_t *format = vertex->format;
    tea_bind_buffer(TEA_ARRAY_BUFFER, vertex->vbo);
    tea_buffer_subdata(TEA_ARRAY_BUFFER, 0, vertex->offset, vertex->data);
    tea_bind_buffer(TEA_ARRAY_BUFFER, 0);
}

te_void tea_draw_vertex(te_vertex_t *vertex, te_enum mode) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    te_uint size = 0;
    te_vertex_format_t *format = vertex->format;
    if (format->stride != 0)
        size = vertex->offset / format->stride;
    te_vao_t vao = vertex->vao;
    //fprintf(stderr, "vao: %d\n", vao);
    //fprintf(stderr, "vbo: %d\n", vertex->vbo);
    //te_float *data = vertex->data;
    //printf("%f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4]);
    tea_bind_vao(vao);
    // tea_bind_buffer(TEA_ARRAY_BUFFER, vertex->vbo);
    tea_draw_arrays(mode, 0, size);
    tea_bind_vao(0);
}

te_void tea_draw_vertex_part(te_vertex_t *vertex, te_enum mode, te_int start, te_int size) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");

}

te_bool tea_vertex_is_empty(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    return vertex->offset == 0;
}

te_int tea_vertex_offset(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    return vertex->offset;
}

te_int tea_vertex_size(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    return vertex->size;
}

te_int tea_vertex_vertices(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    te_uint size = 0;
    te_vertex_format_t *format = vertex->format;
    if (format->stride != 0)
        size = vertex->size / format->stride;
    return size;
}

te_int tea_vertex_vertice_offset(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    te_uint size = 0;
    te_vertex_format_t *format = vertex->format;
    if (format->stride != 0)
        size = vertex->offset / format->stride;
    return size;
}

te_void tea_vertex_grow(te_vertex_t *vertex) {
    TEA_ASSERT(vertex != NULL, "Vertex cannot be NULL");
    vertex->size = vertex->size * 2;
    vertex->data = realloc(vertex->data, vertex->size);
}

/*=====================================*
 *                Debug                *
 *=====================================*/
#include <stdarg.h>

te_void tea_abort(const te_byte *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

/*=====================================*
 *               Loader                *
 *=====================================*/

static te_bool s_tea_load_procs(te_proc_t *procs, te_uint flags);

#if !defined(__APPLE__) && !defined(__HAIKU__)
te_void* (*s_proc_address)(const te_byte*);
#endif

te_bool s_load_gl(te_void) {
#if defined(_WIN32)
    s_glsym = LoadLibrary("opengl32.dll");
    TEA_ASSERT(s_glsym != NULL, "failed to load OpenGL32.dll");
#else
#if defined(__APPLE__)
    const char *names[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/Opengl",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL",
        NULL,
    };
#else
    const char *names[] = {
        "libGL.so.1",
        "libGL.so",
        NULL,
    };
#endif
    te_uint index;
    for (index = 0; names[index] != NULL; ++index) {
        s_glsym = dlopen(names[index], RTLD_LAZY | RTLD_GLOBAL);
        if (s_glsym != NULL) {
#if defined(__APPLE__) || defined(__HAIKU__)
            return TEA_TRUE;
#else
            s_proc_address = (te_void*(*)(const te_byte*))dlsym(s_glsym, "glXGetProcAddress");
            return s_proc_address != NULL;
#endif
            break;
        }
    }
#endif
    return TEA_FALSE;
}

te_void s_close_gl(te_void) {
    if (s_glsym != NULL) {
#if defined(_WIN32)
        FreeLibrary(s_glsym);
#else
        dlclose(s_glsym);
#endif
        s_glsym = NULL;
    }
}

te_void s_setup_gl(te_void) {
    GET_GL(GetString) = s_get_proc("glGetString");
    GET_GL(GetStringi) = s_get_proc("glGetStringi");

    const te_byte *version = (const te_byte*)CALL_GL(GetString)(TEA_GL_VERSION);
    const te_byte *glsl = (const te_byte*)CALL_GL(GetString)(TEA_GL_SHADING_LANGUAGE_VERSION);
    TEA_ASSERT(version != NULL, "Failed to get OpenGL version");
    const te_byte *prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL,
    };

    te_byte *ver = (te_byte*)version;
    for (te_uint i = 0; prefixes[i] != NULL; i++) {
        if (strncmp(ver, prefixes[i], strlen(prefixes[i])) == 0) {
            ver += strlen(prefixes[i]);
            gl()->version.es = TEA_TRUE;
            break;
        }
    }
    s_gl_max_ver.mag = ver[0] - '0';
    s_gl_max_ver.min = ver[2] - '0';
    if (gl()->version.major == 0) {
        gl()->version.major = s_gl_max_ver.mag;
        gl()->version.minor = s_gl_max_ver.min;
    }

    fprintf(stderr, "OpenGL version: %s\n", version);
    fprintf(stderr, "OpenGL shading language version: %s\n", glsl);
    static const te_proc_t glBaseProcs[] = {
        /* Miscellaneous */
        { TEA_GL_ClearColor, { "glClearColor", NULL }},
        { TEA_GL_ClearDepth, { "glClearDepth", NULL }},
        { TEA_GL_Clear, { "glClear", NULL }},
        { TEA_GL_CullFace, { "glCullFace", NULL }},
        { TEA_GL_FrontFace, { "glFrontFace", NULL }},
        { TEA_GL_PolygonMode, { "glPolygonMode", NULL }},
        { TEA_GL_Scissor, { "glScissor", NULL }},
        { TEA_GL_ReadBuffer, { "glReadBuffer", NULL }},
        { TEA_GL_DrawBuffer, { "glDrawBuffer", NULL }},
        { TEA_GL_Enable, { "glEnable", NULL }},
        { TEA_GL_Disable, { "glDisable", NULL }},
        { TEA_GL_EnableClientState, { "glEnableClientState", NULL }},
        { TEA_GL_DisableClientState, { "glDisableClientState", NULL }},
        { TEA_GL_GetBooleanv, { "glGetBooleanv", NULL }},
        { TEA_GL_GetFloatv, { "glGetFloatv", NULL }},
        { TEA_GL_GetIntegerv, { "glGetIntegerv", NULL }},
        { TEA_GL_GetError, { "glGetError" }},
        /* Depth */
        { TEA_GL_DepthFunc, { "glDepthFunc", NULL }},
        { TEA_GL_DepthMask, { "glDepthMask", NULL }},
        { TEA_GL_DepthRange, { "glDepthRange", NULL }},
        /* Transformation */
        { TEA_GL_Viewport, { "glViewport", NULL }},
        { TEA_GL_MatrixMode, { "glMatrixMode", NULL }},
        { TEA_GL_LoadIdentity, { "glLoadIdentity", NULL }},
        { TEA_GL_LoadMatrixf, { "glLoadMatrixf", NULL }},
        { TEA_GL_LoadMatrixd, { "glLoadMatrixd", NULL }},
        { TEA_GL_MultMatrixf, { "glMultMatrixf", NULL }},
        { TEA_GL_MultMatrixd, { "glMultMatrixd", NULL }},
        { TEA_GL_Rotatef, { "glRotatef", NULL }},
        { TEA_GL_Rotated, { "glRotated", NULL }},
        { TEA_GL_Scalef, { "glScalef", NULL }},
        { TEA_GL_Scaled, { "glScaled", NULL }},
        { TEA_GL_Translatef, { "glTranslatef", NULL }},
        { TEA_GL_Translated, { "glTranslated", NULL }},
        { TEA_GL_Ortho, { "glOrtho", NULL }},
        { TEA_GL_Frustum, { "glFrustum", NULL }},
        { TEA_GL_Orthof, { "glOrthof", NULL }},
        { TEA_GL_Frustumf, { "glFrustumf", NULL }},
        { TEA_GL_PushMatrix, { "glPushMatrix", NULL }},
        { TEA_GL_PopMatrix, { "glPopMatrix", NULL }},
        { TEA_GL_LoadTransposeMatrixd, { "glLoadTransposeMatrixd", NULL }},
        { TEA_GL_LoadTransposeMatrixf, { "glLoadTransposeMatrixf", NULL }},
        { TEA_GL_MultTransposeMatrixd, { "glMultTransposeMatrixd", NULL }},
        { TEA_GL_MultTransposeMatrixf, { "glMultTransposeMatrixf", NULL }},
        /* Vertex arrays */
        { TEA_GL_VertexPointer, { "glVertexPointer", NULL }},
        { TEA_GL_NormalPointer, { "glNormalPointer", NULL }},
        { TEA_GL_ColorPointer, { "glColorPointer", NULL }},
        { TEA_GL_TexCoordPointer, { "glTexCoordPointer", NULL }},
        { TEA_GL_IndexPointer, { "glIndexPointer", NULL }},
        { TEA_GL_EdgeFlagPointer, { "glEdgeFlatPointer", NULL }},
        { TEA_GL_DrawArrays, { "glDrawArrays", NULL }},
        { TEA_GL_DrawElements, { "glDrawElements", NULL }},
    #if 0
        { TEA_GL_InterleavedArrays, { "glInterleavedArrays", NULL }},
        { TEA_GL_ClientActiveTexture, { "glClientActiveTexture", NULL }},
        { TEA_GL_ActiveTexture, { "glActiveTexture", NULL }},
        { TEA_GL_MultiTexCoord1d, { "glMultiTexCoord1d", NULL }},
        { TEA_GL_MultiTexCoord1dv, { "glMultiTexCoord1dv", NULL }},
    #endif
        /* Texture mapping */
        { TEA_GL_TexParameterf, { "glTexParameterf", NULL }},
        { TEA_GL_TexParameteri, { "glTexParameteri", NULL } },
        { TEA_GL_TexParameterfv, { "glTexParameterfv", NULL }},
        { TEA_GL_TexParameteriv, { "glTexParameteriv", NULL }},
        { TEA_GL_GetTexParameterfv, { "glGetTexParameterfv", NULL }},
        { TEA_GL_GetTexParameteriv, { "glGetTexParameteriv", NULL }},
        { TEA_GL_GenTextures, { "glGenTextures", NULL }},
        { TEA_GL_DeleteTextures, { "glDeleteTextures", NULL }},
        { TEA_GL_BindTexture, { "glBindTexture", NULL }},
        { TEA_GL_IsTexture, { "glIsTexture", NULL }},
        { TEA_GL_TexImage1D, { "glTexImage1D", NULL }},
        { TEA_GL_TexImage2D, { "glTexImage2D", NULL }},
        { TEA_GL_TexSubImage1D, { "glTexSubImage1D", NULL }},
        { TEA_GL_TexSubImage2D, { "glTexSubImage2D", NULL }},
        { TEA_GL_CopyTexImage1D, { "glCopyTexImage1D", NULL }},
        { TEA_GL_CopyTexImage2D, { "glCopyTexImage2D", NULL }},
        { TEA_GL_CopyTexSubImage1D, { "glCopyTexSubImage1D", NULL }},
        { TEA_GL_CopyTexSubImage2D, { "glCopyTexSubImage2D", NULL }},
        { TEA_GL_TexImage3D, { "glTexImage3D", NULL }},
        { TEA_GL_TexSubImage3D, { "glTexSubImage3D", NULL }},
        { TEA_GL_CopyTexSubImage3D, { "glCopyTexSubImage3D", NULL }},
        /* Vertex buffer object */
        { TEA_GL_GenBuffers, { "glGenBuffers", "glGenBuffersARB", NULL }},
        { TEA_GL_DeleteBuffers, { "glDeleteBuffers", "glDeleteBuffersARB", NULL }},
        { TEA_GL_BindBuffer, { "glBindBuffer", "glBindBufferARB", NULL }},
        { TEA_GL_BufferData, { "glBufferData", "glBufferDataARB", NULL }},
        { TEA_GL_BufferSubData, { "glBufferSubData", "glBufferSubDataARB", NULL }},
        { TEA_GL_MapBuffer, { "glMapBuffer", "glMapBufferARB", NULL }},
        { TEA_GL_UnmapBuffer, { "glUnmapBuffer", "glUnmapBufferARB", NULL }},
        /* Vertex program */
        { TEA_GL_VertexAttribPointer, { "glVertexAttribPointer", "glVertexAttribPointerARB", NULL }},
        { TEA_GL_EnableVertexAttribArray, { "glEnableVertexAttribArray", "glEnableVertexAttribArrayARB", NULL }},
        { TEA_GL_DisableVertexAttribArray, { "glDisableVertexAttribArray", "glDisableVertexAttribArrayARB", NULL }},
        /* Framebuffer */
        { TEA_GL_IsRenderbuffer, { "glIsRenderbuffer", "glIsRenderbufferEXT", NULL }},
        { TEA_GL_BindRenderbuffer, { "glBindRenderbuffer", "glBindRenderbufferEXT", NULL }},
        { TEA_GL_DeleteRenderbuffers, { "glDeleteRenderbuffers", "glDeleteRenderbuffersEXT", NULL }},
        { TEA_GL_GenRenderbuffers, { "glGenRenderbuffers", "glGenRenderbuffersEXT", NULL }},
        { TEA_GL_RenderbufferStorage, { "glRenderbufferStorage", "glRenderbufferStorageEXT", NULL }},
        { TEA_GL_IsFramebuffer, { "glIsFramebuffer", "glIsFramebufferEXT", NULL }},
        { TEA_GL_BindFramebuffer, { "glBindFramebuffer", "glBindFramebufferEXT", NULL }},
        { TEA_GL_DeleteFramebuffers, { "glDeleteFramebuffers", "glDeleteFramebuffersEXT", NULL }},
        { TEA_GL_GenFramebuffers, { "glGenFramebuffers", "glGenFramebuffersEXT", NULL }},
        { TEA_GL_CheckFramebufferStatus, { "glCheckFramebufferStatus", "glCheckFramebufferStatusEXT", NULL }},
        { TEA_GL_FramebufferTexture2D, { "glFramebufferTexture2D", "glFramebufferTexture2DEXT", NULL }},
        { TEA_GL_FramebufferRenderbuffer, { "glFramebufferRenderbuffer", "glFramebufferRenderbufferEXT", NULL }},
        { TEA_GL_GenerateMipmap, { "glGenerateMipmap", "glGenerateMipmapEXT", NULL }},
        { TEA_GL_BlitFramebuffer, { "glBlitFramebuffer", "glBlitFramebufferEXT", NULL }},
        /* Shader */
        { TEA_GL_CreateShader, { "glCreateShader", "glCreateShaderObjectARB", NULL }},
        { TEA_GL_DeleteShader, { "glDeleteShader", "glDeleteObjectARB", NULL }},
        { TEA_GL_ShaderSource, { "glShaderSource", "glShaderSourcerARB", NULL }},
        { TEA_GL_CompileShader, { "glCompileShader", "glCompileShaderARB", NULL }},
        { TEA_GL_GetShaderiv, { "glGetShaderiv", "glGetObjectParameterivARB", NULL }},
        { TEA_GL_GetShaderInfoLog, { "glGetShaderInfoLog", "glGetInfoLogARB", NULL }},
        { TEA_GL_CreateProgram, { "glCreateProgram", "glCreateProgramObjectARB", NULL }},
        { TEA_GL_DeleteProgram, { "glDeleteProgram", "glDeleteObjectARB", NULL }},
        { TEA_GL_AttachShader, { "glAttachShader", "glAttachObjectARB", NULL }},
        { TEA_GL_DetachShader, { "glDetachShader", "glDetachObjectARB", NULL }},
        { TEA_GL_LinkProgram, { "glLinkProgram", "glLinkProgramARB", NULL }},
        { TEA_GL_GetProgramiv, { "glGetProgramiv", "glGetObjectParameterivARB", NULL }},
        { TEA_GL_GetProgramInfoLog, { "glGetProgramInfoLog", "glGetInfoLogARB", NULL }},
        { TEA_GL_UseProgram, { "glUseProgram", "glUseProgramObjectARB", NULL }},
        { TEA_GL_GetUniformLocation, { "glGetUniformLocation", "glGetUniformLocationARB", NULL }},
        { TEA_GL_Uniform1f, { "glUniform1f", "glUniform1fARB", NULL }},
        { TEA_GL_Uniform2f, { "glUniform2f", "glUniform2fARB", NULL }},
        { TEA_GL_Uniform3f, { "glUniform3f", "glUniform3fARB", NULL }},
        { TEA_GL_Uniform4f, { "glUniform4f", "glUniform4fARB", NULL }},
        { TEA_GL_Uniform1i, { "glUniform1i", "glUniform1iARB", NULL }},
        { TEA_GL_Uniform2i, { "glUniform2i", "glUniform2iARB", NULL }},
        { TEA_GL_Uniform3i, { "glUniform3i", "glUniform3iARB", NULL }},
        { TEA_GL_Uniform4i, { "glUniform4i", "glUniform4iARB", NULL }},
        { TEA_GL_Uniform1fv, { "glUniform1fv", "glUniform1fvARB", NULL }},
        { TEA_GL_Uniform2fv, { "glUniform2fv", "glUniform2fvARB", NULL }},
        { TEA_GL_Uniform3fv, { "glUniform3fv", "glUniform3fvARB", NULL }},
        { TEA_GL_Uniform4fv, { "glUniform4fv", "glUniform4fvARB", NULL }},
        { TEA_GL_Uniform1iv, { "glUniform1iv", "glUniform1ivARB", NULL }},
        { TEA_GL_Uniform2iv, { "glUniform2iv", "glUniform2ivARB", NULL }},
        { TEA_GL_Uniform3iv, { "glUniform3iv", "glUniform3ivARB", NULL }},
        { TEA_GL_Uniform4iv, { "glUniform4iv", "glUniform4ivARB", NULL }},
        { TEA_GL_UniformMatrix2fv, { "glUniformMatrix2fv", "glUniformMatrix2fvARB", NULL }},
        { TEA_GL_UniformMatrix3fv, { "glUniformMatrix3fv", "glUniformMatrix3fvARB", NULL }},
        { TEA_GL_UniformMatrix4fv, { "glUniformMatrix4fv", "glUniformMatrix4fvARB", NULL }},
        { TEA_GL_GetAttribLocation, { "glGetAttribLocation", "glGetAttribLocationARB", NULL }},
        { TEA_GL_BindAttribLocation, { "glBindAttribLocation", "glBindAttribLocationARB", NULL }},
        { TEA_GL_GetActiveUniform, { "glGetActiveUniform", "glGetActiveUniformARB", NULL }},
        { TEA_GL_GetActiveAttrib, { "glGetActiveAttrib", "glGetActiveAttribARB", NULL }},
        /* Vertex array object */
        { TEA_GL_GenVertexArrays, { "glGenVertexArrays", NULL }},
        { TEA_GL_DeleteVertexArrays, { "glDeleteVertexArrays", NULL }},
        { TEA_GL_BindVertexArray, { "glBindVertexArray", NULL }},
        { 0, { NULL }}
    };

    s_tea_load_procs((te_proc_t*)glBaseProcs, TEA_PROC_OVERRIDE);

    gl()->extensions |= TEA_HAS_VAO * (GET_GL(GenVertexArrays) != NULL);
    gl()->extensions |= TEA_HAS_VBO * (GET_GL(GenBuffers) != NULL);
    gl()->extensions |= TEA_HAS_SHADER * (GET_GL(CreateShader) != NULL);

    if (gl()->extensions & TEA_HAS_VAO) {
        s_tea_bind_vao = s_tea_bind_vao3;
    } else if (gl()->extensions & TEA_HAS_VBO) {
        s_tea_bind_vao = s_tea_bind_vao2;
    } else {
        s_tea_bind_vao = s_tea_bind_vao1;
    }

    if (gl()->extensions & TEA_HAS_SHADER) {
        fprintf(stderr, "Using shader\n");
        GET_GL(MatrixMode) = s_tea_matrix_mode;
        GET_GL(PushMatrix) = s_tea_push_matrix;
        GET_GL(PopMatrix) = s_tea_pop_matrix;
        GET_GL(LoadIdentity) = s_tea_load_identity;

        GET_GL(LoadMatrixf) = s_tea_load_matrixf;
        GET_GL(LoadTransposeMatrixf) = s_tea_load_transpose_matrixf;
        GET_GL(MultMatrixf) = s_tea_mult_matrixf;
        GET_GL(MultTransposeMatrixf) = s_tea_mult_transpose_matrixf;
        GET_GL(Translatef) = s_tea_translatef;
        GET_GL(Scalef) = s_tea_scalef;
        GET_GL(Rotatef) = s_tea_rotatef;
        GET_GL(Orthof) = s_tea_orthof;
        GET_GL(Frustumf) = s_tea_frustumf;

        GET_GL(LoadMatrixd) = s_tea_load_matrixd;
        GET_GL(LoadTransposeMatrixd) = s_tea_load_transpose_matrixd;
        GET_GL(MultMatrixd) = s_tea_mult_matrixd;
        GET_GL(MultTransposeMatrixd) = s_tea_mult_transpose_matrixd;
        GET_GL(Translated) = s_tea_translated;
        GET_GL(Scaled) = s_tea_scaled;
        GET_GL(Rotated) = s_tea_rotated;

        GET_GL(Ortho) = s_tea_orthod;
        GET_GL(Frustum) = s_tea_frustumd;
    }
}

te_void* s_get_proc(const te_byte *name) {
    te_void *sym = NULL;
    if (s_glsym == NULL) return sym;
#if !defined(__APPLE__) && !defined(__HAIKU__)
    if (s_proc_address != NULL) {
        sym = s_proc_address(name);
    }
#endif
    if (sym == NULL) {
#if defined(_WIN32) || defined(__CYGWIN__)
        sym = (te_void*)GetProcAddress(s_glsym, name);
#else
        sym = (te_void*)dlsym(s_glsym, name);
#endif
    }
    return sym;
}

te_bool s_tea_load_procs(te_proc_t *procs, te_uint flags) {
    te_proc_t *proc = procs;
    while (proc->names[0]) {
        if (!gl()->procs[proc->tag] || (flags & TEA_PROC_OVERRIDE)) {
            te_uint i = 0;
            te_byte **names = (te_byte**)proc->names;
            while (names[i] &&  i < 3) {
                if ((gl()->procs[proc->tag] = s_get_proc(names[i])))
                    break;
                i++;
            }
        }
        else if (flags & TEA_PROC_RET_ON_DUP)
            return TEA_TRUE;

        proc++;
    }
    return TEA_TRUE;
}
