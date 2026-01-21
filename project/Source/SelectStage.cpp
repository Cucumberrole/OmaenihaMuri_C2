#include "SelectStage.h"

#include <DxLib.h>
#include <tchar.h>
#include <algorithm>

#include "PlayScene.h"
#include "../Library/SceneManager.h"

// Assets (adjust if your folders differ)
static const TCHAR* kBgmPath      = TEXT("data/bgm/002.ogg");
static const TCHAR* kCursorSePath = TEXT("data/bgm/cursor.mp3");
static const TCHAR* kDecideSePath = TEXT("data/bgm/decision.mp3");

static const TCHAR* kTitleText = TEXT("STAGE SELECT");
static const TCHAR* kHintText  = TEXT("UP/DOWN or W/S : Select   Enter/Space : OK   T : Title   ESC : Exit");
static const TCHAR* kHintText2 = TEXT("E : Stage1   H : Stage2");

static const TCHAR* kStage1Title = TEXT("STAGE 1");
static const TCHAR* kStage2Title = TEXT("STAGE 2");

static const TCHAR* kStage1Sub = TEXT("For beginners");
static const TCHAR* kStage2Sub = TEXT("For challengers");

static inline bool IsTriggerKey(int key)
{
    return DxLib::CheckHitKey(key) != 0;
}

SelectStage::SelectStage()
{
    DxLib::GetDrawScreenSize(&sw_, &sh_);

    // Fonts (bigger layout)
    fontTitle_     = DxLib::CreateFontToHandle(TEXT("Arial Black"), 84, 3, DX_FONTTYPE_ANTIALIASING_8X8);
    fontCardTitle_ = DxLib::CreateFontToHandle(TEXT("Arial Black"), 46, 2, DX_FONTTYPE_ANTIALIASING_8X8);
    fontCardSub_   = DxLib::CreateFontToHandle(TEXT("Arial"), 32, 2, DX_FONTTYPE_ANTIALIASING_8X8);
    fontHint_      = DxLib::CreateFontToHandle(TEXT("Arial"), 28, 1, DX_FONTTYPE_ANTIALIASING_8X8);

    if (fontTitle_ < 0)     fontTitle_     = DxLib::CreateFontToHandle(TEXT("Arial"), 84, 3, DX_FONTTYPE_ANTIALIASING_8X8);
    if (fontCardTitle_ < 0) fontCardTitle_ = DxLib::CreateFontToHandle(TEXT("Arial"), 46, 2, DX_FONTTYPE_ANTIALIASING_8X8);
    if (fontCardSub_ < 0)   fontCardSub_   = DxLib::CreateFontToHandle(TEXT("Arial"), 32, 2, DX_FONTTYPE_ANTIALIASING_8X8);
    if (fontHint_ < 0)      fontHint_      = DxLib::CreateFontToHandle(TEXT("Arial"), 28, 1, DX_FONTTYPE_ANTIALIASING_8X8);

    // Sounds are optional: if missing, handle stays -1
    bgmHandle_ = DxLib::LoadSoundMem(kBgmPath);
    cursorSe_  = DxLib::LoadSoundMem(kCursorSePath);
    decideSe_  = DxLib::LoadSoundMem(kDecideSePath);

    if (bgmHandle_ >= 0)
    {
        DxLib::ChangeVolumeSoundMem(170, bgmHandle_);
        DxLib::PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP, TRUE);
    }
}

SelectStage::~SelectStage()
{
    if (bgmHandle_ >= 0) { DxLib::StopSoundMem(bgmHandle_); DxLib::DeleteSoundMem(bgmHandle_); bgmHandle_ = -1; }
    if (cursorSe_ >= 0)  { DxLib::DeleteSoundMem(cursorSe_); cursorSe_ = -1; }
    if (decideSe_ >= 0)  { DxLib::DeleteSoundMem(decideSe_); decideSe_ = -1; }

    if (fontTitle_ >= 0)     { DxLib::DeleteFontToHandle(fontTitle_); fontTitle_ = -1; }
    if (fontCardTitle_ >= 0) { DxLib::DeleteFontToHandle(fontCardTitle_); fontCardTitle_ = -1; }
    if (fontCardSub_ >= 0)   { DxLib::DeleteFontToHandle(fontCardSub_); fontCardSub_ = -1; }
    if (fontHint_ >= 0)      { DxLib::DeleteFontToHandle(fontHint_); fontHint_ = -1; }
}

void SelectStage::DecideStage(int stageId)
{
    if (decideSe_ >= 0) DxLib::PlaySoundMem(decideSe_, DX_PLAYTYPE_BACK, TRUE);

    // Your project already used this pattern before.
    PlayScene::SelectedStage = stageId;
    SceneManager::ChangeScene("PLAY");
}

void SelectStage::Update()
{
    ++frame_;

    // Exit
    if (IsTriggerKey(KEY_INPUT_ESCAPE))
    {
        DxLib::DxLib_End();
        return;
    }

    // Title
    if (IsTriggerKey(KEY_INPUT_T))
    {
        SceneManager::ChangeScene("TITLE");
        return;
    }

    // Direct select
    if (IsTriggerKey(KEY_INPUT_E))
    {
        DecideStage(1);
        return;
    }
    if (IsTriggerKey(KEY_INPUT_H))
    {
        DecideStage(2);
        return;
    }

    // Selection move (Up/Down or W/S)
    const bool up   = IsTriggerKey(KEY_INPUT_UP) || IsTriggerKey(KEY_INPUT_W);
    const bool down = IsTriggerKey(KEY_INPUT_DOWN) || IsTriggerKey(KEY_INPUT_S);
    if (up)
    {
        selectedIndex_ = 0;
        if (cursorSe_ >= 0) DxLib::PlaySoundMem(cursorSe_, DX_PLAYTYPE_BACK, TRUE);
    }
    else if (down)
    {
        selectedIndex_ = 1;
        if (cursorSe_ >= 0) DxLib::PlaySoundMem(cursorSe_, DX_PLAYTYPE_BACK, TRUE);
    }

    // Decide
    if (IsTriggerKey(KEY_INPUT_RETURN) || IsTriggerKey(KEY_INPUT_SPACE))
    {
        DecideStage(selectedIndex_ + 1);
        return;
    }
}

void SelectStage::Draw()
{
    DxLib::GetDrawScreenSize(&sw_, &sh_);

    // Background
    DxLib::DrawBox(0, 0, sw_, sh_, DxLib::GetColor(30, 18, 40), TRUE);
    DxLib::DrawBox(0, 0, sw_, sh_ / 3, DxLib::GetColor(60, 30, 70), TRUE);

    // Title
    const int titleW = DxLib::GetDrawStringWidthToHandle(kTitleText, (int)_tcslen(kTitleText), fontTitle_);
    DxLib::DrawStringToHandle((sw_ - titleW) / 2, (int)(sh_ * 0.08f), kTitleText, DxLib::GetColor(255, 240, 200), fontTitle_);

    // Card layout (bigger)
    const int cardW = (int)(sw_ * 0.72f);
    const int cardH = 170;
    const int gapY  = 34;

    const int groupH = cardH * 2 + gapY;
    const int groupY = (sh_ - groupH) / 2 + 20;
    const int cardX  = (sw_ - cardW) / 2;

    auto drawCard = [&](int index, int x, int y, const TCHAR* title, const TCHAR* sub)
    {
        const bool sel = (selectedIndex_ == index);
        const int outer = sel ? DxLib::GetColor(255, 210, 120) : DxLib::GetColor(120, 120, 140);
        const int inner = sel ? DxLib::GetColor(80, 40, 120)   : DxLib::GetColor(50, 35, 65);
        const int edge  = DxLib::GetColor(10, 10, 18);

        // frame
        DxLib::DrawBox(x, y, x + cardW, y + cardH, edge, TRUE);
        DxLib::DrawBox(x + 6, y + 6, x + cardW - 6, y + cardH - 6, outer, TRUE);
        DxLib::DrawBox(x + 16, y + 16, x + cardW - 16, y + cardH - 16, inner, TRUE);

        // text
        const int tx = x + 44;
        const int ty = y + 34;

        DxLib::DrawStringToHandle(tx, ty, title, DxLib::GetColor(255, 255, 255), fontCardTitle_);
        DxLib::DrawStringToHandle(tx, ty + 64, sub, DxLib::GetColor(230, 220, 255), fontCardSub_);

        // cursor triangle
        if (sel)
        {
            const int blink = (frame_ / 10) % 2;
            if (blink == 0)
            {
                const int cx = x - 34;
                const int cy = y + cardH / 2;
                DxLib::DrawTriangle(cx, cy, cx + 26, cy - 18, cx + 26, cy + 18, DxLib::GetColor(255, 240, 180), TRUE);
            }
        }
    };

    drawCard(0, cardX, groupY + 0 * (cardH + gapY), kStage1Title, kStage1Sub);
    drawCard(1, cardX, groupY + 1 * (cardH + gapY), kStage2Title, kStage2Sub);

    // Hints
    const int hintY = (int)(sh_ * 0.88f);
    DxLib::DrawStringToHandle((int)(sw_ * 0.08f), hintY, kHintText, DxLib::GetColor(255, 240, 200), fontHint_);
    DxLib::DrawStringToHandle((int)(sw_ * 0.08f), hintY + 34, kHintText2, DxLib::GetColor(255, 240, 200), fontHint_);
}
