
#include "Graphics/Graphics.h"
#include "Input/Input.h"

#include "SceneGameOver.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "Audio\AudioLoader.h"


SceneGameOver::SceneGameOver():
    GoTitle(),Retry(),back()
{
    AudioLoader::Load(AUDIO::BGM_GAMEOVER, gameOverSe);
    gameOverSe->Play(true);
}

SceneGameOver::~SceneGameOver()
{
    gameOverSe->Stop();
}

void SceneGameOver::Initialize()
{
    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();


    Retry.SetHitBox(HitBox2D::CreateBoxFromCenter({ ScreenSize.x * 0.2f,ScreenSize.y * 0.5f }, { ScreenSize.x * 0.3f,ScreenSize.y * 0.1f }));
    Retry.AddComponent(new RenderComponent("./Data/Sprite/SceneBack/retry_moji.png"));

    GoTitle.SetHitBox(HitBox2D::CreateBoxFromCenter({ ScreenSize.x * 0.2f,ScreenSize.y * 0.7f }, { ScreenSize.x * 0.3f,ScreenSize.y * 0.1f }));
    GoTitle.AddComponent(new RenderComponent("./Data/Sprite/SceneBack/titlehe_moji.png"));


    back = std::make_unique<Sprite>("./Data/Sprite/SceneBack/GameOver_back.png");
}

void SceneGameOver::Finalize()
{
}

void SceneGameOver::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && GoTitle.GetHitBox().Hit(mouse.GetPosition()))
        SceneManager::Instance().ChangeScene(new SceneTitle);

    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && Retry.GetHitBox().Hit(mouse.GetPosition()))
        SceneManager::Instance().ChangeScene(new SceneGame);
}

void SceneGameOver::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    const DirectX::XMFLOAT2 ScreenSize = graphics.GetScreenSize();

    back->Render(dc, { .0f,.0f }, ScreenSize, .0f, { 1.0f,1.0f,1.0f,1.0f });

    GoTitle.Render<RenderComponent>(dc);
    Retry.Render<RenderComponent>(dc);
}
