#pragma once
#include <unordered_set>

//Forward declarations
class Texture;

class ResourceManager final
{
public:
	ResourceManager();
	~ResourceManager();

	static Texture* GetTexture(const std::string& name) const;
private:
	std::unordered_set<std::string> m_TextureNames;
	std::vector<Texture*> m_pTextures;
};

