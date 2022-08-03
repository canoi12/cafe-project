#include "tea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GET_GL(name)\
(gl()->procs[TEA_GL_##name])

#define CALL_GL(name)\
((GL##name##Proc)GET_GL(name))

#define GET_BATCH_DATA(b, T) (T)((te_i8*)(b)->data + (b)->offset)

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
    static void *s_glsym;
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
#define buffer() ()

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
    TEA_GL_Gette_f32v,
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
typedef void(*GLClearColorProc)(te_f32, te_f32, te_f32, te_f32);
typedef void(*GLClearProc)(te_u32);
typedef void(*GLBlendFuncProc)(te_u32, te_u32);
typedef void(*GLLogicOpProc)(te_u32);
typedef void(*GLCullFaceProc)(te_u32);
typedef void(*GLFrontFaceProc)(te_u32);
typedef void(*GLPolygonModeProc)(te_u32, te_u32);
typedef void(*GLScissorProc)(te_i32, te_i32, te_i32, te_i32);
typedef void(*GLDrawBufferProc)(te_u32);
typedef void(*GLReadBufferProc)(te_u32);
typedef void(*GLEnableProc)(te_u32);
typedef void(*GLDisableProc)(te_u32);

typedef void(*GLEnableClientStateProc)(te_u32); /* 1.1 */
typedef void(*GLDisableClientStateProc)(te_u32); /* 1.1 */

typedef void(*GLGetBooleanvProc)(te_u32, te_bool*);
typedef void(*GLGetDoublevProc)(te_u32, te_f64*);
typedef void(*GLGette_f32vProc)(te_u32, te_f32*);
typedef void(*GLGetIntegervProc)(te_u32, te_i32*);
typedef void(*GLGetErrorProc)(void);
typedef const te_u8*(*GLGetStringProc)(te_u32);

typedef const te_u8*(*GLGetStringiProc)(te_u32, te_u32); /* 3.0 */

/* Depth buffer */
typedef void(*GLClearDepthProc)(te_f32);
typedef void(*GLDepthFuncProc)(te_u32);
typedef void(*GLDepthMaskProc)(te_bool);
typedef void(*GLDepthRangeProc)(te_f64, te_f64);

/* Transformation */
typedef void(*GLViewportProc)(te_i32, te_i32, te_i32, te_i32);
typedef void(*GLMatrixModeProc)(te_u32);
typedef void(*GLPushMatrixProc)(void);
typedef void(*GLPopMatrixProc)(void);
typedef void(*GLLoadIdentityProc)(void);
typedef void(*GLLoadMatrixfProc)(const te_f32*);
typedef void(*GLLoadMatrixdProc)(const te_f64*);
typedef void(*GLMultMatrixfProc)(const te_f32*);
typedef void(*GLMultMatrixdProc)(const te_f64*);
typedef void(*GLOrthoProc)(te_f64, te_f64, te_f64, te_f64, te_f64, te_f64);
typedef void(*GLFrustumProc)(te_f64, te_f64, te_f64, te_f64, te_f64, te_f64);
typedef void(*GLTranslatefProc)(te_f32, te_f32, te_f32);
typedef void(*GLRotatefProc)(te_f32, te_f32, te_f32, te_f32);
typedef void(*GLScalefProc)(te_f32, te_f32, te_f32);
typedef void(*GLTranslatedProc)(te_f64, te_f64, te_f64);
typedef void(*GLRotatedProc)(te_f64, te_f64, te_f64, te_f64);
typedef void(*GLScaledProc)(te_f64, te_f64, te_f64);

typedef void(*GLLoadTransposeMatrixdProc)(const te_f64[16]); /* 1.3 */
typedef void(*GLLoadTransposeMatrixfProc)(const te_f32[16]); /* 1.3 */
typedef void(*GLMultTransposeMatrixdProc)(const te_f64[16]); /* 1.3 */
typedef void(*GLMultTransposeMatrixfProc)(const te_f32[16]); /* 1.3 */

/* Vertex Arrays */
typedef void(*GLVertexPointerProc)(te_i32, te_u32, te_i32, const void*);
typedef void(*GLColorPointerProc)(te_i32, te_u32, te_i32, const void*);
typedef void(*GLTexCoordPointerProc)(te_i32, te_u32, te_i32, const void*);
typedef void(*GLNormalPointerProc)(te_u32, te_i32, const void*);
typedef void(*GLIndexPointerProc)(te_u32, te_i32, const void*);
typedef void(*GLEdgeFlagPointerProc)(te_i32, te_i32, const void*);

typedef void(*GLDrawArraysProc)(te_u32, te_i32, te_i32);
typedef void(*GLDrawElementsProc)(te_u32, te_i32, te_u32, const void*);

/* Texture mapping */
typedef void(*GLTexParameterfProc)(te_u32, te_u32, te_f32);
typedef void(*GLTexParameteriProc)(te_u32, te_u32, te_i32);
typedef void(*GLTexParameterfvProc)(te_u32, te_u32, const te_f32*);
typedef void(*GLTexParameterivProc)(te_u32, te_u32, const te_i32*);

typedef void(*GLGetTexParameterfProc)(te_u32, te_u32, te_f32*);
typedef void(*GLGetTexParameteriProc)(te_u32, te_u32, te_i32*);
typedef void(*GLGetTexParameterfvProc)(te_u32, te_u32, te_f32*);
typedef void(*GLGetTexParameterivProc)(te_u32, te_u32, te_i32*);

typedef void(*GLGenTexturesProc)(te_u32, te_u32*);
typedef void(*GLDeleteTexturesProc)(te_u32, te_u32*);
typedef void(*GLBindTextureProc)(te_u32, te_u32);
typedef te_bool(*GLIsTextureProc)(te_u32);
typedef void(*GLTexImage1DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLTexImage2DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLTexImage3DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLTexSubImage1DProc)(te_u32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLTexSubImage2DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLTexSubImage3DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_u32, te_u32, const void*);
typedef void(*GLCopyTexImage1DProc)(te_u32, te_i32, te_u32, te_i32, te_i32, te_i32, te_i32);
typedef void(*GLCopyTexImage2DProc)(te_u32, te_i32, te_u32, te_i32, te_i32, te_i32, te_i32, te_i32);
typedef void(*GLCopyTexSubImage1DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32);
typedef void(*GLCopyTexSubImage2DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32);
typedef void(*GLCopyTexSubImage3DProc)(te_u32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32);

/* GL_ARB_vertex_buffer_object */
typedef void(*GLBindBufferProc)(te_u32, te_u32);
typedef void(*GLBufferDataProc)(te_u32, te_u32, const void*, te_u32);
typedef void(*GLBufferSubDataProc)(te_u32, te_u32, te_u32, const void*);
typedef void(*GLGenBuffersProc)(te_u32, te_u32*);
typedef void(*GLDeleteBuffersProc)(te_u32, te_u32*);
typedef void*(*GLMapBufferProc)(te_u32, te_u32);
typedef te_u32(*GLUnmapBufferProc)(te_u32);

/* GL_ARB_vertex_array_object */
typedef void(*GLGenVertexArraysProc)(te_u32, te_u32*);
typedef void(*GLBindVertexArrayProc)(te_u32);
typedef void(*GLDeleteVertexArraysProc)(te_u32, te_u32*);

/* GL_ARB_vertex_array_program */
typedef void(*GLVertexAttribPointerProc)(te_u32, te_i32, te_u32, te_i32, te_i32, const void*);
typedef void(*GLEnableVertexAttribArrayProc)(te_u32);
typedef void(*GLDisableVertexAttribArrayProc)(te_u32);

/* GL_EXT_framebuffer_object */
typedef te_bool(*GLIsRenderbufferProc)(te_u32);
typedef void(*GLBindRenderbufferProc)(te_u32, te_u32);
typedef void(*GLDeleteRenderbuffersProc)(te_u32, te_u32*);
typedef void(*GLGenRenderbuffersProc)(te_u32, te_u32*);
typedef void(*GLRenderbufferStorageProc)(te_u32, te_u32, te_u32, te_u32);
typedef void(*GLGetRenderbufferParameterivProc)(te_u32, te_u32, te_i32*);

typedef te_bool(*GLIsFramebufferProc)(te_u32);
typedef void(*GLBindFramebufferProc)(te_u32, te_u32);
typedef void(*GLDeleteFramebuffersProc)(te_u32, te_u32*);
typedef void(*GLGenFramebuffersProc)(te_u32, te_u32*);
typedef void(*GLFramebufferRenderbufferProc)(te_u32, te_u32, te_u32, te_u32);
typedef void(*GLFramebufferTexture1DProc)(te_u32, te_u32, te_u32, te_u32, te_i32);
typedef void(*GLFramebufferTexture2DProc)(te_u32, te_u32, te_u32, te_u32, te_i32);
typedef void(*GLFramebufferTexture3DProc)(te_u32, te_u32, te_u32, te_u32, te_i32, te_i32);
typedef void(*GLFramebufferTextureLayerProc)(te_u32, te_u32, te_u32, te_i32, te_i32);
typedef te_u32(*GLCheckFramebufferStatusProc)(te_u32);
typedef void(*GLGetFramebufferAttachmentParameterivProc)(te_u32, te_u32, te_u32, te_i32*);
typedef void(*GLBlitFramebufferProc)(te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_i32, te_u32);
typedef void(*GLGenerateMipmapProc)(te_u32);

/* GL_ARB_shader_objects */
typedef void(*GLDeleteShaderProc)(te_u32);
typedef te_u32(*GLCreateShaderProc)(te_u32);
typedef void(*GLShaderSourceProc)(te_u32, te_i32, const te_i8**, const te_i32*);
typedef void(*GLCompileShaderProc)(te_u32);
typedef te_u32(*GLGetShaderivProc)(te_u32, te_u32, te_i32*);
typedef te_u32(*GLGetShaderInfoLogProc)(te_u32, te_i32, te_i32*, te_i8*);

typedef te_u32(*GLCreateProgramProc)(void);
typedef void(*GLDeleteProgramProc)(te_u32);
typedef void(*GLAttachShaderProc)(te_u32, te_u32);
typedef void(*GLDetachShaderProc)(te_u32, te_u32);
typedef void(*GLLinkProgramProc)(te_u32);
typedef void(*GLUseProgramProc)(te_u32);

typedef void(*GLGetProgramivProc)(te_u32, te_u32, te_i32*);
typedef void(*GLGetProgramInfoLogProc)(te_u32, te_i32, te_i32*, te_i8*);
typedef void(*GLGetActiveUniformProc)(te_u32, te_u32, te_i32, te_i32*, te_i32*, te_i32*, te_i8*);
typedef te_i32(*GLGetUniformLocationProc)(te_u32, const te_i8*);

#define GL_UNIFORM_X(X, T)\
typedef void(*GLUniform1##X##Proc)(te_i32, T);\
typedef void(*GLUniform2##X##Proc)(te_i32, T, T);\
typedef void(*GLUniform3##X##Proc)(te_i32, T, T, T);\
typedef void(*GLUniform4##X##Proc)(te_i32, T, T, T, T);\
typedef void(*GLUniform1##X##vProc)(te_i32, te_i32 value, const T*);\
typedef void(*GLUniform2##X##vProc)(te_i32, te_i32 value, const T*);\
typedef void(*GLUniform3##X##vProc)(te_i32, te_i32 value, const T*);\
typedef void(*GLUniform4##X##vProc)(te_i32, te_i32 value, const T*)

GL_UNIFORM_X(f, te_f32);
GL_UNIFORM_X(i, te_i32);

typedef void(*GLUniformMatrix2fvProc)(te_i32, te_i32, te_bool, const te_f32*);
typedef void(*GLUniformMatrix3fvProc)(te_i32, te_i32, te_bool, const te_f32*);
typedef void(*GLUniformMatrix4fvProc)(te_i32, te_i32, te_bool, const te_f32*);

/* GL_ARB_vertex_shader */
typedef te_i32(*GLGetAttribLocationProc)(te_u32 prog, const te_i8* name);
typedef void(*GLGetActiveAttribProc)(te_u32 prog, te_u32 index, te_i32 bufSize, te_i32* length, te_i32* size, te_u32* type, te_i8* name);
typedef void(*GLBindAttribLocationProc)(te_u32 prog, te_u32 index, const te_i8* name);

struct vertex2D_s {
    te_vec3 pos;
    te_vec4 color;
    te_vec2 texcoord;
};

struct vertex3D_s {
    te_vec3 pos;
    te_vec4 color;
    te_vec2 texcoord;
    te_vec3 normal;
};

struct te_shader_s {
    te_u32 handle;
    te_i32 world_loc, modelview_loc;
    te_i32 texture_loc;
};

struct te_batch_s {
    te_u32 handle;
    te_u32 offset, size;
    void *data;
};

struct te_texture_s {
    te_u8 type;
    te_u32 handle;
    te_u32 fbo;
    te_i32 width, height;
};

typedef struct te_proc_s te_proc_t;
struct te_proc_s {
    te_u8 tag;
    const te_i8 *names[3];
};

static struct {
    te_i8 mag, min;
} s_gl_max_ver;

struct te_gl_s {
    struct {
        te_u8 major, minor;
        te_u16 glsl;
        te_bool es;
    } version;
    te_u32 extensions;
    void* procs[TEA_GL_PROC_COUNT];
};

typedef struct {
    te_u32 vbo, ibo;
    te_u32 index, size;
    void *data;
} te_blitbuffer_t;

typedef struct te_state_node_s te_state_node_t;
typedef struct te_list_node_s te_list_node_t;

struct te_state_node_s {
    te_shader_t *shader;
    te_texture_t *target;
    te_blitbuffer_t *buffer;
    te_bool clear_needed;
    te_color_t clear_color;
};

struct te_list_node_s {
    te_rect_t clip_rect;
    te_texture_t *texture;
    te_f32 *modelview;
    te_u8 draw_mode;

    te_u32 start, end;
};

typedef struct te_draw_command_s te_draw_command_t;
typedef struct te_draw_list_s te_draw_list_t;

struct te_draw_call_s {
    te_texture_t *texture;
    te_shader_t *shader;
    te_mat4 world, modelview;
    te_batch_t *batch;
};

struct te_draw_command_s {
    te_texture_t *target;
    te_shader_t *shader;
    te_batch_t *batch;
    te_bool clear_needed;
    te_color_t clear_color;
    te_u32 start, end;
};

struct te_draw_list_s {
    te_batch_t *batch;
    te_rect_t clip_rect;
    te_texture_t *texture;
    te_f32 *modelview;
    te_i32 draw_mode;
    
    te_u32 start, end;
};

#define MAX_MATRIX_STACK 32
struct matrix_stack_s {
    te_mat4 m[MAX_MATRIX_STACK];
    te_u8 top;
};

struct te_state_s {
    struct {
        struct matrix_stack_s stack[3];
        te_u8 mode;
        te_f32 *ptr;
    } matrix;

    struct {
        te_vec3 pos;
        te_vec4 color;
        te_vec2 texcoord;
    } vertex;

    te_u32 vao;
    te_shader_t *shader;
    te_texture_t *texture;
    te_batch_t *batch;
    te_texture_t *target;
    te_u8 draw_mode;
};

#define DEFAULT_COMMAND_SIZE 256
#define DEFAULT_LIST_SIZE 256

struct Tea {
    struct te_gl_s gl;
    struct te_state_s state;

    struct {
        te_draw_command_t *pool;
        te_u32 index;
        te_u32 count;
    } command;

    struct {
        te_draw_list_t *pool;
        te_u32 index;
        te_u32 count;
    } list;

    void(*next_command)(void);
    void(*next_list)(void);
};

struct Tea s_tea_ctx;
te_batch_t *default_batch;
te_texture_t *white_texture;
te_shader_t *default_shader;
te_u32 default_vao;

static void s_next_command(void);
static void s_next_list(void);

static const char *s_150_vert_header =
"#version 150\n"
"uniform mat4 u_World;\n"
"uniform mat4 u_ModelView;\n"
"in vec3 a_Position;\n"
"in vec4 a_Color;\n"
"in vec2 a_TexCoord;\n"
"out vec4 v_Color;\n"
"out vec2 v_TexCoord;\n";

static const char *s_150_frag_header =
"#version 150\n"
"in vec4 v_Color;\n"
"in vec2 v_TexCoord;\n"
"uniform sampler2D u_Texture;\n"
"out vec4 o_FragColor;\n";

static const char *s_120_vert_header =
"#version 120\n"
"uniform mat4 u_World;\n"
"uniform mat4 u_ModelView;\n"
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"attribute vec2 a_TexCoord;\n"
"varying vec4 v_Color;\n"
"varying vec2 v_TexCoord;\n";

static const char *s_120_frag_header =
"#version 120\n"
"varying vec4 v_Color;\n"
"varying vec2 v_TexCoord;\n"
"uniform sampler2D u_Texture;\n"
"#define o_FragColor gl_FragColor\n"
"#define texture texture2D\n";

static const char *s_default_vert_function =
"vec4 position(mat4 model_view, mat4 world, vec3 pos) {\n"
"  return model_view * world * vec4(pos, 1.0);\n"
"}\n";

static const char *s_default_frag_function =
"vec4 pixel(vec4 color, vec2 tex_coord, sampler2D tex) {\n"
"  return color * texture(tex, tex_coord);\n"
"}\n";
// "  return color;\n"

static const char *s_vert_main =
"void main() {\n"
"  v_Color = a_Color;\n"
"  v_TexCoord = a_TexCoord;\n"
"  gl_Position = position(u_ModelView, u_World, a_Position);\n"
"}\n";

static const char *s_frag_main =
"void main() {\n"
"  o_FragColor = pixel(v_Color, v_TexCoord, u_Texture);\n"
"}\n";

static void* s_get_proc(const te_i8 *name);
static te_bool s_load_gl(void);
static void s_setup_gl(void);
static void s_close_gl(void);

static te_blitbuffer_t* s_create_blitbuffer(void);
static void s_free_blitbuffer();
static void s_set_blitbuffer(te_blitbuffer_t *buffer);
static void s_send_vertex(te_vec2 pos, te_vec4 color, te_vec2 texcoord);
static void s_send_point(te_vec2 pos);
static void s_send_line(te_vec2 p0, te_vec2 p1);
static void s_send_fill_rectangle(te_vec2 pos, te_vec2 size);
static void s_send_draw_rectangle(te_vec2 pos, te_vec2 size);
static void s_send_fill_circle(te_vec2 pos, te_f32 radius);
static void s_send_draw_circle(te_vec2 pos, te_f32 radius);
static void s_send_fill_triangle(te_vec2 p0, te_vec2 p1, te_vec2 p3);
static void s_send_draw_triangle(te_vec2 p0, te_vec2 p1, te_vec2 p3);

/* data types */
#define TEA_GL_BYTE           0x1400
#define TEA_GL_UNSIGNED_BYTE  0x1401
#define TEA_GL_SHORT          0x1402
#define TEA_GL_UNSIGNED_SHORT 0x1403
#define TEA_GL_INT            0x1404
#define TEA_GL_UNSIGNED_INT   0x1405
#define TEA_GL_FLOAT          0x1406
#define TEA_GL_2_BYTES        0x1407
#define TEA_GL_3_BYTES        0x1408
#define TEA_GL_4_BYTES        0x1409
#define TEA_GL_DOUBLE         0x140A

/* Primitives */
#define TEA_GL_POINTS         0x0000
#define TEA_GL_LINES          0x0001
#define TEA_GL_LINE_LOOP      0x0002
#define TEA_GL_LINE_STRIP     0x0003
#define TEA_GL_TRIANGLES      0x0004
#define TEA_GL_TRIANGLE_STRIP 0x0005
#define TEA_GL_TRIANGLE_FAN   0x0006
#define TEA_GL_QUADS          0x0007
#define TEA_GL_QUAD_STRIP     0x0008
#define TEA_GL_POLYGON        0x0009

/* Clear buffer bits */
#define TEA_GL_DEPTH_BUFFER_BIT   0x00000100
#define TEA_GL_ACCUM_BUFFER_BIT   0x00000200
#define TEA_GL_STENCIL_BUFFER_BIT 0x00000400
#define TEA_GL_COLOR_BUFFER_BIT   0x00004000

#define TEA_GL_RGB         0x1907
#define TEA_GL_RGBA        0x1908

/* bgra */
#define TEA_GL_BGR  0x80E0
#define TEA_GL_BGRA 0x80E1

#define TEA_GL_ARRAY_BUFFER 0x8892
#define TEA_GL_ELEMENT_ARRAY_BUFFER 0x8893

#define TEA_GL_STREAM_DRAW  0x88E0
#define TEA_GL_STREAM_READ  0x88E1
#define TEA_GL_STREAM_COPY  0x88E2
#define TEA_GL_STATIC_DRAW  0x88E4
#define TEA_GL_STATIC_READ  0x88E5
#define TEA_GL_STATIC_COPY  0x88E6
#define TEA_GL_DYNAMIC_DRAW 0x88E8
#define TEA_GL_DYNAMIC_READ 0x88E9
#define TEA_GL_DYNAMIC_COPY 0x88EA

#define TEA_GL_TEXTURE_2D 0x0DE1
#define TEA_GL_TEXTURE_MIN_FILTER 0x2800
#define TEA_GL_TEXTURE_MAG_FILTER 0x2801
#define TEA_GL_TEXTURE_WRAP_S 0x2802
#define TEA_GL_TEXTURE_WRAP_T 0x2803

#define TEA_GL_NEAREST 0x2600
#define TEA_GL_REPEAT 0x2901
#define TEA_GL_CLAMP 0x2900

#define TEA_GL_CLAMP_TO_EDGE 0x812F /* 1.2 */
#define TEA_CLAMP_TO_BORDER  0x812D /* 1.3 */

#define BUFFER_SIZE 1000
te_config_t tea_config(void) {
    te_config_t conf = {0};
    memset(&conf, 0, sizeof(te_config_t));
    conf.vbo.size = 4 * BUFFER_SIZE;
    conf.vbo.usage = TEA_GL_DYNAMIC_DRAW;
    conf.ibo.size = 6 * BUFFER_SIZE;
    conf.ibo.usage = TEA_GL_DYNAMIC_DRAW;
    return conf;
}

te_bool tea_init(te_config_t *config) {
    TEA_ASSERT(config != NULL, "config is ́NULL");
    memset(tea(), 0, sizeof tea());
    gl()->version.major = config->gl.mag;
    gl()->version.minor = config->gl.min;
    gl()->version.es = config->gl.es;

    if (s_load_gl()) {
        s_setup_gl();
        s_close_gl();
    } else
        TEA_ASSERT(0, "failed to initialize OpenGL");

    tea()->command.count = DEFAULT_COMMAND_SIZE;
    tea()->command.index = 0;
    tea()->command.pool = TEA_MALLOC(DEFAULT_COMMAND_SIZE * sizeof(te_draw_command_t));

    tea()->list.count = DEFAULT_LIST_SIZE;
    tea()->list.index = 0;
    tea()->list.pool = TEA_MALLOC(DEFAULT_LIST_SIZE * sizeof(te_draw_list_t));

    tea()->next_command = s_next_command;
    tea()->next_list = s_next_list;

    default_batch = tea_batch(10000);
    default_shader = tea_shader(NULL, NULL);
    te_u8 pixels[4] = {255, 255, 255, 255};
    white_texture = tea_texture(TEA_RGBA, 1, 1, pixels, TEA_TEXTURE_STATIC);
    CALL_GL(GenVertexArrays)(1, &default_vao);

    state()->vertex.color[3] = 1.f;

    state()->batch = default_batch;
    state()->vao = default_vao;
    state()->shader = default_shader;
    state()->texture = white_texture;
#ifdef TEA_DEBUG
    TEA_LOG("Default shader: %p", default_shader);
    TEA_LOG("Default batch: %p", default_batch);
#endif

    CALL_GL(BindVertexArray)(default_vao);
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, default_batch->handle);
    CALL_GL(EnableVertexAttribArray)(0);
    CALL_GL(EnableVertexAttribArray)(1);
    CALL_GL(EnableVertexAttribArray)(2);
    CALL_GL(VertexAttribPointer)(0, 3, TEA_GL_FLOAT, 0, sizeof(te_f32)*9, (void*)0);
    CALL_GL(VertexAttribPointer)(1, 4, TEA_GL_FLOAT, 0, sizeof(te_f32)*9, (void*)(sizeof(te_f32)*3));
    CALL_GL(VertexAttribPointer)(2, 2, TEA_GL_FLOAT, 0, sizeof(te_f32)*9, (void*)(sizeof(te_f32)*7));
    CALL_GL(BindVertexArray)(0);
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, 0);
#ifdef TEA_DEBUG
    TEA_LOG("TEA INITIATED");
#endif
    return TEA_TRUE;
}

void tea_quit(void) {
    tea_shader_destroy(default_shader);
    CALL_GL(DeleteVertexArrays)(1, &default_vao);
    tea_batch_destroy(default_batch);
}

void tea_begin(void) {
    te_batch_t *batch = state()->batch;
    if (!batch)
        return;
    batch->offset = 0;
    CALL_GL(ClearColor)(0.3f, 0.4f, 0.4f, 1.f);
    CALL_GL(Clear)(TEA_GL_COLOR_BUFFER_BIT | TEA_GL_DEPTH_BUFFER_BIT);
    tea_matrix_mode(TEA_PERSPECTIVE);
    tea_load_identity();
    tea_ortho(0, 640, 480, 0, -1, 1);
    tea_matrix_mode(TEA_MODELVIEW);
    tea_load_identity();

    CALL_GL(Viewport)(0, 0, 640, 480);
}

void tea_end(void) {
    te_u32 vao = state()->vao;
    te_batch_t *batch = state()->batch;
    te_shader_t *shader = state()->shader;
    te_texture_t *tex = state()->texture;
    if (!batch)
        return;
    tea_batch_flush(batch);
    CALL_GL(BindTexture)(TEA_GL_TEXTURE_2D, tex->handle);
    CALL_GL(UseProgram)(shader->handle);
    tea_shader_set_uniform_matrix4fv(shader->world_loc, 1, TEA_FALSE, tea_get_matrix(TEA_PERSPECTIVE));
    tea_shader_set_uniform_matrix4fv(shader->modelview_loc, 1, TEA_FALSE, tea_get_matrix(TEA_MODELVIEW));
#ifdef TEA_DEBUG
    te_f32 *data = batch->data;
    TEA_LOG("batch vertex 1: (%f, %f, %f, %f, %f, %f, %f)", data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
    TEA_LOG("batch vertex 1: (%f, %f, %f, %f, %f, %f, %f)", data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    TEA_LOG("batch vertex 1: (%f, %f, %f, %f, %f, %f, %f)", data[18], data[19], data[20], data[21], data[22], data[23], data[24]);
#endif
    CALL_GL(BindVertexArray)(vao);
    CALL_GL(DrawArrays)(TEA_GL_TRIANGLES, 0, batch->offset / (9*sizeof(te_f32)));
    CALL_GL(BindTexture)(TEA_GL_TEXTURE_2D, 0);
    CALL_GL(BindVertexArray)(0);
}

void tea_set_texture(te_texture_t *tex) {}
void tea_set_target(te_texture_t *target) {}
void tea_set_shader(te_shader_t *shader) {}
void tea_set_mode(te_i32 mode) {}

/*=================================*
 *             Batch               *
 *=================================*/

te_batch_t *tea_batch(te_u32 size) {
    TEA_ASSERT(size > 0, "batch size must be greater than zero");
    te_batch_t *batch = NULL;
    batch = TEA_MALLOC(sizeof *batch);
    TEA_ASSERT(batch != NULL, "cannot alloc memory for batch");
    memset(batch, 0, sizeof *batch);
    CALL_GL(GenBuffers)(1, &batch->handle);
    batch->size = size;
    batch->data = TEA_MALLOC(size);
    TEA_ASSERT(batch->data != NULL, "cannot alloc memory for batch data");
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, batch->handle);
    CALL_GL(BufferData)(TEA_GL_ARRAY_BUFFER, size, NULL, TEA_GL_DYNAMIC_DRAW);
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, 0);
    return batch;
}

void tea_batch_destroy(te_batch_t *batch) {
    if (!batch) return;
    if (batch->handle)
        CALL_GL(DeleteBuffers)(1, &batch->handle);
    if (batch->data)
        TEA_FREE(batch->data);
    TEA_FREE(batch);
}

void tea_batch_set(te_batch_t *batch) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    state()->batch = batch;
}

void tea_batch_flush(te_batch_t *batch) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, batch->handle);
    CALL_GL(BufferSubData)(TEA_GL_ARRAY_BUFFER, 0, batch->offset, batch->data);
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, 0);
}

void tea_batch_resize(te_batch_t *batch, te_u32 size) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    if (batch->size >= size) return;
    batch->size = size;
    batch->data = TEA_REALLOC(batch->data, size);
}

void tea_batch_grow(te_batch_t *batch) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    batch->size *= 2;
    batch->data = TEA_REALLOC(batch->data, batch->size);
}

void tea_batch_offset(te_batch_t *batch, te_u32 offset) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    batch->offset = TEA_MIN(batch->size, offset);
}

te_u32 tea_batch_tell(te_batch_t *batch) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    return batch->offset;
}

void tea_batch_write(te_batch_t *batch, te_u32 size, const void *data) {
    TEA_ASSERT(batch != NULL, "batch cannot be NULL");
    size = TEA_MIN(size, batch->size - batch->offset);
    te_i8 *dt = data;
    te_i8 *bdt = batch->data;
    for (te_u32 i = 0; i < size; i++)
        bdt[i] = dt[i];
    batch->offset += size;
}

void tea_vertex2f(te_f32 x, te_f32 y) {
    te_batch_t *batch = state()->batch;
    te_f32 *data = GET_BATCH_DATA(batch, te_f32*);

    data[0] = x;
    data[1] = y;
    data[2] = 0.f;

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_vertex3f(te_f32 x, te_f32 y, te_f32 z) {
    te_batch_t *batch = state()->batch;
    te_f32 *data =  GET_BATCH_DATA(batch, te_f32*);

    data[0] = x;
    data[1] = y;
    data[2] = z;

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_vertex4f(te_f32 x, te_f32 y, te_f32 z, te_f32 w) {
    te_batch_t *batch = state()->batch;
    te_f32 *data = GET_BATCH_DATA(batch, te_f32*);

    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_vertex2fv(te_f32 *v) {
    if (!v) return;
    te_batch_t *batch = state()->batch;
    te_f32 *data = GET_BATCH_DATA(batch, te_f32*);

    for (te_i8 i = 0; i < 2; i++)
        data[i] = v[i];

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_vertex3fv(te_f32 *v) {
    if (!v) return;
    te_batch_t *batch = state()->batch;
    te_f32 *data =  GET_BATCH_DATA(batch, te_f32*);

    for (te_i8 i = 0; i < 3; i++)
        data[i] = v[i];

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_vertex4fv(te_f32 *v) {
    if (!v) return;
    te_batch_t *batch = state()->batch;
    te_f32 *data = GET_BATCH_DATA(batch, te_f32*);

    for (te_i8 i = 0; i < 4; i++)
        data[i] = v[i];

    data[3] = state()->vertex.color[0];
    data[4] = state()->vertex.color[1];
    data[5] = state()->vertex.color[2];
    data[6] = state()->vertex.color[3];

    data[7] = state()->vertex.texcoord[0];
    data[8] = state()->vertex.texcoord[1];

    batch->offset += 9*sizeof(te_f32);
}

void tea_color3f(te_f32 r, te_f32 g, te_f32 b) {
    te_f32 *color = state()->vertex.color;

    color[0] = r;
    color[1] = g;
    color[2] = b;
}
void tea_color4f(te_f32 r, te_f32 g, te_f32 b, te_f32 a) {
    te_f32 *color = state()->vertex.color;

    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

void tea_color3ub(te_u8 r, te_u8 g, te_u8 b) {
    te_f32 *color = state()->vertex.color;
    
    color[0] = r / 255.f;
    color[1] = g / 255.f;
    color[2] = b / 255.f;
}
void tea_color4ub(te_u8 r, te_u8 g, te_u8 b, te_u8 a) {
    te_f32 *color = state()->vertex.color;

    color[0] = r / 255.f;
    color[1] = g / 255.f;
    color[2] = b / 255.f;
    color[3] = a / 255.f;
}

void tea_color3fv(te_f32 *v) {
    te_f32 *color = state()->vertex.color;
    if (!v) return;

    for (te_i8 i = 0; i < 3; i++)
        color[i] = v[i];
}

void tea_color4fv(te_f32 *v) {
    te_f32 *color = state()->vertex.color;
    if (!v) return;

    for (te_i8 i = 0; i < 4; i++)
        color[i] = v[i];
}
void tea_color3ubv(te_u8 *v) {
    te_f32 *color = state()->vertex.color;
    if (!v) return;

    for (te_i8 i = 0; i < 3; i++)
        color[i] = v[i] / 255.f;
}
void tea_color4ubv(te_u8 *v) {
    te_f32 *color = state()->vertex.color;
    if (!v) return;

    for (te_i8 i = 0; i < 4; i++)
        color[i] = v[i] / 255.f;
}

void tea_texcoord2f(te_f32 u, te_f32 v) {
    te_f32 *tc = state()->vertex.texcoord;

    tc[0] = u;
    tc[1] = v;
}

void tea_texcoord2fv(te_f32 *v) {
    te_f32 *tc = state()->vertex.texcoord;
    if (!v) return;

    for (te_i8 i = 0; i < 2; i++)
        tc[i] = v[i];
}

void tea_normal3f(te_f32 x, te_f32 y, te_f32 z) {
    te_f32 *data = GET_BATCH_DATA(state()->batch, te_f32*);

    data[10] = x;
    data[11] = y;
    data[12] = z;
}

void tea_normal3fv(te_f32 *v) {
    te_f32 *data = GET_BATCH_DATA(state()->batch, te_f32*);
    if (!v) return;

    for (te_i8 i = 0; i < 3; i++)
        data[10+i] = v[i];
}

/*=================================*
 *             Matrix              *
 *=================================*/

#define CLONE_MATRIX(dest, src)\
    for (i = 0; i < 16; i++) dest[i] = src[i]

#define TRANSPOSE_MATRIX(dest, src)\
    for (i = 0; i < 4; i++)\
        for (j = 0; j < 4; j++)\
            dest[i * 4 + j] = src[j * 4 + i]

/* auxiliar */
static void s_clone_matrixf(te_f32 *dst, const te_f32 *src) {
    te_i32 i;
    CLONE_MATRIX(dst, src);
}
#if 0
static void s_transpose_matrixf(te_f32 *dst, const te_f32 *src) {
    te_i32 i, j;
    TRANSPOSE_MATRIX(dst, src);
}
#endif
static void s_mult_matrixf(te_f32 *dest, const te_f32 *a, const te_f32 *b) {
    te_i32 i, j, k;
    te_f32 tmp[16];
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

void tea_matrix_mode(te_u8 mode) {
    TEA_ASSERT(mode < 2, "Invalid matrix mode");
    state()->matrix.mode = mode;
    struct matrix_stack_s *stack = &state()->matrix.stack[mode];
    state()->matrix.ptr = stack->m[stack->top];
}

te_f32* tea_get_matrix(te_u8 mode) {
    TEA_ASSERT(mode < 2, "Invalid matrix mode");
    struct matrix_stack_s *stack = &state()->matrix.stack[mode];
    return (te_f32*)stack->m[stack->top];
}

void tea_load_identity(void) {
    for (te_i8 i = 0; i < 16; i++)
        state()->matrix.ptr[i] = 0.f;
    state()->matrix.ptr[0] = 1.f;
    state()->matrix.ptr[5] = 1.f;
    state()->matrix.ptr[10] = 1.f;
    state()->matrix.ptr[15] = 1.f;
}

void tea_push_matrix(void) {
    struct matrix_stack_s *stack = &state()->matrix.stack[state()->matrix.mode];
    TEA_ASSERT(stack->top < MAX_MATRIX_STACK - 1, "Matrix stack overflow");
    stack->top++;
    s_clone_matrixf(stack->m[stack->top], state()->matrix.ptr);
    state()->matrix.ptr = stack->m[stack->top];
}

void tea_pop_matrix(void) {
    struct matrix_stack_s *stack = &state()->matrix.stack[state()->matrix.mode];
    TEA_ASSERT(stack->top > 0, "Matrix stack underflow");
    stack->top--;
    state()->matrix.ptr = stack->m[stack->top];
}

void tea_load_matrix(const te_mat4 matrix) {
    TEA_ASSERT(matrix != NULL, "Invalid matrix");
    te_i32 i;
    CLONE_MATRIX(state()->matrix.ptr, matrix);
}

void tea_load_transpose_matrix(const te_mat4 matrix) {
    TEA_ASSERT(matrix != NULL, "Invalid matrix");
    te_i32 i, j;
    TRANSPOSE_MATRIX(state()->matrix.ptr, matrix);
}

void tea_mult_matrix(const te_mat4 matrix) {
    TEA_ASSERT(matrix != NULL, "Invalid matrix");
    s_mult_matrixf(state()->matrix.ptr, matrix, state()->matrix.ptr);
}

void tea_mult_transpose_matrix(const te_mat4 matrix) {
    TEA_ASSERT(matrix != NULL, "Invalid matrix");
    te_mat4 tmatrix;
    te_i8 i, j;
    TRANSPOSE_MATRIX(tmatrix, matrix);
    s_mult_matrixf(state()->matrix.ptr, tmatrix, state()->matrix.ptr);
}

void tea_ortho(te_f32 left, te_f32 right, te_f32 bottom, te_f32 top, te_f32 near, te_f32 far) {
    te_f32 *ptr = state()->matrix.ptr;
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

void tea_frustum(te_f32 left, te_f32 right, te_f32 bottom, te_f32 top, te_f32 near, te_f32 far) {
    te_f32 *ptr = state()->matrix.ptr;
    ptr[0] = 2.f * near / (right - left);
    ptr[1] = 0.f;
    ptr[2] = 0.f;
    ptr[3] = 0.f;

    ptr[4] = 0.f;
    ptr[5] = 2.f * near / (top - bottom);
    ptr[6] = 0.f;
    ptr[7] = 0.f;
    
    ptr[8] = (right + left) / (right - left);
    ptr[9] = (top + bottom) / (top - bottom);
    ptr[10] = -(far + near) / (far - near);
    ptr[11] = -1.f;

    ptr[12] = 0.f;
    ptr[13] = 0.f;
    ptr[14] = -(2.f * far * near) / (far - near);
    ptr[15] = 0.f;
}

void tea_perspective(te_f32 fovy, te_f32 aspect, te_f32 near, te_f32 far) {
   te_f32 ymax = near * tan(fovy * TEA_PI / 360.f);
   te_f32 ymin = -ymax;
   te_f32 xmin = ymin * aspect;
   te_f32 xmax = ymax * aspect;
   tea_frustum(xmin, xmax, ymin, ymax, near, far);
}

void tea_translate(te_f32 x, te_f32 y, te_f32 z) {
    te_mat4 m = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        x, y, z, 1.f
    };
    tea_mult_matrix(m);
}

void tea_scale(te_f32 x, te_f32 y, te_f32 z) {
    te_mat4 m = {
        x, 0.f, 0.f, 0.f,
        0.f, y, 0.f, 0.f,
        0.f, 0.f, z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    tea_mult_matrix(m);
}

void tea_rotate(te_f32 angle, te_f32 x, te_f32 y, te_f32 z) {
    te_f32 rad = TEA_DEG2RAD(angle);
    te_f32 c = cosf(rad);
    te_f32 s = sinf(rad);
    te_f32 nc = 1.f - c;
    te_f32 len = x*x + y*y + z*z;
    if (len > 0.f) {
        te_f32 rlen = 1.f / sqrtf(len);
        x *= rlen;
        y *= rlen;
        z *= rlen;
    }

    te_mat4 m = {
        x*x*nc + c,   y*x*nc + z*s, z*x*nc - y*s, 0.f,
        x*y*nc - z*s, y*y*nc + c,   z*y*nc + x*s, 0.f,
        x*z*nc + y*s, y*z*nc - x*s, z*z*nc + c,   0.f,
        0.f,         0.f,         0.f,         1.f
    };
    tea_mult_matrix(m);
}

/*=================================*
 *             Texture             *
 *=================================*/

te_texture_t *tea_texture(te_u8 format, te_i32 width, te_i32 height, const void *data, te_u8 type) {
    TEA_ASSERT(width > 0 && height > 0, "Invalid texture size");
    te_texture_t *tex = TEA_MALLOC(sizeof *tex);
    tex->width = width;
    tex->height = height;
    tex->fbo = 0;
    tex->type = type;
    te_i32 gl_format = format == TEA_RGBA ? TEA_GL_RGBA : TEA_GL_RGB;
    CALL_GL(GenTextures)(1, &tex->handle);
    CALL_GL(BindTexture)(TEA_GL_TEXTURE_2D, tex->handle);
    CALL_GL(TexParameteri)(TEA_GL_TEXTURE_2D, TEA_GL_TEXTURE_WRAP_S, TEA_GL_CLAMP_TO_EDGE);
    CALL_GL(TexParameteri)(TEA_GL_TEXTURE_2D, TEA_GL_TEXTURE_WRAP_T, TEA_GL_CLAMP_TO_EDGE);
    CALL_GL(TexParameteri)(TEA_GL_TEXTURE_2D, TEA_GL_TEXTURE_MIN_FILTER, TEA_GL_NEAREST);
    CALL_GL(TexParameteri)(TEA_GL_TEXTURE_2D, TEA_GL_TEXTURE_MAG_FILTER, TEA_GL_NEAREST);
    CALL_GL(TexImage2D)(TEA_GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, TEA_GL_UNSIGNED_BYTE, data);
    CALL_GL(BindTexture)(TEA_GL_TEXTURE_2D, 0);

    return tex;
}

void tea_texture_destroy(te_texture_t *tex) {
    if (!tex) return;
    if (tex->handle)
        CALL_GL(DeleteTextures)(1, &tex->handle);
    TEA_FREE(tex);
}


/*=================================*
 *             Shader              *
 *=================================*/

#define TEA_GL_FRAGMENT_SHADER 0x8B30
#define TEA_GL_VERTEX_SHADER 0x8B31

#define TEA_GL_COMPILE_STATUS 0x8B81
#define TEA_GL_LINK_STATUS 0x8B82

const te_i8 *shader_type_names[] = {
    "Fragment",
    "Vertex"
};

static te_u32 s_compile_shader(te_u32 type, const te_i8 *src) {
    te_u32 shader = CALL_GL(CreateShader)(type);
    CALL_GL(ShaderSource)(shader, 1, &src, NULL);
    CALL_GL(CompileShader)(shader);
    te_i32 status;
    CALL_GL(GetShaderiv)(shader, TEA_GL_COMPILE_STATUS, &status);
    if (status == 0) {
        te_i8 log[1024];
        CALL_GL(GetShaderInfoLog)(shader, 1024, NULL, log);
        TEA_ASSERT(0, "%s shader compile error: %s", shader_type_names[type - TEA_GL_FRAGMENT_SHADER], log);
    }
    return shader;
}

te_shader_t *tea_shader(const te_i8 *vert, const te_i8 *frag) {
    vert = vert ? vert : s_default_vert_function;
    frag = frag ? frag : s_default_frag_function;

    te_shader_t *shader = TEA_MALLOC(sizeof *shader);
    memset(shader, 0, sizeof *shader); 

    const te_i8 *vert_shader_strs[] = { s_120_vert_header, vert, s_vert_main };
    const te_i8 *frag_shader_strs[] = { s_120_frag_header, frag, s_frag_main };
    
    te_i32 vert_len, frag_len;
    vert_len = frag_len = 0;
    for (te_i8 i = 0; i < 3; i++) {
        vert_len += strlen(vert_shader_strs[i]);
        frag_len += strlen(frag_shader_strs[i]);
    }

    te_i8 vert_source[vert_len + 1];
    te_i8 frag_source[frag_len + 1];
    vert_source[0] = frag_source[0] = '\0';
    for (te_i8 i = 0; i < 3; i++) {
        strcat(vert_source, (const te_i8*)vert_shader_strs[i]);
        strcat(frag_source, (const te_i8*)frag_shader_strs[i]);
    }
    vert_source[vert_len] = frag_source[frag_len] = '\0';

    te_u32 shaders[2];
    shaders[0] = s_compile_shader(TEA_GL_VERTEX_SHADER, vert_source);
    shaders[1] = s_compile_shader(TEA_GL_FRAGMENT_SHADER, frag_source);

    shader = tea_shader_from_gl(2, shaders);
    CALL_GL(DeleteShader)(shaders[0]);
    CALL_GL(DeleteShader)(shaders[1]);
    return shader;
}

te_shader_t *tea_shader_from_gl(te_i32 count, te_u32 *shaders) {
    TEA_ASSERT(count > 0, "Invalid shader count");
    TEA_ASSERT(shaders != NULL, "shaders cannot be NULL");
    te_shader_t *shader = TEA_MALLOC(sizeof *shader);
    TEA_ASSERT(shader != NULL, "cannot alloc memory for shader");
    shader->handle = CALL_GL(CreateProgram)();
    for (te_i32 i = 0; i < count; i++)
        CALL_GL(AttachShader)(shader->handle, shaders[i]);
    
    CALL_GL(LinkProgram)(shader->handle);
    te_i32 status;
    CALL_GL(GetProgramiv)(shader->handle, TEA_GL_LINK_STATUS, &status);
    if (status == 0) {
        te_i8 log[1024];
        CALL_GL(GetProgramInfoLog)(shader->handle, 1024, NULL, log);
        TEA_ASSERT(0, "program link error: %s", log);
    }
    shader->world_loc = CALL_GL(GetUniformLocation)(shader->handle, "u_World");
    shader->modelview_loc = CALL_GL(GetUniformLocation)(shader->handle, "u_ModelView");
    shader->texture_loc = CALL_GL(GetUniformLocation)(shader->handle, "u_Texture");
    return shader;
}

void tea_shader_destroy(te_shader_t *shader) {
    if (!shader) return;
    if (shader->handle)
        CALL_GL(DeleteProgram)(shader->handle);
    TEA_FREE(shader);
}

void tea_shader_set(te_shader_t *shader) {
    TEA_ASSERT(shader != NULL, "shader cannot be NULL");
    state()->shader = shader;
}

te_i32 tea_shader_uniform_location(te_shader_t *shader, const te_i8 *name) {
    return CALL_GL(GetUniformLocation)(shader->handle, name);
}

#define TEA_UNIFORM_X_IMPL(X, T)\
void tea_shader_set_uniform1##X(te_i32 l, T v) { CALL_GL(Uniform1##X)(l, v); }\
void tea_shader_set_uniform2##X(te_i32 l, T v0, T v1) { CALL_GL(Uniform2##X)(l, v0, v1); }\
void tea_shader_set_uniform3##X(te_i32 l, T v0, T v1, T v2) { CALL_GL(Uniform3##X)(l, v0, v1, v2); }\
void tea_shader_set_uniform4##X(te_i32 l, T v0, T v1, T v2, T v3) { CALL_GL(Uniform4##X)(l, v0, v1, v2, v3); }\
void tea_shader_set_uniform1##X##v(te_i32 l, te_i32 val, const T *v) { CALL_GL(Uniform1##X##v)(l, val, v); }\
void tea_shader_set_uniform2##X##v(te_i32 l, te_i32 val, const T *v) { CALL_GL(Uniform2##X##v)(l, val, v); }\
void tea_shader_set_uniform3##X##v(te_i32 l, te_i32 val, const T *v) { CALL_GL(Uniform3##X##v)(l, val, v); }\
void tea_shader_set_uniform4##X##v(te_i32 l, te_i32 val, const T *v) { CALL_GL(Uniform4##X##v)(l, val, v); }

TEA_UNIFORM_X_IMPL(f, te_f32)
TEA_UNIFORM_X_IMPL(i, te_i32)

void tea_shader_set_uniform_matrix2fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m) {
    CALL_GL(UniformMatrix2fv)(location, count, transpose, m);
}

void tea_shader_set_uniform_matrix3fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m) {
    CALL_GL(UniformMatrix3fv)(location, count, transpose, m);
}

void tea_shader_set_uniform_matrix4fv(te_i32 location, te_i32 count, te_bool transpose, const te_mat4 m) {
    CALL_GL(UniformMatrix4fv)(location, count, transpose, m);
}

/*=================================*
 *           Blitbuffer            *
 *=================================*/

te_blitbuffer_t *s_create_blitbuffer(void) {
    te_blitbuffer_t *buffer = TEA_MALLOC(sizeof(*buffer));
    TEA_ASSERT(buffer != NULL, "cannot alloc memory for blit buffer");
    memset(buffer, 0, sizeof *buffer);

    CALL_GL(GenBuffers)(1, &buffer->vbo);
    CALL_GL(GenBuffers)(1, &buffer->ibo);
    int size = sizeof(float) * 9 * BUFFER_SIZE;

    buffer->size = BUFFER_SIZE;
    buffer->data = TEA_MALLOC(BUFFER_SIZE * 9 * sizeof(float));
    TEA_ASSERT(buffer->data != NULL, "cannot alloc memory for buffer data");
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, buffer->vbo);
    CALL_GL(BufferData)(TEA_GL_ARRAY_BUFFER, size, NULL, TEA_GL_DYNAMIC_DRAW);
    CALL_GL(BindBuffer)(TEA_GL_ARRAY_BUFFER, 0);

    return buffer;
}

void s_free_blitbuffer(te_blitbuffer_t *buffer) {
    if (!buffer) return;
    if (buffer->vbo)
        CALL_GL(DeleteBuffers)(1, &buffer->vbo);
    if (buffer->ibo)
        CALL_GL(DeleteBuffers)(1, &buffer->ibo);
    if (buffer->data)
        TEA_FREE(buffer->data);
    TEA_FREE(buffer);
}

void s_send_vertex(te_vec2 pos, te_vec4 color, te_vec2 uv) {
    float vertices[9];
    memcpy(&vertices, pos, sizeof(te_vec2));
    vertices[2] = 0.f;
    memcpy(&vertices[4], color, sizeof(te_vec4));
    memcpy(&vertices[8], uv, sizeof(te_vec2));
}

void s_send_point(te_vec2 pos) {
    s_send_vertex(pos, (te_vec4){1.f, 1.f, 1.f, 1.f}, (te_vec2){0.f, 0.f});
}

void s_send_line(te_vec2 p0, te_vec2 p1) {
    s_send_vertex(p0, (te_vec4){1.f, 1.f, 1.f, 1.f}, (te_vec2){0.f, 0.f});
    s_send_vertex(p1, (te_vec4){1.f, 1.f, 1.f, 1.f}, (te_vec2){0.f, 0.f});
}

void s_send_draw_rectangle(te_vec2 p0, te_vec2 size) {
    te_vec2 p1, p2, p3;
    p1[0] = p0[0] + size[0];
    p1[1] = p0[1];

    p2[0] = p0[0] + size[0];
    p2[1] = p0[1] + size[1];

    p3[0] = p0[0];
    p3[1] = p0[1] + size[1];

    s_send_line(p0, p1);
    s_send_line(p1, p2);
    s_send_line(p2, p3);
    s_send_line(p3, p0);
}

void s_send_draw_circle(te_vec2 pos, te_f32 radius) {
    int sides = 32;
    double pi2 = 2.0 * TEA_PI;
    int i;
    for (i = 0; i < sides; i++) {
        float tetha = (i * pi2) / sides;
        te_vec2 p;
        p[0] = pos[0] + (cosf(tetha) * radius);
        p[1] = pos[1] + (sinf(tetha) * radius);
        s_send_vertex(p, (te_vec4){1.f, 1.f, 1.f, 1.f}, (te_vec2){0.f, 0.f});
        tetha = ((i+1) * pi2) / sides;
        p[0] = pos[0] + (cosf(tetha) * radius);
        p[1] = pos[1] + (sinf(tetha) * radius);
        s_send_vertex(p, (te_vec4){1.f, 1.f, 1.f, 1.f}, (te_vec2){0.f, 0.f});
    }
}

void s_send_fill_circle(te_vec2 p0, te_f32 radius) {
    int sides = 32;
    double pi2 = 2.0 * TEA_PI;
    int i;
    for (i = 0; i < sides; i++) {
        float tetha = (i * pi2) / sides;
        float tetha2 = ((i+1) * pi2) / sides;
        te_vec2 p1, p2;
        p1[0] = p0[0] + (cosf(tetha) * radius);
        p1[1] = p0[1] + (sinf(tetha) * radius);

        p2[0] = p0[0] + (cosf(tetha2) * radius);
        p2[1] = p0[1] + (sinf(tetha2) * radius);

        s_send_fill_triangle(p0, p1, p2);
    }
}

void s_send_draw_triangle(te_vec2 p0, te_vec2 p1, te_vec2 p2) {
    s_send_line(p0, p1);
    s_send_line(p1, p2);
    s_send_line(p2, p0);
}

void s_send_fill_triangle(te_vec2 p0, te_vec2 p1, te_vec2 p2) {
    s_send_point(p0);
    s_send_point(p1);
    s_send_point(p2);
}

/*=================================*
 *              Debug              *
 *=================================*/
#include <stdarg.h>

void tea_log(te_i32 line, const te_i8* func, const te_i8* file, const te_i8 *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s:%d - %s(...): ", file, line, func);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void tea_abort(const te_i8 *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

/*=================================*
 *             Loader              *
 *=================================*/

void s_next_command(void) {
    if (tea()->command.index + 1 > tea()->command.count) {
        tea()->command.count *= 2;
        tea()->command.pool = TEA_REALLOC(tea()->command.pool, tea()->command.count * sizeof(te_draw_command_t));
    }
    te_draw_command_t *old = tea()->command.pool + tea()->command.index;
    tea()->command.index++;
    te_draw_command_t *current = tea()->command.pool + tea()->command.index;
    memcpy(current, old, sizeof *current);
    current->start = old->end;
    current->end = old->end;
    current->clear_needed = TEA_FALSE;
}

void s_next_list(void) {
    te_draw_command_t *command = tea()->command.pool + tea()->command.index;
    te_u32 index = command->end;
    if (index + 1 > tea()->command.count) {
        tea()->command.count *= 2;
        tea()->command.pool = TEA_REALLOC(tea()->command.pool, tea()->command.count * sizeof(te_draw_command_t));
    }
    te_draw_list_t *old = tea()->list.pool + index;
    command->end++;
    te_draw_list_t *current = tea()->list.pool + command->end;
    memcpy(current, old, sizeof *current);
}

static te_bool s_tea_load_procs(te_proc_t *procs, te_u32 flags);

#if !defined(__APPLE__) && !defined(__HAIKU__)
void* (*s_proc_address)(const te_i8*);
#endif

te_bool s_load_gl(void) {
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
    te_u32 index;
    for (index = 0; names[index] != NULL; ++index) {
        s_glsym = dlopen(names[index], RTLD_LAZY | RTLD_GLOBAL);
        if (s_glsym != NULL) {
#if defined(__APPLE__) || defined(__HAIKU__)
            return TEA_TRUE;
#else
            s_proc_address = (void*(*)(const te_i8*))dlsym(s_glsym, "glXGetProcAddress");
            return s_proc_address != NULL;
#endif
            break;
        }
    }
#endif
    return TEA_FALSE;
}

void s_close_gl(void) {
    if (s_glsym != NULL) {
#if defined(_WIN32)
        FreeLibrary(s_glsym);
#else
        dlclose(s_glsym);
#endif
        s_glsym = NULL;
    }
}

void s_setup_gl(void) {
    GET_GL(GetString) = s_get_proc("glGetString");
    GET_GL(GetStringi) = s_get_proc("glGetStringi");

    const te_i8 *version = (const te_i8*)CALL_GL(GetString)(TEA_GL_VERSION);
    const te_i8 *glsl = (const te_i8*)CALL_GL(GetString)(TEA_GL_SHADING_LANGUAGE_VERSION);
    TEA_ASSERT(version != NULL, "Failed to get OpenGL version");
    const te_i8 *prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL,
    };

    te_i8 *ver = (te_i8*)version;
    for (te_u32 i = 0; prefixes[i] != NULL; i++) {
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
        { TEA_GL_Gette_f32v, { "glGette_f32v", NULL }},
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
#if 0
    if (gl()->extensions & TEA_HAS_VAO) {
        s_tea_bind_vao = s_tea_bind_vao3;
    } else if (gl()->extensions & TEA_HAS_VBO) {
        s_tea_bind_vao = s_tea_bind_vao2;
    } else {
        s_tea_bind_vao = s_tea_bind_vao1;
    }
#endif

    if (gl()->extensions & TEA_HAS_SHADER) {
        fprintf(stderr, "Using shader\n");
#if 0
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
#endif
    }
}

void* s_get_proc(const te_i8 *name) {
    void *sym = NULL;
    if (s_glsym == NULL) return sym;
#if !defined(__APPLE__) && !defined(__HAIKU__)
    if (s_proc_address != NULL) {
        sym = s_proc_address(name);
    }
#endif
    if (sym == NULL) {
#if defined(_WIN32) || defined(__CYGWIN__)
        sym = (void*)GetProcAddress(s_glsym, name);
#else
        sym = (void*)dlsym(s_glsym, name);
#endif
    }
    return sym;
}

te_bool s_tea_load_procs(te_proc_t *procs, te_u32 flags) {
    te_proc_t *proc = procs;
    while (proc->names[0]) {
        if (!gl()->procs[proc->tag] || (flags & TEA_PROC_OVERRIDE)) {
            te_u32 i = 0;
            te_i8 **names = (te_i8**)proc->names;
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
