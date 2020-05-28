#include "FileIO.hpp"

#include "Topl_Renderer_GL4.hpp"

// This code is platform independent

namespace _GL4 {

	// GLChar** error;
	static void readShader(const std::string& filePath, const char** shaderStr){
		// Needs to be compatible with glShaderSource
		return;
	}
}

#ifdef _WIN32
static void init_win(const HWND* hwnd, HDC* windowDC, HGLRC* hglrc){
    // Creates an HDC based on the window
    *(windowDC) = GetDC(*(hwnd));

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

	pixFrmt = ChoosePixelFormat(*(windowDC), pixDescript_ptr);
	BOOL pixFrmtChk = SetPixelFormat(*(windowDC), pixFrmt, pixDescript_ptr);

    // wgl initialization functions
    *(hglrc) = wglCreateContext(*(windowDC));
    wglMakeCurrent(*(windowDC), *(hglrc));
}

static void render_win(HDC* windowDC) {
	//HDC windowDC = GetDC(hwnd); // Probably have to save the old HDC!!! This wont work

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(*(windowDC));
}

static void cleanup_win(HWND* hwnd, HDC* windowDC, HGLRC* hglrc){
    wglMakeCurrent(NULL, NULL);
	wglDeleteContext(*(hglrc));

	ReleaseDC(*(hwnd), *(windowDC));
}

#endif


Topl_Renderer_GL4::~Topl_Renderer_GL4() {
#ifdef _WIN32
	cleanup_win(m_native.window, &m_native.windowDevice_Ctx, &m_native.GL_Ctx);
#endif
}


void Topl_Renderer_GL4::init(NATIVE_WINDOW hwnd){
	m_native.window = &hwnd;
#ifdef _WIN32
    init_win(m_native.window, &m_native.windowDevice_Ctx, &m_native.GL_Ctx);
#endif
	glEnable(GL_DEPTH_TEST); // Make these customizable
	glDepthFunc(GL_LESS); // Make these customizable
	glClearColor(0.4f, 0.4f, 0.9f, 1.0f);

	// glGenBuffers()
	// (GL4_BUFFER_CAPACITY, &m_bufferData.slots[0]) // Might need glew
}

void Topl_Renderer_GL4::buildScene(const Topl_SceneGraph* sceneGraph){
    return; // To be continued
}

void Topl_Renderer_GL4::createPipeline(void){
	return;
}

void Topl_Renderer_GL4::render(void){
#ifdef _WIN32
	render_win(&m_native.windowDevice_Ctx);
#endif  
}