#include "GameApplication.h"
#include "../Resources/ResourceManager.h"
#include "../Scene/BackgroundNode.h"
#include "../Components/Event_DestroyActor.h"

#include <iostream>

std::shared_ptr<GameApplication> GameApplication::m_Instance = std::shared_ptr<GameApplication>();

GameApplication::GameApplication()
{
    m_Scene = new Scene;
    m_EventManager = new EventManager;
    m_ActorFactory = new ActorFactory;
}

GameApplication::~GameApplication()
{
    delete m_Scene;
    delete m_EventManager;
    delete m_ActorFactory;
}

void GameApplication::Initialize(float width, float height)
{
    m_ScreenWidth = width;
    m_ScreenHeight = height;
    // Initialize renderer
    m_Scene->GetRenderer()->Initialize();
    m_Scene->GetCamera()->SetProjection(width, height);
    // Load necessary actors/textures/shaders before loading level
    ResourceManager::GetInstance()->LoadShader("sprite", "shaders/sprite.vs", "shaders/sprite.frag");
    ResourceManager::GetInstance()->LoadTexture("block", "textures/block.png");
    ResourceManager::GetInstance()->LoadLevel(m_Scene, "levels/begin.lvl");
    // Create level-independant actors
    // - background (no need for it to be related to an actor, only for rendering)
    std::shared_ptr<Actor> backgroundActor = CreateActor(DEFAULT_ACTOR_TYPES::ACTOR_EMPTY);
    std::shared_ptr<BackgroundNode> backgroundNode(new BackgroundNode(backgroundActor->GetID())); // 0 = no_actor id
    Material backgroundMaterial;
    backgroundMaterial.SetShader(ResourceManager::GetInstance()->GetShader("sprite"));
    backgroundMaterial.SetDiffuse(ResourceManager::GetInstance()->LoadTexture("background", "textures/background.png"));
    backgroundNode->SetMaterial(backgroundMaterial);
    m_Scene->AddChild(backgroundActor->GetID(), backgroundNode);
    // - player
    std::shared_ptr<Actor> actor = CreateActor(DEFAULT_ACTOR_TYPES::ACTOR_PLAYER);
    actor->Position() = glm::vec2(150.0, 413.0);
    actor->Depth()    = 0;
    actor->Scale()    = glm::vec2(65.0);
    std::shared_ptr<SceneNode> node(new SceneNode(actor->GetID(), "player", "MAIN", actor->Position(), actor->Depth(), actor->Scale()));
    Material material;
    material.SetShader(ResourceManager::GetInstance()->GetShader("sprite"));
    material.SetDiffuse(ResourceManager::GetInstance()->LoadTexture("player", "textures/player.png", true));
    node->SetMaterial(material);
    m_Scene->AddChild(actor->GetID(), node);

    // initialize scene (e.g. set default projection matrices for each shader)
    m_Scene->Initialize();

}

std::shared_ptr<Actor> GameApplication::CreateActor(DEFAULT_ACTOR_TYPES type)
{
    std::shared_ptr<Actor> actor = m_ActorFactory->CreateActor(type);
    m_Actors.push_back(actor);
    return actor;
}

void GameApplication::Update(float deltaTime)
{
    // Update all actors
    auto it = m_Actors.begin();
    for(auto it = m_Actors.begin(); it != m_Actors.end(); it++)
        (*it)->Update(deltaTime);

    if (m_Keys[GLFW_KEY_5])
    {
        std::shared_ptr<Event_DestroyActor> pEvent(new Event_DestroyActor(1337));
        m_EventManager->QueueEvent(pEvent);
    }

    // Process all queued events
    m_EventManager->Update();

    // Update all scene components
    m_Scene->Update(deltaTime);
}

void GameApplication::Render()
{
    m_Scene->Render();
}

void GameApplication::ProcessKeyboardDown(char key)
{
    m_Keys[key] = true;
    //std::cout << key << std::endl;
}

void GameApplication::ProcessKeyboardUp(char key)
{
    m_Keys[key] = false;
    m_KeysPressed[key] = false;
    //std::cout << "OFF:" << key << std::endl;
}

bool GameApplication::IsKeyPressed(char key, bool check_once)
{
    if (check_once)
    {
        if (m_Keys[key] && !m_KeysPressed[key])
        {
            m_KeysPressed[key] = true;
            return true;
        }
        else
            return false;
    }
    return m_Keys[key];
}


void GameApplication::checkOtherInstances()
{
    // TODO
}

void GameApplication::checkStorage()
{
    // TODO
}

void GameApplication::checkRAM()
{
    // TODO
}