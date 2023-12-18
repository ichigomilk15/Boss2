#pragma once

#include <d3d11.h>
#include <memory>
#include <vector>
#include <concepts>
#include <string>

#include "Graphics/Sprite.h"

#include "HitCollisions.h"

class Component
{
	friend class UI;
protected:
	Component() {};
public:
	virtual ~Component() {};


protected:
	class UI* parent = nullptr;
};
template<class T>
concept is_base_of_component = requires {std::is_base_of_v<Component, T>; };
template<class T>
concept is_have_Render = requires(T t,ID3D11DeviceContext* dc) {is_base_of_component<T>; t.Render(dc); };

class Behavior
{
	friend class UI;
protected:
	Behavior() = delete;
public:
	virtual ~Behavior() = default;

	virtual void Update(float elapsedTime) {};
private:
	class UI* parent = nullptr;
};
template<class T>
concept is_base_of_Behavior = requires(T) { std::is_base_of_v<T, Behavior>; };

//‰¼UI
class UI
{
public://constractors
	UI() =default;
	UI(const char* name);
	UI(const std::string& name);
	~UI();

public://functions
	template<is_base_of_component T>
	T* GetComponent()
	{
		for (auto& component : components)
		{
			if (typeid(*component) == typeid(T))return dynamic_cast<T*>(component);
		}

		return nullptr;
	}

	template<is_base_of_Behavior T>
	T* GetBehavior()
	{
		for (auto& behavior : behaviors)
		{
			if (typeid(*behavior) == typeid(T))return dynamic_cast<T*>(behavior);
		}
		return nullptr;
	}

	template<is_base_of_component T>
	void AddComponent(T* component)
	{
		component->parent = this;
		components.emplace_back(component);
	}

	template<is_base_of_Behavior T>
	void AddBehavior(T* behavior)
	{
		behavior->parent = this;
		behaviors.emplace_back(behavior);
	}

	UI* AddChild(std::unique_ptr<UI> child);
	UI* SearchChildFromName(const std::string& name)noexcept;

	void ChildsRender(ID3D11DeviceContext* dc);

	void Update(float elapsedTime);

	template<is_have_Render T>
	void Render(ID3D11DeviceContext* dc)
	{
		if (auto renderer = GetComponent<T>())
			renderer->Render(dc);
	}

	//Getter&Setter**********************************************************************************
#if 1
	const HitBox2D& GetHitBox()const noexcept { return collision; }
	void SetHitBox(const HitBox2D& box)noexcept { collision = box; }
	const std::string& GetName()const noexcept { return name; }
#endif // 1
	//Getter&Setter**********************************************************************************

private://functions

private://members
	std::vector<Component*>	components;
	std::vector<Behavior*>	behaviors;
	std::vector<std::unique_ptr<UI>> childs;
	HitBox2D collision;
	std::string name;//ui‚ðŽ¯•Ê‚·‚é—p‚Ì–¼‘O
	bool deleteflag = false;
};

//**********************************************************************************************
// Components
//**********************************************************************************************

class RenderComponent : public Component
{
public://constractor
	RenderComponent(const char* filename);
	~RenderComponent() {};

public://functions
	void Render(ID3D11DeviceContext* dc);

	const DirectX::XMFLOAT4& GetColor()const noexcept { return color; }
	void SetColor(const DirectX::XMFLOAT4& set) { color = set; }
private:
	std::unique_ptr<Sprite> sprite;
	DirectX::XMFLOAT4 color;
};


//**********************************************************************************************
// Behaviors
//**********************************************************************************************

class WaveChange : public Behavior
{
public:
	WaveChange();
	~WaveChange();
	void Update(float elapsedTime)override;
};