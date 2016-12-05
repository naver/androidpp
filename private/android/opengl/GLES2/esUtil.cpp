//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 2.0 Programming Guide.
//

///
//  Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "esUtil.h"
#include "esUtil_win.h"

#include <assert>



///
// CreateEGLContext()
//
//    Creates an EGL rendering context and all associated elements
//
EGLBoolean ESUTIL_API CreateEGLContext ( EGLNativeWindowType hWnd, EGLDisplay* eglDisplay,
                              EGLContext* eglContext, EGLConfig* eglConfig,
                              EGLint attribList[])
{
   EGLint numConfigs;
   EGLint majorVersion;
   EGLint minorVersion;
   EGLDisplay display;
   EGLContext context;
   EGLConfig config;
   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, 
       EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_EXT, EGL_LOSE_CONTEXT_ON_RESET_EXT,
       EGL_NONE };

   // Get Display
   display = eglGetDisplay(GetDC(hWnd));
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }

   // Initialize EGL
   static bool isEGLInitialized = false;
   //if ( !isEGLInitialized ) // FIXME: Is it safe to call eglInitialize() multiple times?
   {
       if ( !eglInitialize(display, &majorVersion, &minorVersion) )
       {
          return EGL_FALSE;
       }
       isEGLInitialized = true;
   }

   // Get configs
   if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Choose config
   if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Create a GL context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return EGL_FALSE;
   }   
   
   *eglDisplay = display;
   *eglContext = context;
   *eglConfig = config;
   return EGL_TRUE;
} 

EGLBoolean ESUTIL_API CreateEGLSurface(EGLNativeWindowType hWnd, EGLDisplay eglDisplay,
    EGLContext eglContext, EGLConfig eglConfig, EGLSurface* eglSurface)
{
    assert(eglSurface);

    EGLDisplay display = eglDisplay;
    EGLContext context = eglContext;
    EGLSurface surface;
    EGLConfig config = eglConfig;

    if (*eglSurface)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display, *eglSurface);
    }

    // Create a surface
    surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, NULL);
    if (surface == EGL_NO_SURFACE)
    {
        return EGL_FALSE;
    }

    // Make the context current
    if (!eglMakeCurrent(display, surface, surface, context))
    {
        return EGL_FALSE;
    }

    *eglSurface = surface;
    return EGL_TRUE;
}

EGLBoolean ESUTIL_API CreateSharedEGLContext ( EGLContext* eglContext, EGLDisplay display, 
                              EGLContext share_context, EGLSurface surface, EGLConfig* eglConfig,
                              EGLint attribList[], void* hWnd)
{
   EGLint numConfigs;
   EGLContext context;
   EGLConfig config;
   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

   // Get configs
   if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Choose config
   if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Create a surface
   if ( surface == EGL_NO_SURFACE )
   {
       EGLint surface_attribs[] =
       {
           EGL_WIDTH,          1,
           EGL_HEIGHT,         1,
           EGL_NONE
       };

       surface = eglCreatePbufferSurface(display, config, surface_attribs);
       if ( surface == EGL_NO_SURFACE )
       {
          return EGL_FALSE;
       }
   }

   // Create a GL context
   context = eglCreateContext(display, config, share_context, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return EGL_FALSE;
   }   
   
   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      return EGL_FALSE;
   }
   
   *eglContext = context;
   *eglConfig = config;
   return EGL_TRUE;
} 

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esInitContext()
//
//      Initialize ES utility context.  This must be called before calling any other
//      functions.
//
void ESUTIL_API esInitContext ( ESContext *esContext )
{
   if ( esContext != NULL )
   {
      memset( esContext, 0, sizeof( ESContext) );
   }
}

///
//  esCreateContext()
//
//    Creates a rendering context and all associated elements
//
GLboolean ESUTIL_API esCreateContext ( ESContext *esContext, GLint width, GLint height,
                                       GLint bitsRed, GLint bitsGreen, GLint bitsBlue, bool alphaChannel, bool depthBuffer, 
                                       bool stencilBuffer, bool multiSample )
{
   EGLint attribList[] =
   {
       EGL_RED_SIZE,       bitsRed,
       EGL_GREEN_SIZE,     bitsGreen,
       EGL_BLUE_SIZE,      bitsBlue,
       EGL_ALPHA_SIZE,     (alphaChannel) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     (depthBuffer) ? 8 : EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   (stencilBuffer) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (multiSample) ? 1 : 0,
       EGL_NONE
   };
   
   if ( esContext == NULL )
   {
      return GL_FALSE;
   }

   esContext->width = width;
   esContext->height = height;

   if ( !CreateEGLContext ( esContext->hWnd,
                            &esContext->eglDisplay,
                            &esContext->eglContext,
                            &esContext->eglConfig,
                            attribList) )
   {
      return GL_FALSE;
   }
   

   return GL_TRUE;
}

GLboolean ESUTIL_API esCreateSurface(ESContext *esContext, GLint width, GLint height)
{
    if (esContext == NULL)
    {
        return GL_FALSE;
    }

    esContext->width = width;
    esContext->height = height;

    if (!CreateEGLSurface(esContext->hWnd,
        esContext->eglDisplay,
        esContext->eglContext,
        esContext->eglConfig,
        &esContext->eglSurface))
    {
        return GL_FALSE;
    }


    return GL_TRUE;
}

GLboolean ESUTIL_API esCreateSharedContext ( ESContext *esContext, const ESContext *esShareContext,
                                       GLint bitsRed, GLint bitsGreen, GLint bitsBlue, bool alphaChannel, bool depthBuffer, 
                                       bool stencilBuffer, bool multiSample )
{
   EGLint attribList[] =
   {
       EGL_RED_SIZE,       bitsRed,
       EGL_GREEN_SIZE,     bitsGreen,
       EGL_BLUE_SIZE,      bitsBlue,
       EGL_ALPHA_SIZE,     (alphaChannel) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     (depthBuffer) ? 8 : EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   (stencilBuffer) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (multiSample) ? 1 : 0,
       EGL_NONE
   };
   
   if ( esContext == NULL || esShareContext == NULL )
   {
      return GL_FALSE;
   }

   esContext->hWnd          = esShareContext->hWnd;
   esContext->width         = esShareContext->width;
   esContext->height        = esShareContext->height;
   esContext->eglDisplay    = esShareContext->eglDisplay;
   esContext->eglSurface    = esShareContext->eglSurface;
   esContext->eglConfig     = esShareContext->eglConfig;

   if ( !CreateSharedEGLContext ( &esContext->eglContext,
                                  esShareContext->eglDisplay,
                                  esShareContext->eglContext,
                                  esShareContext->eglSurface,
                                  &esContext->eglConfig,
                                  attribList, esContext->hWnd) )
   {
      return GL_FALSE;
   }
   

   return GL_TRUE;
}

GLvoid ESUTIL_API esDestroySurface(ESContext *esContext)
{
    if (esContext->eglSurface != EGL_NO_SURFACE)
    {
        assert(esContext->eglDisplay != EGL_NO_DISPLAY);
        eglDestroySurface(esContext->eglDisplay, esContext->eglSurface);
        esContext->eglSurface = EGL_NO_SURFACE;
    }
}

GLvoid ESUTIL_API esDestroyContext(ESContext *esContext)
{
    if (esContext->eglContext != EGL_NO_CONTEXT)
    {
        assert(esContext->eglDisplay != EGL_NO_DISPLAY);
        eglDestroyContext(esContext->eglDisplay, esContext->eglContext);
        esContext->eglContext = EGL_NO_CONTEXT;
    }
}

GLvoid ESUTIL_API esTerminateDisplay(ESContext *esContext)
{
    if (esContext->eglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(esContext->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(esContext->eglDisplay);
        esContext->eglDisplay = EGL_NO_DISPLAY;
    }
}

GLvoid ESUTIL_API esDestroyGL(ESContext *esContext)
{
    esDestroySurface(esContext);
    esDestroyContext(esContext);
    esTerminateDisplay(esContext);
}

///
//  esCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//      flags  - bitwise or of window creation flags 
//          ES_WINDOW_ALPHA       - specifies that the framebuffer should have alpha
//          ES_WINDOW_DEPTH       - specifies that a depth buffer should be created
//          ES_WINDOW_STENCIL     - specifies that a stencil buffer should be created
//          ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
//
GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const wchar_t* title, GLint width, GLint height, GLuint flags )
{
   EGLint attribList[] =
   {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
       EGL_NONE
   };
   
   if ( esContext == NULL )
   {
      return GL_FALSE;
   }

   esContext->width = width;
   esContext->height = height;

   if ( !WinCreate ( esContext, title) )
   {
      return GL_FALSE;
   }

  
   if ( !CreateEGLContext ( esContext->hWnd,
                            &esContext->eglDisplay,
                            &esContext->eglContext,
                            &esContext->eglConfig,
                            attribList) )
   {
      return GL_FALSE;
   }

   if ( !CreateEGLSurface ( esContext->hWnd,
                            esContext->eglDisplay,
                            esContext->eglContext,
                            esContext->eglConfig,
                            &esContext->eglSurface))
   {
       return GL_FALSE;
   }

   

   return GL_TRUE;
}

///
//  esMainLoop()
//
//    Start the main loop for the OpenGL ES application
//
void ESUTIL_API esMainLoop ( ESContext *esContext )
{
   WinLoop ( esContext );
}


///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void (ESCALLBACK *drawFunc) (ESContext* ) )
{
   esContext->drawFunc = drawFunc;
}


///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void (ESCALLBACK *updateFunc) ( ESContext*, float ) )
{
   esContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext,
                                    void (ESCALLBACK *keyFunc) (ESContext*, unsigned char, int, int ) )
{
   esContext->keyFunc = keyFunc;
}


///
// esLogMessage()
//
//    Log an error message to the debug output for the platform
//
void ESUTIL_API esLogMessage ( const char *formatStr, ... )
{
    va_list params;
    char buf[BUFSIZ];

    va_start ( params, formatStr );
    vsprintf_s ( buf, sizeof(buf),  formatStr, params );
    
    printf ( "%s", buf );
    
    va_end ( params );
}


///
// esLoadTGA()
//
//    Loads a 24-bit TGA image from a file
//
char* ESUTIL_API esLoadTGA ( char *fileName, int *width, int *height )
{
   char *buffer;

   if ( WinTGALoad ( fileName, &buffer, width, height ) )
   {
      return buffer;
   }

   return NULL;
}