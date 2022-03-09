#pragma once



class Camera
{
public:
	Camera();
	~Camera();
	glm::vec3 GetCameraPos()const;
	void SetCameraPos(glm::vec3 Pos);


	glm::vec3 GetRight()const;
	glm::vec3 GetUp()const;
	glm::vec3 GetTarget()const;

	void Init();

	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;


	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;
	
	void LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 worldUp);


	glm::mat4x4 GetView()const;
	glm::mat4x4 GetProj()const;

	void Walk(glm::vec3 Trans);

	void Pitch(float angle);
	void Yaw(float angle);


	void UpdateProjMatrix(float fovY, float aspect, float zn, float zf);
	void UpdateViewMatrix();

private:

	glm::vec3 mPosition = { 0.0f,-1500.0f,1500.0f };
	glm::vec3 mUp = { 0.0f,1.0f,0.0f };
	glm::vec3 mTarget = { 0.0f,500.0f,0.0f }; 
	glm::vec3 mRight = { 1.0f,0.0f,0.0f };

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspectRatio = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowsHeight = 0.0f;
	float mFarWindowsHeight = 0.0f;

	bool mInitView = true;

	glm::mat4x4 mView = glm::mat4(1.0f);
	glm::mat4x4 mProj = glm::mat4(1.0f);

};

