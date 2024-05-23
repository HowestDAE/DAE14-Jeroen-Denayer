#pragma once

//Forward declarations
class SoundStream;

class SoundManager
{
public:
	static SoundManager& Get();

	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) = delete;
	SoundManager& operator=(SoundManager&& other) = delete;
private:
	SoundManager();
};

