#include "pch.h"
#include "LevelEditor.h"
#include <filesystem>
#include <fstream>
#include "Level.h"
#include "LevelScreen.h"
#include "Camera.h"
#include "AssetManager.h"
#include "InputManager.h"
#include <functional>

LevelEditor::LevelEditor(const Rectf& viewport)
	: m_Viewport{ viewport }
    , m_ResolutionScale{ Point2f{1.f, 1.f } }
    , m_pCamera{ new Camera(Point2f{ m_Viewport.width, m_Viewport.height }, m_ResolutionScale) }
    , m_Mode{}
    , m_NUM_MODES{ 6 }
    , m_MODE_NAMES{ std::vector<std::string>{"ModeSelect", "RunLevel", "CreateLevel", "EditLevel", "CreateLevelScreen", "EditLevelScreen"} }
    , m_pCurLevel{ nullptr }
    , m_pCurLevelScreen{ nullptr }
{
    //To-Do: wrap this in a function
    std::function<void(void)> fPressedEnter = std::bind(&LevelEditor::PressedEnter, this);
    InputManager::RegisterCallback(InputManager::Event::PressedEnter, fPressedEnter);

    std::function<void(void)> fPressedEscape = std::bind(&LevelEditor::PressedEscape, this);
    InputManager::RegisterCallback(InputManager::Event::PressedEscape, fPressedEscape); 

    std::function<void(void)> fClickedLMB = std::bind(&LevelEditor::ClickedLMB, this);
    InputManager::RegisterCallback(InputManager::Event::ClickedLMB, fClickedLMB);

    std::function<void(void)> fScrollThroughModes = std::bind(&LevelEditor::ScrollThroughModes, this);
    InputManager::RegisterCallback(InputManager::Event::ScrollingMMB, fScrollThroughModes);

    SetModeSelect();
}

LevelEditor::~LevelEditor()
{
    delete m_pCurLevel;
    delete m_pCurLevelScreen;
    delete m_pCamera;
}

void LevelEditor::Draw() const
{
    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
        m_pCurLevelScreen->Draw();
        break;
    }
}

void LevelEditor::Update(float dt)
{
    if (m_EnteredMode)
    {
        switch (m_Mode)
        {
        //case Mode::ModeSelect:
        //    if (InputManager::GetMouseInfo().scrollingMMB)
        //        ScrollThroughModes();
        //    break;
        case Mode::EditLevelScreen:
            EditLevelScreen();
            break;
        }
    }
}

void LevelEditor::PressedEnter()
{
    switch (m_Mode)
    {
    case Mode::ModeSelect:
        EnterSelectedMode();
        break;
    }
}

void LevelEditor::PressedEscape()
{
    if (m_Mode != Mode::ModeSelect)
        SetModeSelect();
}

void LevelEditor::ClickedLMB()
{
}

void LevelEditor::ScrollThroughModes()
{
    if (m_Mode != Mode::ModeSelect)
        return;

    int direction{ InputManager::GetMouseInfo().scrollDir };
    int modeIdx{ int(m_SelectedMode) };
    if (direction > 0)
    {
        ++modeIdx;
        if (modeIdx > m_NUM_MODES - 1)
            modeIdx = 1;
    }
    else if (direction < 0)
    {
        --modeIdx;
        if (modeIdx < 1)
            modeIdx = m_NUM_MODES - 1;
    }
    SetSelectedMode(Mode(modeIdx));
}

void LevelEditor::SetModeSelect()
{
    SetSelectedMode(Mode::ModeSelect, true);
}

void LevelEditor::CreateLevel()
{
    PrintHeader("Creating a new Level");

    //Get values from user
	std::string levelName{};
	std::cout << "Enter a level name: ";
	std::cin >> levelName;

    const std::string& dir(AssetManager::GetDir(AssetManager::Dir::LevelData));
    CreateDirIfDoesntExist(dir);

    //Create metadata file for level
    std::string fileType{ ".txt" };
    std::string metadataFilePath{ dir + levelName + fileType };
    std::ofstream outputFile{ metadataFilePath };
    std::cout << "Created: " + metadataFilePath << std::endl;

    if (!IsFileOpen(outputFile))
        return;

    // Write variables to the file
    outputFile << "levelName: " << levelName << std::endl;

    CloseFile(std::move(outputFile));

    //m_pCurLevel = new Level(m_Viewport, m_pInputManager);

    SetSelectedMode(Mode::CreateLevelScreen, true);
}

void LevelEditor::CreateLevelScreen()
{
    PrintHeader("Creating a new LevelScreen");

    //Get values from user
    std::string levelScreenName{};
    std::cout << "Enter a levelScreen name: ";
    std::cin >> levelScreenName;
    int rows{};
    std::cout << "Enter the number of rows: ";
    std::cin >> rows;
    int cols{};
    std::cout << "Enter the number of columns: ";
    std::cin >> cols;

    const std::string& dir{ AssetManager::GetDir(AssetManager::Dir::LevelScreenData) };
    CreateDirIfDoesntExist(dir);

    std::string blueprintPath{ dir + "Blueprint.bmp" };
    //Steal format from Blueprint.bmp on disk
    SDL_Surface* pBlueprintSurface{ IMG_Load(blueprintPath.c_str()) };
    if (!pBlueprintSurface)
    {
        std::cout << "LevelEditor::CreateLevelScreen(): Didn't find: " << blueprintPath << std::endl;
        SetModeSelect();
        return;
    }
    const SDL_PixelFormat& bpFormat{ *(pBlueprintSurface->format) };

    //Create new levelScreen.bmp file on disk
    SDL_Surface* pSurface{ SDL_CreateRGBSurface(0, cols, rows, bpFormat.BitsPerPixel, bpFormat.Rmask, bpFormat.Gmask, bpFormat.Bmask, bpFormat.Amask) };
    
    //Copy color pallette from blueprint format
    SDL_LockSurface(pSurface);
    SDL_SetPaletteColors(pSurface->format->palette, bpFormat.palette->colors, 0, bpFormat.palette->ncolors);
    SDL_UnlockSurface(pSurface);

    //Save SDL_Surface to disk as .bmp file
    std::string textureFilePath{ dir + levelScreenName + ".bmp" };
    int result = SDL_SaveBMP(pSurface, textureFilePath.c_str());
    if (result == -1)
    {
        std::cout << "LevelEditor::CreateLevelScreen(): SDL_SaveBMP couldn't save surface to " << textureFilePath << std::endl;
        return;
    }
    else
        std::cout << "Created: " << textureFilePath << std::endl;

    //Create metadata file for levelScreen
    std::string fileType{ ".txt" };
    std::string metadataFilePath{ dir + levelScreenName + fileType };
    std::ofstream outputFile{ metadataFilePath };
    std::cout << "Created: " + metadataFilePath << std::endl;

    if (!IsFileOpen(outputFile))
        return;

    // Write variables to the file
    outputFile << "TextureNames: " << "grass reflection" << std::endl;
    outputFile << "IdToTextureIdx: " << "-1 -1 0 0 1" << std::endl;

    CloseFile(std::move(outputFile));

    m_pCurLevelScreen = new LevelScreen(levelScreenName);

    SetSelectedMode(Mode::EditLevelScreen, true);
}

void LevelEditor::EditLevelScreen()
{
    if (!m_pCurLevelScreen)
        SelectLevelScreenToEdit();

    //scale resolution
}

void LevelEditor::SelectLevelScreenToEdit()
{
    PrintHeader("Selecting a level screen to edit");

    std::cout << "Enter a level screen name: ";
    std::string name{};
    std::cin >> name;
    const std::string& dir{ AssetManager::GetDir(AssetManager::Dir::LevelScreenData) };
    std::string path{ dir + name + ".bmp"};
    if (!std::filesystem::exists(path))
    {
        std::cout << "LevelEditor::SelectLevelScreenToEdit(): Couldn't find: " << path << std::endl;
        SetModeSelect();
        return;
    }
    //Load the levelScreen
    m_pCurLevelScreen = new LevelScreen(name);
}

void LevelEditor::SetSelectedMode(Mode mode, bool enter)
{
    m_SelectedMode = mode;
    if (enter)
        EnterSelectedMode();
    else
    {
        std::cout << "Selected Mode: " << m_MODE_NAMES[int(m_SelectedMode)] << ", ";
        std::cout << "Press Enter to activate mode." << std::endl;
    }
}

void LevelEditor::EnterSelectedMode()
{
    m_Mode = m_SelectedMode;
    std::cout << "Mode changed to: " << m_MODE_NAMES[int(m_Mode)] << std::endl;
    m_EnteredMode = true;
    switch (m_Mode)
    {
    case Mode::ModeSelect:
        m_EnteredMode = false;
        std::cout << "Use scroll wheel to select a mode." << std::endl;
        break;
    case Mode::CreateLevel:
        CreateLevel();
        break;
    case Mode::CreateLevelScreen:
        CreateLevelScreen();
        break;
    case Mode::EditLevelScreen:
        if (!m_pCurLevelScreen)
            SelectLevelScreenToEdit();
        break;
    }
}

void LevelEditor::CreateDirIfDoesntExist(const std::string& dir)
{
    //create directory
    if (!std::filesystem::exists(dir))
    {
        bool result{ std::filesystem::create_directories(dir) };
        if (!result)
        {
            std::cout << "LevelEditor::CreateDirIfDoesntExist(): couldn't create dir: " << dir << std::endl;
            SetModeSelect();
        }
    }
}

bool LevelEditor::IsFileOpen(std::ofstream& file)
{
    // Check if the file is opened successfully
    if (!file.is_open())
    {
        std::cout << "LevelEditor::IsFileOpen(): Error opening file." << std::endl;
        SetModeSelect();
        return false;
    }
    else
        return true;
}

void LevelEditor::CloseFile(std::ofstream&& file)
{
    file.close();
}

void LevelEditor::PrintHeader(const std::string& text)
{
    std::cout << "#################" << std::endl;
    std::cout << text << std::endl;
}
