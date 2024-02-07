
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "GameSystemManager.h"

#ifdef _DEBUG
#include "SceneClear.h"
#include "SceneGameOver.h"
#endif // _DEBUG

GameSystemManager::GameSystemManager()
{
    Graphics& graphics = Graphics::Instance();
    const DirectX::XMFLOAT2 ScreenSize = graphics.GetScreenSize();
    const float HparW = ScreenSize.y / ScreenSize.x;

    DirectX::XMFLOAT2 size = { ScreenSize.x * 0.1f * HparW,ScreenSize.y * 0.1f };
    DirectX::XMFLOAT2 pos = { ScreenSize.x - size.x,ScreenSize.y * 0.0f };
    pouseButton.SetHitBox(HitBox2D(pos, size));
    pouseButton.AddComponent(new RenderComponent("./Data/Sprite/pouse_icon.png"));

    pos.y += size.y;
    CardAllInfoButton.SetHitBox(HitBox2D(pos, size));
    CardAllInfoButton.AddComponent(new RenderComponent("./Data/Sprite/card_icon.png"));

    pos = { ScreenSize.x * 0.55f,ScreenSize.y * 0.4f };
    size = { ScreenSize.x * 0.75f,ScreenSize.y * 0.8f };
    auto allInfo = CardAllInfoButton.AddChild(std::make_unique<UI>("allInfo"));
    allInfo->SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    allInfo->AddComponent(new RenderComponent("./Data/Sprite/CardCombos/_CardAllInfo.png"));


    //ポーズ画面のボタン設定
    float offset = ScreenSize.x * 0.01f;
    pos = { ScreenSize.x * 0.4f,ScreenSize.y * 0.43f };
    size = { ScreenSize.x * 0.25f,ScreenSize.y * 0.1f };
    GoTitleButton.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    GoTitleButton.AddComponent(new RenderComponent("./Data/Sprite/pouse/title.png"));

    pos.y += size.y+offset;
    ExitPouseButton.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    ExitPouseButton.AddComponent(new RenderComponent("./Data/Sprite/pouse/return.png"));

    //チュートリアルボタン
    pos.y += size.y + offset;
    tutorialButton.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, size));
    tutorialButton.AddComponent(new RenderComponent("./Data/Sprite/pouse/info.png"));

    //ポーズ画面背景
    pos = { ScreenSize.x * 0.5f,ScreenSize.y * 0.5f };
    pouseBackGround.SetHitBox(HitBox2D::CreateBoxFromCenter(pos, ScreenSize));
    pouseBackGround.AddComponent(new RenderComponent("./Data/Sprite/pouse/back.png"));
    pouseBackGround.GetComponent<RenderComponent>()->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    tutorialSprite.push_back(std::make_unique<Sprite>("./Data/Sprite/tutorial1.png"));
    tutorialSprite.push_back(std::make_unique<Sprite>("./Data/Sprite/tutorial2.png"));
    tutorialSprite.push_back(std::make_unique<Sprite>("./Data/Sprite/tutorial3.png"));
}

void GameSystemManager::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    GamePad& gamepad = Input::Instance().GetGamePad();

    if (isTutorial)
    {
        TutorialOnlyUpdate(elapsedTime);
        return;
    }

    if (isPoused)
    {
        PousedOnlyUpdate(elapsedTime);
    }

    if (gamepad.GetButtonDown()&GamePad::BTN_ESCAPE||(mouse.GetButtonDown()&Mouse::BTN_LEFT&& pouseButton.GetHitBox().Hit(mouse.GetPosition())))
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
    else if (isTutorial)
    {
        TutorialOnlyRender(dc);
    }


    if(auto render = pouseButton.GetComponent<RenderComponent>())render->Render(dc);


    if (!(isPoused|isTutorial)&& CardAllInfoButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        CardAllInfoButton.SearchChildFromName("allInfo")->Render<RenderComponent>(dc);
    }
}

void GameSystemManager::PousedOnlyUpdate(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (isTutorial)
    {
        TutorialOnlyUpdate(elapsedTime);
        return;
    }

    //タイトルへボタン
    if (!SceneManager::Instance().IsWaitSceneChange()&& mouse.GetButtonDown()&Mouse::BTN_LEFT&& GoTitleButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        SceneManager::Instance().ChangeScene(new SceneTitle);
    }

    //ポーズ戻るボタン
    if (mouse.GetButtonDown()&Mouse::BTN_LEFT&& ExitPouseButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        this->isPoused = false;
    }
    //チュートリアルボタン
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && tutorialButton.GetHitBox().Hit(mouse.GetPosition()))
    {
        isTutorial = true;
        isPoused = true;
    }

    //debug
#ifdef _DEBUG
    if (!SceneManager::Instance().IsWaitSceneChange() && mouse.GetPositionX() < .0f)
    {
        SceneManager::Instance().SetSceneChangeSprite(std::make_unique<Sprite>("./Data/Sprite/SceneBack/clear_Back.png"));
        SceneManager::Instance().ChangeScene(new SceneClear,true);
    }
    if (!SceneManager::Instance().IsWaitSceneChange()&& mouse.GetPositionY() < .0f)
        SceneManager::Instance().ChangeScene(new SceneGameOver);
#endif // _DEBUG
}

void GameSystemManager::PousedOnlyRender(ID3D11DeviceContext* dc)
{
    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
    if (auto render = pouseBackGround.GetComponent<RenderComponent>())render->Render(dc);

    if (auto render = GoTitleButton.GetComponent<RenderComponent>())render->Render(dc);
    if(auto render = ExitPouseButton.GetComponent<RenderComponent>())render->Render(dc);
    if (auto render = tutorialButton.GetComponent<RenderComponent>())render->Render(dc);

    if (isTutorial)
        TutorialOnlyRender(dc);
}

void GameSystemManager::TutorialOnlyUpdate(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        if (++tutrialIndex >= std::size(tutorialSprite))
        {
            tutrialIndex = 0;
            isTutorial = false;
            isPoused = false;
        }
    }
    mouse.ClearButtonDown();
}

void GameSystemManager::TutorialOnlyRender(ID3D11DeviceContext* dc)
{
    const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
    const DirectX::XMFLOAT2 size = {ScreenSize.x,ScreenSize.y};
    HitBox2D box = HitBox2D::CreateBoxFromCenter({ ScreenSize.x * 0.5f,ScreenSize.y * 0.5f }, { ScreenSize.x,ScreenSize.y });
    tutorialSprite.at(tutrialIndex)->Render(dc, box.GetLeftTop(), box.GetBoxSize(), .0f, { 1.0f,1.0f,1.0f,1.0f });
}


