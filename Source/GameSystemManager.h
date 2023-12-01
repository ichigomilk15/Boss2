#pragma once

#include <d3d11.h>

#include "UI.h"


class GameSystemManager final
{
public://constractor
	static GameSystemManager& Instance()noexcept { static GameSystemManager instance; return instance; }
private://constractor
	GameSystemManager();
	~GameSystemManager() {};

public://functions

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);

	//Getter&Setter************************************************************************************
#if 1
	const bool& GetIsPoused()const noexcept { return isPoused; }
	void SetPoused(const bool& flag)noexcept { isPoused = flag; }
#endif // 1
	//Getter&Setter************************************************************************************

private://functions
	void PousedOnlyUpdate(float elapsedTime);
	void PousedOnlyRender(ID3D11DeviceContext* dc);
private:

	bool isPoused = false;
	UI pouseButton;
	UI GoTitleButton;
	UI ExitGameButton;
	UI CardAllInfoButton;
	std::unique_ptr<Sprite> pouseBackGround;
};