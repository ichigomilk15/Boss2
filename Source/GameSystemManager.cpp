
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneManager.h"
#include "SceneTitle.h"

#include "GameSystemManager.h"

GameSystemManager::GameSystemManager()
{
    Graphics& graphics = Graphics::Instance();
    const DirectX::XMFLOAT2 ScreenSize = graphics.GetScreenSize();

    DirectX::XMFLOAT2 pos = { ScreenSize.x * 0.9f,ScreenSize.y * 0.0f };
    DirectX::XMFLOAT2 size = { ScreenSize.x * 0.1f,ScreenSize.y * 0.1f };
    pouseButton.SetHitBox(HitBox2D(pos, size));
    pouseButton.AddComponent(new RenderComponent(nullptr));

    pos.y += size.y;
    CardAllInfoButton.SetHitBox(HitBox2D(pos, size));
    CardAllInfoButton.AddComponent(new RenderComponent(nullptr));

    pos = { ScreenSize.x*0.5f,ScreenSize.y*0.5f };
    size = { ScreenSize.x * 0.9f,ScreenSize.y * 0.9f };
    CardAllInfoButton.AddChild(std::make_unique<UI>("allInfo"));
    auto allInfo = CardAllInfoButton.SearchChildFromName("allInfo");
    allInfo->SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    allInfo->AddComponent(new RenderComponent("./Data/Sprite/CardCombos/_CardAllInfo.png"));


    pos = { ScreenSize.x * 0.5f,ScreenSize.y * 0.3f };
    size = { ScreenSize.x * 0.5f,ScreenSize.y * 0.2f };
    GoTitleButton.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    GoTitleButton.AddComponent(new RenderComponent(nullptr));

    pos.y = ScreenSize.y * 0.6f;
    ExitGameButton.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    ExitGameButton.AddComponent(new RenderComponent(nullptr));

    

    pouseBackGround = std::make_unique<Sprite>();
}

void GameSystemManager::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    GamePad& gamepad = Input::Instance().GetGamePad();
    if (isPoused)
    {
        PousedOnlyUpdate(elapsedTime);
    }

    if (gamepad.GetButtonDown()&GamePad::BTN_ESCAPE||mouse.GetButtonDown()&Mouse::BTN_LEFT&& pouseButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        isPoused ^= true;
    }
}

void GameSystemManager::Render(ID3D11DeviceContext* dc)
{
    Mouse& mouse = Input::Instance().GetMouse();


    if (auto render = CardAllInfoButton.GetComponent<RenderComponent>())render->Render(dc);
    if (isPoused)
    {
        PousedOnlyRender(dc);
    }

    if(auto render = pouseButton.GetComponent<RenderComponent>())render->Render(dc);


    if (!isPoused&& CardAllInfoButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        CardAllInfoButton.SearchChildFromName("allInfo")->Render<RenderComponent>(dc);
    }
}

void GameSystemManager::PousedOnlyUpdate(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown()&Mouse::BTN_LEFT&& GoTitleButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        SceneManager::Instance().ChangeScene(new SceneTitle);
    }

    if (mouse.GetButtonDown()&Mouse::BTN_LEFT&& ExitGameButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        Graphics::Instance().Quit();
    }
}

void GameSystemManager::PousedOnlyRender(ID3D11DeviceContext* dc)
{
    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
    pouseBackGround->Render(dc,
        .0f, .0f,
        ScreenSize.x, ScreenSize.y,
        .0f, .0f,
        pouseBackGround->GetTextureWidthf(), pouseBackGround->GetTextureHeightf(),
        .0f,
        .0f, .0f, .0f, 0.6f
    );

    if (auto render = GoTitleButton.GetComponent<RenderComponent>())render->Render(dc);
    if(auto render = ExitGameButton.GetComponent<RenderComponent>())render->Render(dc);
}


