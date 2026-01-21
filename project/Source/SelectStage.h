#pragma once
#include "../Library/SceneBase.h"
#include <vector>
#include <tchar.h>

class SelectStage final : public SceneBase
{
public:
    SelectStage();
    ~SelectStage() override;

    void Update() override;
    void Draw() override;

private:
    struct Option
    {
        const TCHAR* title = nullptr;
        const TCHAR* sub1 = nullptr;
        const TCHAR* sub2 = nullptr;

        int stageId = 0;
        int lives = 0;
        int hotKey = 0;

        int boxR = 200;
        int boxG = 60;
        int boxB = 40;
    };

private:
    void StartDecide(int index);

    void DrawOutlinedText(int x, int y, const TCHAR* text, int colorText, int colorOutline, int font) const;
    void DrawCenteredOutlinedText(int centerX, int y, const TCHAR* text, int colorText, int colorOutline, int font) const;

private:
    std::vector<Option> options_;
    int selectedIndex_ = 0;

    int bgmHandle_ = -1;
    int selectSE_ = -1;
    int decideSE_ = -1;

    bool deciding_ = false;
    int decideTimer_ = 0;
    int decideStageId_ = 0;

    float cursorBlinkFrame_ = 0.0f;
    float subTextAnimFrame_ = 0.0f;

    int fontTitle_ = -1;
    int fontCardTitle_ = -1;
    int fontCardSub_ = -1;
    int fontHint_ = -1;
};
