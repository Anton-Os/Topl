#include "Topl_Renderer_GL4.hpp"

/*#ifdef _WIN32
    static void setupPixFrmt_win(HWND hwnd){

    }
#endif

void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){

#ifdef _WIN32
    setupPixFrmt_win(hwnd);
#endif
} */

#ifdef _WIN32
static void init_win(HWND hwnd, HGLRC hglrc){
    // Creates an HDC based on the window
    HDC windowDC = GetDC(hwnd);

    // Pixel format descriptor stuff
    PIXELFORMATDESCRIPTOR pixDescript, *pixDescript_ptr;
	pixDescript_ptr = &pixDescript;
	int pixFrmt;

	pixDescript.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixDescript.nVersion = 1;
	pixDescript.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixDescript.dwLayerMask = PFD_MAIN_PLANE;
	pixDescript.iPixelType = PFD_TYPE_RGBA;
	pixDescript.cColorBits = 24;

	pixFrmt = ChoosePixelFormat(windowDC, pixDescript_ptr);
	BOOL pixFrmtChk = SetPixelFormat(windowDC, pixFrmt, pixDescript_ptr);

    // wgl initialization functions
    hglrc = wglCreateContext(windowDC);
    wglMakeCurrent(windowDC, hglrc);
}

static void cleanup_win(HGLRC hglrc){
    wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
}

// #define GL_INIT_FUNC(NATIVE_WINDOW hwnd, NATIVE_GL_CONTEXT gContext) init_win(hwnd, gContext);
// #define GL_CLEANUP_FUNC(NATIVE_GL_CONEXT GL_Ctx) cleanup_win(GL_Ctx);

#endif

void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){
    // GL_INIT_FUNC(hwnd, m_GL4_Ctx)
#ifdef _WIN32
    init_win(hwnd, m_GL4_Ctx);
#endif    
}

void Topl_Renderer_GL4::buildScene(Topl_SceneGraph sceneGraph){
    return;
}

void Topl_Renderer_GL4::render(void){
    return;
}

void Topl_Renderer_GL4::cleanup(void){
    // GL_CLEANUP_FUNC(m_GL4_Ctx)
#ifdef _WIN32
    cleanup_win(m_GL4_Ctx);
#endif
}