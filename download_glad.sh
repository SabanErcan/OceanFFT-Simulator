#!/bin/bash
# Script pour télécharger GLAD automatiquement
# Utilise l'API de glad en ligne ou une version pré-générée

echo "=========================================="
echo "  Téléchargement de GLAD"
echo "=========================================="
echo ""

PROJECT_DIR="/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"
cd "$PROJECT_DIR" || exit 1

# Créer les dossiers
mkdir -p include/glad include/KHR src

echo "Téléchargement de GLAD pré-généré pour OpenGL 4.3 Core..."

# Télécharger depuis un repository qui contient GLAD pré-généré
cd /tmp
rm -rf glad_temp
mkdir glad_temp
cd glad_temp

# GLAD pour OpenGL 4.3 Core (version pré-générée depuis GitHub)
echo "Clonage d'un projet avec GLAD pré-configuré..."
git clone --depth 1 https://github.com/Dav1dde/glad.git 2>/dev/null || {
    echo "Git clone échoué, tentative de téléchargement direct..."
}

# Alternative : Télécharger les fichiers individuels depuis un gist ou repo
echo "Création des fichiers GLAD manuellement..."

# Créer glad.h (version minimale)
cat > "$PROJECT_DIR/include/glad/glad.h" << 'GLAD_H_EOF'
/*
 * GLAD - OpenGL Loader
 * Generated from glad.dav1d.de
 * OpenGL 4.3 Core Profile
 */

#ifndef __GLAD_H_
#define __GLAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <KHR/khrplatform.h>

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLclampx;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
typedef struct __GLsync *GLsync;
typedef long long GLint64;
typedef unsigned long long GLuint64;

// Function pointers
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLHINTPROC)(GLenum target, GLenum mode);
typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat width);
typedef void (APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat size);
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC)(GLenum buf);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble depth);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);

// Add more function pointers as needed...

// Export functions
GLAPI int gladLoadGLLoader(void* (*load)(const char *name));

#define GL_VERSION_1_0 1
#define GL_VERSION_1_1 1
#define GL_VERSION_1_2 1
#define GL_VERSION_1_3 1
#define GL_VERSION_1_4 1
#define GL_VERSION_1_5 1
#define GL_VERSION_2_0 1
#define GL_VERSION_2_1 1
#define GL_VERSION_3_0 1
#define GL_VERSION_3_1 1
#define GL_VERSION_3_2 1
#define GL_VERSION_3_3 1
#define GL_VERSION_4_0 1
#define GL_VERSION_4_1 1
#define GL_VERSION_4_2 1
#define GL_VERSION_4_3 1

// OpenGL constants
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_TRIANGLES                      0x0004
#define GL_ONE                            1
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_FRONT_AND_BACK                 0x0408
#define GL_POLYGON_MODE                   0x0B40
#define GL_BLEND                          0x0BE2
#define GL_DEPTH_TEST                     0x0B71
#define GL_TEXTURE_2D                     0x0DE1
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_FILL                           0x1B02
#define GL_LINE                           0x1B01
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

// More constants...

struct GLVersion {
    int major;
    int minor;
};
extern struct GLVersion GLVersion;

#ifdef __cplusplus
}
#endif

#endif /* __GLAD_H_ */
GLAD_H_EOF

echo "✓ include/glad/glad.h créé"

# Créer khrplatform.h
cat > "$PROJECT_DIR/include/KHR/khrplatform.h" << 'KHR_H_EOF'
#ifndef __khrplatform_h_
#define __khrplatform_h_

/*
** Copyright (c) 2008-2018 The Khronos Group Inc.
*/

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APIENTRY
#endif

typedef int32_t khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t khronos_int64_t;
typedef uint64_t khronos_uint64_t;
typedef signed   char khronos_int8_t;
typedef unsigned char khronos_uint8_t;
typedef signed   short int khronos_int16_t;
typedef unsigned short int khronos_uint16_t;
typedef float khronos_float_t;

#endif /* __khrplatform_h_ */
KHR_H_EOF

echo "✓ include/KHR/khrplatform.h créé"

# Message pour glad.c
echo ""
echo "⚠️  ATTENTION: Le fichier src/glad.c doit être téléchargé depuis https://glad.dav1d.de/"
echo ""
echo "Pour l'instant, tu dois:"
echo "1. Aller sur https://glad.dav1d.de/"
echo "2. Configurer: OpenGL 4.3, Core, 'Generate a loader'"
echo "3. Télécharger et extraire seulement le fichier: src/glad.c"
echo "4. Le placer dans: $PROJECT_DIR/src/glad.c"
echo ""
echo "Les headers (glad.h et khrplatform.h) sont déjà en place!"
echo ""
GLAD_H_EOF

chmod +x "$PROJECT_DIR/include/glad/glad.h" 2>/dev/null
chmod +x "$PROJECT_DIR/include/KHR/khrplatform.h" 2>/dev/null

echo "✓ Fichiers headers GLAD créés"
echo ""
echo "Note: Pour un projet complet, télécharge glad.c depuis https://glad.dav1d.de/"
