#include "Common.h"
#include "Fader.h"                 // 使っていなければ消してもOK
#include "PlayScene.h"
#include "../Library/SceneManager.h"
#include "../ImGui/imgui.h"

Common* Common::s_instance = nullptr;

Common::Common()
{
	DontDestroyOnSceneChange(); // シーン切り替えで消えない共通オブジェクト
	s_instance = this;

	stageNumber = 1;
	score = 0;
	hiScore = 0;
	invincible = false;
}

Common* Common::GetInstance()
{
	return s_instance;
}

void Common::Update()
{
	// 共通で毎フレームやりたい処理があればここに書く
}

void Common::Draw()
{
    ImGui::Begin("Common");

    ImGui::InputInt("StageNumber", &stageNumber);

    if (ImGui::Button("JumpToStage"))
    {
        if (stageNumber < 1) stageNumber = 1;

        PlayScene::SelectedStage = stageNumber;
        SceneManager::ChangeScene("PlayScene");
    }

    ImGui::Checkbox("Invincible", &invincible);

    ImGui::End();
}
