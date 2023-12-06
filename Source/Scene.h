#pragma once
#include "Audio\AudioSource.h"

//シーン
class Scene
{
public:
	Scene() {}
	virtual ~Scene(){}

	//初期化
	virtual void Initialize() = 0;

	//終了化
	virtual void Finalize() = 0;

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render() = 0;

	//準備完了しているか
	bool IsReady() const { return ready; }

	//準備完了設定
	void SetReady() { ready = true; }

private:
	bool ready = false;

protected:
	/*std::unique_ptr<AudioSource> gameSe;
	std::unique_ptr<AudioSource> gameOverSe;
	std::unique_ptr<AudioSource> gameClearSe;*/
};