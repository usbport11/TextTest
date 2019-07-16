#include "../../stdafx.h"
#include "StringBuffer.h"

MStringBuffer::MStringBuffer() {
	BindNumber = 0;
	FontTextureId = 0;
	VerticesId = -1;
	UVsId = -1;
	TextureSize[0] = TextureSize[1] = -1;
	FontSize[0] = FontSize[1] = -1;
	RowSize = -1;
	ColumnSize = -1;
	FontUVSize = glm::vec2(0, 0);
	SpaceSize = glm::vec2(-16, 0);
	BuffersType = GL_STREAM_DRAW;
}

bool MStringBuffer::Initialize(unsigned int inFontTextureId, int inTextureSizeX, int inTextureSizeY, int inRowSize, int inColumnSize) {
	if(inFontTextureId <= 0) {
		LogFile<<"String Buffer: NULL font texture id"<<endl;
		return false;
	}
	if(inTextureSizeX <= 0 || inTextureSizeY <= 0 || !IsPowerOfTwo(inTextureSizeX) || !IsPowerOfTwo(inTextureSizeY)) {
		LogFile<<"String Buffer: wrong texture size"<<endl;
		return false;
	}
	if(inRowSize <= 0 || inColumnSize <= 0 || (inRowSize * inColumnSize < 256)) {
		LogFile<<"String Buffer: font parameters"<<endl;
		return false;
	}
	
	FontTextureId = inFontTextureId;
	TextureSize[0] = inTextureSizeX;
	TextureSize[1] = inTextureSizeY;
	RowSize = inRowSize;
	ColumnSize = inColumnSize;
	
	FontSize[0] = TextureSize[0] / RowSize;
	FontSize[1] = TextureSize[1] / ColumnSize;
	
	FontUVSize = glm::vec2((float)1 / RowSize, (float)1 / ColumnSize);
	
	glGenBuffers(1, &VerticesId);
	glBindBuffer(GL_ARRAY_BUFFER, VerticesId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec2), NULL, BuffersType);
	glGenBuffers(1, &UVsId);
	glBindBuffer(GL_ARRAY_BUFFER, UVsId);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), NULL, BuffersType);
	
	return true;
}

stString MStringBuffer::CreateString(const char* Value, glm::vec2 Position, glm::vec2 Scale, int ExistingNumber = 0) {
	stString NewString;
	int StringLength = strlen(Value);
	
	int SubStringStart = 0;
	int SubStringNumber = 0;
	glm::vec2 FontUVPosition;
	glm::vec2 Size = glm::vec2(FontSize[0] * Scale.x, FontSize[1] * Scale.y);
	stTextureQuad TextureQuad;
	
	NewString.Value = Value;
	NewString.Scale = Scale;
	NewString.BufferOffset = Vertices.size();
	NewString.Position = Position;
	NewString.Size = glm::vec2(0, 0);
	
	if(!ExistingNumber) {
		for(unsigned int i=0; i<StringLength; i++) {
			if(Value[i] == 0x0A) {
				if(NewString.Size.x < (i - SubStringStart) * Size.x + SpaceSize.x * (i - SubStringStart - 1))
					NewString.Size.x = (i - SubStringStart) * Size.x + SpaceSize.x * (i - SubStringStart - 1);
				SubStringStart = i + 1;
				SubStringNumber ++;
				NewString.Size.y = (SubStringNumber + 1) * Size.y;
				NewString.Position.y -= Size.y;
			}
			FontUVPosition.x = (float)((unsigned char)Value[i] % RowSize) / RowSize;
			FontUVPosition.y = (float)(1 - (float)((unsigned char)Value[i] / ColumnSize) / ColumnSize + (float)FontSize[1] / TextureSize[1]);
			TextureQuad = stTextureQuad(Position + SpaceSize * glm::vec2(i - SubStringStart, 1) + glm::vec2(Size.x * (i - SubStringStart), -Size.y * SubStringNumber), Size, FontUVPosition, FontUVSize);
			Vertices.push_back(TextureQuad.v[0]);
			Vertices.push_back(TextureQuad.v[1]);
			Vertices.push_back(TextureQuad.v[2]);
			Vertices.push_back(TextureQuad.v[3]);
			UVs.push_back(TextureQuad.t[0]);
			UVs.push_back(TextureQuad.t[1]);
			UVs.push_back(TextureQuad.t[2]);
			UVs.push_back(TextureQuad.t[3]);
		}
	}
	else {
		int Offset = StringList[ExistingNumber].BufferOffset;
		for(unsigned int i=0; i<StringLength; i++) {
			if(Value[i] == 0x0A) {
				if(NewString.Size.x < (i - SubStringStart) * Size.x + SpaceSize.x * (i - SubStringStart - 1))
					NewString.Size.x = (i - SubStringStart) * Size.x + SpaceSize.x * (i - SubStringStart - 1);
				SubStringStart = i + 1;
				SubStringNumber ++;
				NewString.Size.y = (SubStringNumber + 1) * Size.y;
				NewString.Position.y -= Size.y;
			}
			FontUVPosition.x = (float)((unsigned char)Value[i] % RowSize) / RowSize;
			FontUVPosition.y = (float)(1 - (float)((unsigned char)Value[i] / ColumnSize) / ColumnSize + (float)FontSize[1] / TextureSize[1]);
			TextureQuad = stTextureQuad(Position + SpaceSize * glm::vec2(i - SubStringStart, 1) + glm::vec2(Size.x * (i - SubStringStart), -Size.y * SubStringNumber), Size, FontUVPosition, FontUVSize);
			Vertices[Offset + i * 4 + 0] = TextureQuad.v[0];
			Vertices[Offset + i * 4 + 1] = TextureQuad.v[1];
			Vertices[Offset + i * 4 + 2] = TextureQuad.v[2];
			Vertices[Offset + i * 4 + 3] = TextureQuad.v[3];
			UVs[Offset + i * 4 + 0] = TextureQuad.t[0];
			UVs[Offset + i * 4 + 1] = TextureQuad.t[1];
			UVs[Offset + i * 4 + 2] = TextureQuad.t[2];
			UVs[Offset + i * 4 + 3] = TextureQuad.t[3];
		}
	}
	if(NewString.Size.x == 0 && NewString.Size.y == 0) {
		NewString.Size = glm::vec2(Size.x * StringLength, Size.y) + SpaceSize * glm::vec2((StringLength - 1), 1);
	}
	
	return NewString;
}

int MStringBuffer::AddString(const char* Value, glm::vec2 Position, glm::vec2 Scale) {
	if(!Value) {
		LogFile<<"String Buffer: Can't add string. Null value"<<endl;
		return -1;
	}
	if(Scale.x <= 0 || Scale.y <= 0) {
		LogFile<<"String Buffer: Can't add string. Wrong scale"<<endl;
		return -1;
	}
	if(strlen(Value) <= 0) {
		LogFile<<"String Buffer: Can't add string. Null length"<<endl;
		return -1;
	}

	StringList.push_back(CreateString(Value, Position, Scale));
	
	return StringList.size() - 1;
}

bool MStringBuffer::UpdateString(int Number, const char* Value, glm::vec2 Position, glm::vec2 Scale) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}
	if(!Value) {
		LogFile<<"String Buffer: Can't add string. Null value"<<endl;
		return -1;
	}
	if(Scale.x <= 0 || Scale.y <= 0) {
		LogFile<<"String Buffer: Can't add string. Wrong scale"<<endl;
		return -1;
	}
	
	//if we have same length then we not needed to delete data and add it in the end of buffer, just update
	if(strlen(Value) != StringList[Number].Value.length()) {
		Vertices.erase(Vertices.begin() + StringList[Number].BufferOffset, Vertices.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
		UVs.erase(UVs.begin() + StringList[Number].BufferOffset, UVs.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
		StringList[Number] = CreateString(Value, Position, Scale);
	}
	else {
		StringList[Number] = CreateString(Value, Position, Scale, Number);
	}
	
	return true;
}

bool MStringBuffer::UpdateStringValue(int Number, const char* Value) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}
	if(!Value) {
		LogFile<<"String Buffer: Can't add string. Null value"<<endl;
		return -1;
	}
	
	//if we have same length then we not needed to delete data and add it in the end of buffer, just update
	if(strlen(Value) != StringList[Number].Value.length()) {
		Vertices.erase(Vertices.begin() + StringList[Number].BufferOffset, Vertices.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
		UVs.erase(UVs.begin() + StringList[Number].BufferOffset, UVs.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
		StringList[Number] = CreateString(Value, StringList[Number].Position, StringList[Number].Scale);
	}
	else {
		StringList[Number] = CreateString(Value, StringList[Number].Position, StringList[Number].Scale, Number);
	}
	
	return true;
}

bool MStringBuffer::UpdateStringPosition(int Number, glm::vec2 Position) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}
	
	int Offset = StringList[Number].BufferOffset;
	int Length = StringList[Number].Value.length();
	glm::vec2 PositionOffset = Position - StringList[Number].Position;
	for(unsigned int i=0; i<Length; i++) {
		Vertices[Offset + i * 4 + 0] += PositionOffset;
		Vertices[Offset + i * 4 + 1] += PositionOffset;
		Vertices[Offset + i * 4 + 2] += PositionOffset;
		Vertices[Offset + i * 4 + 3] += PositionOffset;
	}
	StringList[Number].Position = Position;
	
	return true;
}

bool MStringBuffer::UpdateStringScale(int Number, glm::vec2 Scale) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}
	if(Scale.x <= 0 || Scale.y <= 0) {
		LogFile<<"String Buffer: Can't add string. Wrong scale"<<endl;
		return -1;
	}
	
	int Offset = StringList[Number].BufferOffset;
	int Length = StringList[Number].Value.length();
	glm::vec2 Size = glm::vec2(FontSize[0] * Scale.x, FontSize[1] * Scale.y);
	stTextureQuad TextureQuad;
	for(unsigned int i=0; i<Length; i++) {
		TextureQuad = stTextureQuad(Vertices[Offset + i * 4], Size, glm::vec2(0, 0), glm::vec2(1, 1));
		Vertices[Offset + i * 4 + 0] = TextureQuad.v[0];
		Vertices[Offset + i * 4 + 1] = TextureQuad.v[1];
		Vertices[Offset + i * 4 + 2] = TextureQuad.v[2];
		Vertices[Offset + i * 4 + 3] = TextureQuad.v[3];
	}
	StringList[Number].Scale = Scale;
	
	return true;
}

bool MStringBuffer::RemoveString(int Number) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}

	Vertices.erase(Vertices.begin() + StringList[Number].BufferOffset, Vertices.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
	UVs.erase(UVs.begin() + StringList[Number].BufferOffset, UVs.begin() + StringList[Number].BufferOffset + 4 * StringList[Number].Value.length());
	StringList.erase(StringList.begin() + Number);
	
	return false;
}

bool MStringBuffer::MoveString(int Number, glm::vec2 Step) {
	if(Number >= StringList.size() || Number < 0) {
		LogFile<<"String Buffer: Can't update string. Not existing string"<<endl;
		return false;
	}
	return UpdateStringPosition(Number, StringList[Number].Position + Step);
}

void MStringBuffer::Begin() {
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
}

void MStringBuffer::End() {
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MStringBuffer::DrawString(int Number) {
	glActiveTexture(GL_TEXTURE0 + BindNumber);
	glBindTexture(GL_TEXTURE_2D, FontTextureId);
	glBindBuffer(GL_ARRAY_BUFFER, VerticesId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, UVsId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_QUADS, StringList[Number].BufferOffset * 4, StringList[Number].Value.length() * 4);
}

void MStringBuffer::DrawAll() {
	glActiveTexture(GL_TEXTURE0 + BindNumber);
	glBindTexture(GL_TEXTURE_2D, FontTextureId);
	glBindBuffer(GL_ARRAY_BUFFER, VerticesId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, UVsId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_QUADS, 0, Vertices.size());
}

void MStringBuffer::Relocate() {
	glBindBuffer(GL_ARRAY_BUFFER, VerticesId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec2), NULL, BuffersType);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec2), &Vertices[0][0], BuffersType);
	glBindBuffer(GL_ARRAY_BUFFER, UVsId);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), NULL, BuffersType);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0][0], BuffersType);
}

void MStringBuffer::Clear() {
	Vertices.clear();
	UVs.clear();
	StringList.clear();
	glBindBuffer(GL_ARRAY_BUFFER, VerticesId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec2), NULL, BuffersType);
	glBindBuffer(GL_ARRAY_BUFFER, UVsId);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), NULL, BuffersType);
}

void MStringBuffer::Close() {
	Clear();
	glDeleteBuffers(1, &VerticesId);
	glDeleteBuffers(1, &UVsId);
}
	
int MStringBuffer::GetStringListSize() {
	return StringList.size();
}

stString MStringBuffer::GetString(int Number) {
	if(Number >= StringList.size() || Number < 0) return stString();
	return StringList[Number];
}
