#include "renderer/Topl_Renderer.hpp"

namespace GL4 {
    // Vertex Array Object

    struct VertexArray : public Topl_RenderObj {
        VertexArray() : Topl_RenderObj() {}
        VertexArray(unsigned id, GLuint v) : Topl_RenderObj(id){ vao = v; }

        GLuint vao;
    };

    // Buffer

    struct Buffer : public Topl_Buffer {
        Buffer(GLuint b){ buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, GLuint b) : Topl_Buffer(id, t){ buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Topl_Buffer(id, t, c) { buffer = b; }
        
        GLuint buffer; // OpenGL buffer
    };

    // Texture

    struct Texture : public Topl_Texture {
        Texture() : Topl_Texture() {}
        Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Topl_Texture(id, f, m) { texture = t; }
        Texture(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Topl_Texture(id, b, f, m) { texture = t; }

        GLuint texture;
    };

    // Functions

    GLenum getShaderFormat(enum SHDR_ValueType type);
	void genVertexArrayLayout(GL4::VertexArray* VAO, entry_shader_cptr entryShader);

    GLuint compileShader(std::string shaderText, GLenum shaderType);

	void setTextureProperties(GLenum type, TEX_Mode m);
}