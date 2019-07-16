#ifndef stringbufferH
#define stringbufferH

#include "TextureQuad.h"

struct stString {
	stString() {
		BufferOffset = -1;
		Value = "";
		Position = glm::vec2(0, 0);
		Scale = glm::vec2(0, 0);
		Size = glm::vec2(0, 0); 
	}
	int BufferOffset;
	string Value;
	glm::vec2 Scale;
	glm::vec2 Position;
	glm::vec2 Size;
};

class MStringBuffer {
private:
	GLenum BuffersType;
	unsigned int BindNumber;
	unsigned int FontTextureId;
	GLuint VerticesId;
	GLuint UVsId;
	int TextureSize[2];
	int FontSize[2];
	int RowSize;
	int ColumnSize;
	glm::vec2 FontUVSize;
	glm::vec2 SpaceSize;
	
	vector<stString> StringList; //store strings or pointers to strings?
	vector<glm::vec2> Vertices;
	vector<glm::vec2> UVs;
	
	stString CreateString(const char* Value, glm::vec2 Position, glm::vec2 Scale, int ExistingNumber);
public:
	MStringBuffer();
	bool Initialize(unsigned int inFontTextureId, int inTextureSizeX, int TextureSizeY, int RowSize, int ColumnSize);
	int AddString(const char* Value, glm::vec2 Position, glm::vec2 Scale);
	bool UpdateString(int Number, const char* Value, glm::vec2 Position, glm::vec2 Scale);
	bool UpdateStringValue(int Number, const char* Value);
	bool UpdateStringPosition(int Number, glm::vec2 Position);
	bool UpdateStringScale(int Number, glm::vec2 Scale);
	bool RemoveString(int Number);
	
	bool MoveString(int Number, glm::vec2 Step);
	
	void Begin();
	void End();
	void DrawString(int Number);
	void DrawAll();
	void Relocate();
	void Clear();
	void Close();
	
	int GetStringListSize();
	stString GetString(int Number);
};

#endif
