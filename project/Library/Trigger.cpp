#include <sstream>
#include <DxLib.h>
#include "Trigger.h"
#include "time.h"

namespace KeyBuffer
{
    static const float REPEAT_TIME = 0.5f;      //  キーリピート間隔(sec)

    static bool triggerKeyBufferArray[0xff];    //  トリガーキーバッファ
}


/**********************/
/***    KeyUtility  ***/
/**********************/

//  キー関連処理の初期化
void KeyTrigger::Init()
{
    for (int i = 0; i < 0xff; i++)
    {
        KeyBuffer::triggerKeyBufferArray[i] = false;
    }
}

//  キー入力のトリガー取得
bool KeyTrigger::CheckTrigger(int keyCode)
{
    bool triggerFlag = false;

    //  該当キーが押されている
    if (CheckHitKey(keyCode) != 0)
    {
        //  前フレームで押されていなかった＝トリガー
        if (!KeyBuffer::triggerKeyBufferArray[keyCode])
        {
            triggerFlag = true;
        }
        //キーバッファに登録
        KeyBuffer::triggerKeyBufferArray[keyCode] = true;
    }
    else
    {
        //キーバッファに登録
        KeyBuffer::triggerKeyBufferArray[keyCode] = false;
    }

    return triggerFlag;
}