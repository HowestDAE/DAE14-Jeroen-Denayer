#include "pch.h"
#include "LevelEditor.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Level.h"
#include "LevelScreen.h"
#include "Camera.h"
#include "AssetManager.h"
#include "InputManager.h"
#include "GameData.h"
#include <functional>
#include "FileIO.h"
#include "Texture.h"

LevelEditor::LevelEditor()
    : m_MousePos{ Vector2f{} }
    , m_DefaultMode{ Mode::ModeSelect }
    , m_Mode{}
    , m_SelectedMode{}
    , m_ModeNames{ std::vector<std::string>{"ModeSelect", "RunLevel", "CreateLevel", "EditLevel", "CreateLevelScreen", "EditLevelScreen"} }
    , m_EditLevelScreenMode{ EditLevelScreenMode::Default }
    , m_EditLevelScreenModeNames{ std::vector<std::string>{"Default", "AddCrystal"}}
    , m_pCamera{ new Camera() }
    , m_pCurLevel{ nullptr }
    , m_pCurLevelScreen{ nullptr }
    , m_PreviewTexture{ nullptr }
{
    //To-Do: wrap this in a function
    std::function<void(void)> fClickedLMB = std::bind(&LevelEditor::ClickedLMB, this);
    InputManager::RegisterCallback(InputManager::MouseEvent::ClickedLMB, fClickedLMB, GameData::Mode::RunEditor);

    std::function<void(void)> fScrollingMMB = std::bind(&LevelEditor::ScrollingMMB, this);
    InputManager::RegisterCallback(InputManager::MouseEvent::ScrollingMMB, fScrollingMMB, GameData::Mode::RunEditor);

    std::function<void(void)> fDraggingMMB = std::bind(&LevelEditor::DraggingMMB, this);
    InputManager::RegisterCallback(InputManager::MouseEvent::DraggingMMB, fDraggingMMB, GameData::Mode::RunEditor);

    std::function<void(void)> fKeyPressed = std::bind(&LevelEditor::KeyPressed, this);
    InputManager::RegisterCallback(InputManager::Key::None, fKeyPressed, GameData::Mode::RunEditor);

    SetDefaultMode();
}

LevelEditor::~LevelEditor()
{
    delete m_pCurLevel;
    delete m_pCurLevelScreen;
    delete m_pCamera;
    AssetManager::RemoveTexture(m_PreviewTexture);
}

void LevelEditor::Draw() const
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
    {
        Point2f levelScreenDimensions{ m_pCurLevelScreen->GetDimensions() };
        m_pCamera->Aim(Rectf{ 0.f, 0.f, levelScreenDimensions.x, levelScreenDimensions.y });
        m_pCurLevelScreen->Draw();

        switch (m_EditLevelScreenMode)
        {
        case EditLevelScreenMode::Default:
        {
            //Draw circle around mousePos
            Vector2f mousePos{ InputManager::GetMouseInfo().pos };
            utils::SetColor(Color4f{ 0.f, 1.f, 1.f, 1.f });
            utils::DrawEllipse(mousePos.x, mousePos.y, 5.f, 5.f, 2.f);
            break;
        }
        case EditLevelScreenMode::AddCrystal:
        {
            Vector2f pos{ m_pCamera->GetWorldPos(m_MousePos) };
            pos -= Vector2f{ m_PreviewTexture->GetWidth() / 2, m_PreviewTexture->GetHeight() / 2 };
            m_PreviewTexture->Draw(Point2f{ pos.x, pos.y });
            break;
        }
        }
        m_pCamera->Reset();
        break;
    }
    }
}

void LevelEditor::Update(float dt)
{
    m_MousePos = InputManager::GetMouseInfo().pos;
    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
        break;
    }
}

void LevelEditor::KeyPressed()
{
    switch (m_Mode)
    {
    case Mode::ModeSelect:
        if (InputManager::IsKeyPressed(InputManager::Key::Enter))
            EnterSelectedMode();
        break;
    case Mode::EditLevelScreen:
        if (InputManager::IsKeyPressed(InputManager::Key::Escape))
            if (m_EditLevelScreenMode == EditLevelScreenMode::Default)
                SetDefaultMode();
            else
                SetEditLevelScreenMode(EditLevelScreenMode::Default);
        else if (InputManager::IsKeyPressed(InputManager::Key::ctrl) &&
                 InputManager::IsKeyPressed(InputManager::Key::S))
            m_pCurLevelScreen->SaveData();
        else if (InputManager::IsKeyPressed(InputManager::Key::E))
            SelectLevelScreenToEdit();
        else if (InputManager::IsKeyPressed(InputManager::Key::F))
            m_pCamera->Focus();
        else if (InputManager::IsKeyPressed(InputManager::Key::C))
            SetEditLevelScreenMode(EditLevelScreenMode::AddCrystal);
        break;
    }   
}


void LevelEditor::ClickedLMB()
{
    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
        switch (m_EditLevelScreenMode)
        {
        case EditLevelScreenMode::Default:
        {
            Vector2f mousePos{ InputManager::GetMouseInfo().pos };
            Vector2f worldPos{ m_pCamera->GetWorldPos(mousePos) };
            TileIdx tileIdx{ utils::GetTileIdxByPos(worldPos, GameData::TILE_SIZE_PIX()) };
            std::cout << tileIdx.r << " " << tileIdx.c << std::endl;
            break;
        }
        case EditLevelScreenMode::AddCrystal:
            m_pCurLevelScreen->AddCrystal(m_pCamera->GetWorldPos(m_MousePos));
            break;
        }
    }
}

void LevelEditor::ScrollingMMB()
{
    switch (m_Mode)
    {
    case Mode::ModeSelect:
        ScrollThroughModes();
        break;
    case Mode::EditLevelScreen:
    {
        int dir{ InputManager::GetMouseInfo().scrollDir };
        m_pCamera->Zoom(dir);
        break;
    }
    }
}

void LevelEditor::DraggingMMB()
{
    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
        Vector2f offset{ InputManager::GetMouseInfo().dragDist };
        m_pCamera->Offset(offset);
        break;
    }
}

void LevelEditor::EditCurrentMode()
{
    switch (m_Mode)
    {
    case Mode::EditLevelScreen:
        SelectLevelScreenToEdit();
        break;
    }
}

void LevelEditor::SetDefaultMode()
{
    SetSelectedMode(m_DefaultMode, true);
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
        if (modeIdx > m_ModeNames.size() - 1)
            modeIdx = 1;
    }
    else if (direction < 0)
    {
        --modeIdx;
        if (modeIdx < 1)
            modeIdx = int(m_ModeNames.size()) - 1;
    }
    SetSelectedMode(Mode(modeIdx));
}

void LevelEditor::CreateLevel()
{
    PrintHeader("Creating a new Level");

    //Get values from user
	std::string levelName{};
	std::cout << "Enter a level name: ";
	std::cin >> levelName;

    const std::string& dir(FileIO::GetDir(FileIO::Dir::LevelData));
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

    const std::string& dir{ FileIO::GetDir(FileIO::Dir::LevelScreenData) };
    CreateDirIfDoesntExist(dir);

    std::string blueprintPath{ dir + "Blueprint.bmp" };
    //Steal format from Blueprint.bmp on disk
    SDL_Surface* pBlueprintSurface{ IMG_Load(blueprintPath.c_str()) };
    if (!pBlueprintSurface)
    {
        std::cout << "LevelEditor::CreateLevelScreen(): Didn't find: " << blueprintPath << std::endl;
        SetDefaultMode();
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

void LevelEditor::SelectLevelScreenToEdit()
{
    PrintHeader("Selecting a level screen to edit");

    std::cout << "Enter a level screen name: ";
    std::string name{};
    std::cin >> name;
    std::stringstream path;
    path << FileIO::GetDir(FileIO::Dir::LevelScreenData) << name + ".bmp";
    if (!std::filesystem::exists(path.str()))
    {
        std::cout << "LevelEditor::SelectLevelScreenToEdit(): Couldn't find: " << path.str() << std::endl;
        SetDefaultMode();
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
        std::cout << "Selected Mode: " << m_ModeNames[int(m_SelectedMode)] << ", ";
        std::cout << "Press Enter to activate mode." << std::endl;
    }
}

void LevelEditor::EnterSelectedMode()
{
    m_Mode = m_SelectedMode;
    std::cout << "Mode changed to: " << m_ModeNames[int(m_Mode)] << std::endl;
    switch (m_Mode)
    {
    case Mode::ModeSelect:
        std::cout << "Use scroll wheel to select a mode." << std::endl;
        break;
    case Mode::CreateLevel:
        CreateLevel();
        break;
    case Mode::CreateLevelScreen:
        CreateLevelScreen();
        break;
    case Mode::EditLevelScreen:
        SetEditLevelScreenMode(EditLevelScreenMode::Default);
        if (!m_pCurLevelScreen)
            SelectLevelScreenToEdit();
        m_PreviewTexture = AssetManager::GetTexture("Crystal");
        break;
    }
}

void LevelEditor::SetEditLevelScreenMode(EditLevelScreenMode mode)
{
    if (m_EditLevelScreenMode == mode)
        return;
    m_EditLevelScreenMode = mode;
    int idx{ int(m_EditLevelScreenMode) };
    std::cout << "EditLevelScreen mode changed to: " << m_EditLevelScreenModeNames[idx] << std::endl;
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
            SetDefaultMode();
        }
    }
}

bool LevelEditor::IsFileOpen(std::ofstream& file)
{
    // Check if the file is opened successfully
    if (!file.is_open())
    {
        std::cout << "LevelEditor::IsFileOpen(): Error opening file." << std::endl;
        SetDefaultMode();
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
