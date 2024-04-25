#pragma once

//Forward declarations
class Texture;

class NamedTexture final
{
public:
	NamedTexture(const std::string& name);
	~NamedTexture();
	NamedTexture(const NamedTexture& other) = delete;
	NamedTexture& operator=(const NamedTexture& other) = delete;
	NamedTexture(NamedTexture&& other) = delete;
	NamedTexture&& operator=(NamedTexture&& other) = delete;
	operator Texture&();

private:
	Texture& m_Texture;
	std::string m_Name;
};

