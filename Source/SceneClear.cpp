#include "Graphics/Graphics.h"
#include "Input/Input.h"

#include "SceneManager.h"
#include "SceneClear.h"
#include "SceneTitle.h"

SceneClear::SceneClear() :
    back()
{

}

SceneClear::~SceneClear()
{
}

void SceneClear::Initialize()
{
    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
    //GoTitle.SetHitBox(HitBox2D::CreateBoxFromCenter({ ScreenSize.x * 0.9f,ScreenSize.y * 0.9f }, { ScreenSize.x * 0.3f,ScreenSize.y * 0.1f }));
    //GoTitle.AddComponent(new RenderComponent("./Data/Sprite/SceneBack/titlehe_moji.png"));

    back = std::make_unique<Sprite>("./Data/Sprite/SceneBack/clear_Back.png");
}

void SceneClear::Finalize()
{
}

void SceneClear::Update(float elapseTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown()&Mouse::BTN_LEFT/*&&GoTitle.GetHitBox().Hit(mouse.GetPosition())*/)
    {
        SceneManager::Instance().ChangeScene(new SceneTitle);
        return;
    }
}

void SceneClear::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();


    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
    back->Render(dc, { .0f,.0f }, ScreenSize, .0f, { 1.0f,1.0f,1.0f,1.0f });

    //GoTitle.Render<RenderComponent>(dc);
}
