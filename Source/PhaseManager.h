#pragma once


class FhaseManager
{
public://class
	enum Fhase
	{

		Max,
	};
private://constructer
	FhaseManager();
	~FhaseManager();
public:
	static FhaseManager& Instance()noexcept { static FhaseManager instance; return instance; }

	void Initialize();
	void Update(float elapsedTime);
	void Reset();

//Getter&Setter*****************************************************************************
#if 1
	const Fhase& GetFhase()const noexcept { return fhase; }
#endif // 1
//Getter&Setter*****************************************************************************
private:
	Fhase fhase;
};