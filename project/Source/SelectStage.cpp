#include "SelectStage.h"
#include "PlayScene.h"
#include "../Library/SceneManager.h"
#include <DxLib.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <tchar.h>

static const TCHAR* kBgmPath      = TEXT("data/bgm/002.ogg");
static const TCHAR* kCursorSePath = TEXT("data/bgm/cursor.mp3");
static const TCHAR* kDecideSePath = TEXT("data/bgm/decision.mp3");

#if defined(UNICODE) || defined(_UNICODE)
static const TCHAR* kTitleText    = L"\u30B9\u30C6\u30FC\u30B8\u30BB\u30EC\u30AF\u30C8";
static const TCHAR* kHint1        = L"\u2191\u2193 / W,S : \u9078\u629E    Enter/Space : \u6C7A\u5B9A    E/H : \u76F4\u9078\u629E";
static const TCHAR* kHint2        = L"T : \u30BF\u30A4\u30C8\u30EB\u3078    ESC : \u7D42\u4E86";
static const TCHAR* kDecidingText = L"\u8AAD\u307F\u8FBC\u307F\u4E2D...";
static const TCHAR* kEasyTitle    = L"\u7C21\u5358\u30B9\u30C6\u30FC\u30B8";
static const TCHAR* kEasySub1     = L"\u521D\u3081\u3066\u306E\u4EBA\u306F\u3053\u3061\u3089";
static const TCHAR* kHardTitle    = L"\u96E3\u3057\u3044\u30B9\u30C6\u30FC\u30B8";
static const TCHAR* kHardSub1     = L"\u305F\u304F\u3055\u3093\u6B7B\u306B\u305F\u3044\u4EBA\u306F\u3053\u3061\u3089";
#else
static const TCHAR* kTitleText    = TEXT("STAGE SELECT");
static const TCHAR* kHint1        = TEXT("UP/DOWN or W/S : Move   Enter/Space : Decide   E/H : Quick Select");
static const TCHAR* kHint2        = TEXT("T : Title   ESC : Exit");
static const TCHAR* kDecidingText = TEXT("Loading...");
static const TCHAR* kEasyTitle    = TEXT("EASY STAGE");
static const TCHAR* kEasySub1     = TEXT("For beginners");
static const TCHAR* kHardTitle    = TEXT("HARD STAGE");
static const TCHAR* kHardSub1     = TEXT("For experts");
#endif

SelectStage::SelectStage()
{
    // audio
    bgmHandle_ = LoadSoundMem(kBgmPath);
    if (bgmHandle_ >= 0)
    {
        ChangeVolumeSoundMem(160, bgmHandle_);
        PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP, TRUE);
    }

    selectSE_ = LoadSoundMem(kCursorSePath);
    decideSE_ = LoadSoundMem(kDecideSePath);

    // fonts
    fontTitle_ = CreateFontToHandle(TEXT("Arial Black"), 56, 3, DX_FONTTYPE_ANTIALIASING_8X8);
    if (fontTitle_ < 0) fontTitle_ = CreateFontToHandle(TEXT("Arial"), 56, 3, DX_FONTTYPE_ANTIALIASING_8X8);

    fontCardTitle_ = CreateFontToHandle(TEXT("Arial Black"), 34, 2, DX_FONTTYPE_ANTIALIASING_8X8);
    if (fontCardTitle_ < 0) fontCardTitle_ = CreateFontToHandle(TEXT("Arial"), 34, 2, DX_FONTTYPE_ANTIALIASING_8X8);

    fontCardSub_ = CreateFontToHandle(TEXT("Arial"), 24, 1, DX_FONTTYPE_ANTIALIASING_8X8);
    fontHint_    = CreateFontToHandle(TEXT("Arial"), 20, 1, DX_FONTTYPE_ANTIALIASING_8X8);

    // options
    options_.clear();
    options_.reserve(4);

    Option easy{};
    easy.title   = kEasyTitle;
    easy.sub1    = kEasySub1;
    easy.sub2    = TEXT("");
    easy.lives   = 5;
    easy.stageId = 1;
    easy.hotKey  = KEY_INPUT_E;
    easy.boxR = 240; easy.boxG = 170; easy.boxB = 60;
    options_.push_back(easy);

    Option hard{};
    hard.title   = kHardTitle;
    hard.sub1    = kHardSub1;
    hard.sub2    = TEXT("");
    hard.lives   = 3;
    hard.stageId = 2;
    hard.hotKey  = KEY_INPUT_H;
    hard.boxR = 220; hard.boxG = 60; hard.boxB = 60;
    options_.push_back(hard);

    selectedIndex_ = 0;
}

SelectStage::~SelectStage()
{
    if (selectSE_ >= 0) DeleteSoundMem(selectSE_);
    if (decideSE_ >= 0) DeleteSoundMem(decideSE_);
    if (bgmHandle_ >= 0) DeleteSoundMem(bgmHandle_);

    if (fontTitle_ >= 0) DeleteFontToHandle(fontTitle_);
    if (fontCardTitle_ >= 0) DeleteFontToHandle(fontCardTitle_);
    if (fontCardSub_ >= 0) DeleteFontToHandle(fontCardSub_);
    if (fontHint_ >= 0) DeleteFontToHandle(fontHint_);
}

void SelectStage::StartDecide(int index)
{
    if (deciding_) return;
    if (index < 0 || index >= (int)options_.size()) return;

    deciding_ = true;
    decideTimer_ = 0;
    decideStageId_ = options_[index].stageId;

    if (decideSE_ >= 0) PlaySoundMem(decideSE_, DX_PLAYTYPE_BACK, TRUE);
}

void SelectStage::Update()
{
    // back to title
    if (CheckHitKey(KEY_INPUT_T))
    {
        SceneManager::ChangeScene("TITLE");
        return;
    }

    // exit
    if (CheckHitKey(KEY_INPUT_ESCAPE))
    {
        SceneManager::Exit();
        return;
    }

    if (deciding_)
    {
        ++decideTimer_;
        if (decideTimer_ >= 30)
        {
            PlayScene::SelectedStage = decideStageId_;
            SceneManager::ChangeScene("PLAY");
        }
        return;
    }

    // quick select
    for (int i = 0; i < (int)options_.size(); ++i)
    {
        if (options_[i].hotKey != 0 && CheckHitKey(options_[i].hotKey))
        {
            if (selectedIndex_ != i && selectSE_ >= 0) PlaySoundMem(selectSE_, DX_PLAYTYPE_BACK, TRUE);
            selectedIndex_ = i;
            StartDecide(i);
            break;
        }
    }

    // navigation
    const bool up   = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
    const bool down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);

    if (up)
    {
        const int prev = selectedIndex_;
        selectedIndex_ = max(0, selectedIndex_ - 1);
        if (prev != selectedIndex_ && selectSE_ >= 0) PlaySoundMem(selectSE_, DX_PLAYTYPE_BACK, TRUE);
    }
    else if (down)
    {
        const int prev = selectedIndex_;
        selectedIndex_ = min((int)options_.size() - 1, selectedIndex_ + 1);
        if (prev != selectedIndex_ && selectSE_ >= 0) PlaySoundMem(selectSE_, DX_PLAYTYPE_BACK, TRUE);
    }

    // decide
    if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
    {
        StartDecide(selectedIndex_);
    }

    cursorBlinkFrame_ += 0.13f;
    if (cursorBlinkFrame_ > DX_PI_F * 2.0f) cursorBlinkFrame_ -= DX_PI_F * 2.0f;

    subTextAnimFrame_ += 0.08f;
    if (subTextAnimFrame_ > DX_PI_F * 2.0f) subTextAnimFrame_ -= DX_PI_F * 2.0f;
}

void SelectStage::DrawOutlinedText(int x, int y, const TCHAR* text, int colorText, int colorOutline, int font) const
{
    DrawStringToHandle(x - 2, y, text, colorOutline, font);
    DrawStringToHandle(x + 2, y, text, colorOutline, font);
    DrawStringToHandle(x, y - 2, text, colorOutline, font);
    DrawStringToHandle(x, y + 2, text, colorOutline, font);
    DrawStringToHandle(x - 2, y - 2, text, colorOutline, font);
    DrawStringToHandle(x + 2, y - 2, text, colorOutline, font);
    DrawStringToHandle(x - 2, y + 2, text, colorOutline, font);
    DrawStringToHandle(x + 2, y + 2, text, colorOutline, font);
    DrawStringToHandle(x, y, text, colorText, font);
}

void SelectStage::DrawCenteredOutlinedText(int centerX, int y, const TCHAR* text, int colorText, int colorOutline, int font) const
{
    const int len = (int)_tcslen(text);
    const int tw  = GetDrawStringWidthToHandle(text, len, font);
    DrawOutlinedText(centerX - tw / 2, y, text, colorText, colorOutline, font);
}

void SelectStage::Draw()
{
    int screenW = 0, screenH = 0;
    GetDrawScreenSize(&screenW, &screenH);

    // background
    DrawBox(0, 0, screenW, screenH, GetColor(20, 18, 30), TRUE);

    // subtle grid
    for (int x = 0; x < screenW; x += 64)
        DrawLine(x, 0, x, screenH, GetColor(40, 35, 55));
    for (int y = 0; y < screenH; y += 64)
        DrawLine(0, y, screenW, y, GetColor(40, 35, 55));

    // title
    DrawCenteredOutlinedText(screenW / 2, (int)(screenH * 0.10f), kTitleText, GetColor(255, 235, 170), GetColor(0, 0, 0), fontTitle_);

    // layout block centered
    const int cardW = (int)(screenW * 0.62f);
    const int cardH = (int)(screenH * 0.16f);
    const int gapY  = (int)(screenH * 0.04f);

    const int blockH = (int)options_.size() * cardH + ((int)options_.size() - 1) * gapY;
    const int blockY = (int)(screenH * 0.28f);
    const int startY = blockY;
    const int startX = (screenW - cardW) / 2;

    const int border = 3;
    const float blink = (std::sin(cursorBlinkFrame_) * 0.5f + 0.5f); // 0..1

    for (int i = 0; i < (int)options_.size(); ++i)
    {
        const int x = startX;
        const int y = startY + i * (cardH + gapY);

        const bool isCursor = (i == selectedIndex_) && !deciding_;
        const int glow = isCursor ? (int)(120 + 90 * blink) : 0;

        const int baseR = options_[i].boxR;
        const int baseG = options_[i].boxG;
        const int baseB = options_[i].boxB;

        const int outer = GetColor(255, 220, 140);
        const int edge  = GetColor(40, 20, 10);

        const int inner1 = GetColor(min(255, baseR + 40), min(255, baseG + 40), min(255, baseB + 40));
        const int inner2 = GetColor(baseR, baseG, baseB);

        if (isCursor)
        {
            SetDrawBlendMode(DX_BLENDMODE_ADD, glow);
            DrawBox(x - 12, y - 12, x + cardW + 12, y + cardH + 12, GetColor(255, 200, 80), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        DrawBox(x, y, x + cardW, y + cardH, edge, TRUE);
        DrawBox(x + 4, y + 4, x + cardW - 4, y + cardH - 4, outer, TRUE);
        DrawBox(x + 10, y + 10, x + cardW - 10, y + cardH - 10, inner1, TRUE);
        DrawBox(x + 16, y + 16, x + cardW - 16, y + cardH - 16, inner2, TRUE);

        for (int t = 0; t < border; ++t)
            DrawBox(x - t, y - t, x + cardW + t, y + cardH + t, GetColor(0, 0, 0), FALSE);

        // hotkey label
        TCHAR hot[16] = TEXT("");
        if (options_[i].hotKey == KEY_INPUT_E) _tcscpy_s(hot, TEXT("[E]"));
        else if (options_[i].hotKey == KEY_INPUT_H) _tcscpy_s(hot, TEXT("[H]"));

        DrawOutlinedText(x + 22, y + 18, hot, GetColor(255, 255, 255), GetColor(0, 0, 0), fontCardSub_);

        // title
        DrawOutlinedText(x + 100, y + 16, options_[i].title, GetColor(255, 255, 255), GetColor(0, 0, 0), fontCardTitle_);

        // sub text (slight wave)
        const float wave = std::sin(subTextAnimFrame_ + (float)i * 0.8f) * 2.0f;
        DrawOutlinedText(x + 104, y + 62 + (int)wave, options_[i].sub1, GetColor(255, 240, 200), GetColor(0, 0, 0), fontCardSub_);
        if (options_[i].sub2 && options_[i].sub2[0] != 0)
            DrawOutlinedText(x + 104, y + 88 + (int)wave, options_[i].sub2, GetColor(255, 240, 200), GetColor(0, 0, 0), fontCardSub_);

        // right info
        TCHAR right[64] = TEXT("");
        _stprintf_s(right, TEXT("STAGE %d   LIVES %d"), options_[i].stageId, options_[i].lives);
        const int rW = GetDrawStringWidthToHandle(right, (int)_tcslen(right), fontCardSub_);
        DrawOutlinedText(x + cardW - 22 - rW, y + cardH - 36, right, GetColor(255, 255, 255), GetColor(0, 0, 0), fontCardSub_);

        // deciding overlay
        if (deciding_ && i == selectedIndex_)
        {
            const float t = min(1.0f, decideTimer_ / 20.0f);
            const int a = (int)(220 * t);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
            DrawBox(x + 16, y + 16, x + cardW - 16, y + cardH - 16, GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            DrawCenteredOutlinedText(x + cardW / 2, y + cardH / 2 - 12, kDecidingText,
                                     GetColor(255, 255, 255), GetColor(0, 0, 0), fontCardSub_);
        }
    }

    // hints
    const int hintY = screenH - 72;
    DrawCenteredOutlinedText(screenW / 2, hintY, kHint1, GetColor(255, 255, 255), GetColor(0, 0, 0), fontHint_);
    DrawCenteredOutlinedText(screenW / 2, hintY + 24, kHint2, GetColor(255, 255, 255), GetColor(0, 0, 0), fontHint_);
}
