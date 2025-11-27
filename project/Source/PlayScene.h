#pragma once
#include "../Library/SceneBase.h"

class PlayScene : public SceneBase
{
public:
    PlayScene();        // ← 引数なし
    PlayScene(int stage); // ← オプション版（使わなくてもOK）
    ~PlayScene();

    static int SelectedStage;  // ← ステージ番号を保持

    void Update() override;
    void Draw() override;

private:
    int Ssound;
    bool sound;
};
