#pragma once
#include "../Library/SceneBase.h"

// Stage selection scene (2 stages).
class SelectStage final : public SceneBase
{
public:
    SelectStage();
    ~SelectStage() override;

    void Update() override;
    void Draw() override;

private:
    void DecideStage(int stageId);

private:
    int selectedIndex_ = 0; // 0: stage1, 1: stage2
    int frame_ = 0;

    // Fonts
    int fontTitle_ = -1;
    int fontCardTitle_ = -1;
    int fontCardSub_ = -1;
    int fontHint_ = -1;

    // Sounds (optional; safe if missing)
    int bgmHandle_ = -1;
    int cursorSe_ = -1;
    int decideSe_ = -1;

    // Cached screen size
    mutable int sw_ = 1280;
    mutable int sh_ = 720;
};
