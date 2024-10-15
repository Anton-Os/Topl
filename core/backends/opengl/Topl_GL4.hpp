#include "Topl_Renderer.hpp"

namespace GL4 {
    // Buffer

    struct Buffer : public Buffer {
        Buffer(GLuint b){ buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }
        
        GLuint buffer; // OpenGL buffer
    };

    // Vertex Array Object

    struct VertexArray : public RenderObj {
        VertexArray() : RenderObj() {}
        VertexArray(unsigned id, GLuint v) : RenderObj(id){ vao = v; }

        GLuint vao;
    };

    // Texture

    struct Texture : public Texture {
        Texture() : Texture() {}
        Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, f, m) { texture = t; }
        Texture(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, b, f, m) { texture = t; }

        GLuint texture;
    };

    // Functions

    GLenum getShaderFormat(enum SHDR_ValueType type);
	void genVertexArrayLayout(VertexArray_GL4* VAO, entry_shader_cptr entryShader);

    GLuint compileShader(std::string shaderText, GLenum shaderType);

	void setTextureProperties(GLenum type, TEX_Mode m);
}