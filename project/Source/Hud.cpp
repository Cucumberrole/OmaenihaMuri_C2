#include "Hud.h"
#include <DxLib.h>
#include <algorithm>
#include <string>
#include <cstdio>

static int s_fontLarge = -1;
static int s_fontSmall = -1;
static int s_heartGraph = -1;

static void EnsureFonts()
{
	// フォントがすでに作成済みなら何もしない
	if (s_fontLarge != -1 && s_fontSmall != -1) return;

	// フォント名
	s_fontLarge = CreateFontToHandle("Arial", 28, 3, DX_FONTTYPE_ANTIALIASING_8X8);
	s_fontSmall = CreateFontToHandle("Arial", 20, 2, DX_FONTTYPE_ANTIALIASING_8X8);
}

static void DrawPanel(int x, int y, int w, int h)
{
	// 半透明の黒いパネル + 白い枠線を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
	DrawBox(x, y, x + w, y + h, GetColor(0, 0, 0), TRUE);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(x, y, x + w, y + h, GetColor(255, 255, 255), FALSE);

	// ブレンドを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

static void DrawTextOutlined(int x, int y, const char* text, int textColor, int outlineColor, int fontHandle)
{
	// 文字に縁取りを付けて描画する（上下左右にずらして縁色、最後に本体色）
	DrawStringToHandle(x - 1, y, text, outlineColor, fontHandle);
	DrawStringToHandle(x + 1, y, text, outlineColor, fontHandle);
	DrawStringToHandle(x, y - 1, text, outlineColor, fontHandle);
	DrawStringToHandle(x, y + 1, text, outlineColor, fontHandle);
	DrawStringToHandle(x, y, text, textColor, fontHandle);
}

static std::string FormatScore6(int score)
{
	// スコアを0以上に丸め、6桁ゼロ埋めにする
	score = max(0, score);
	char buf[32];
	std::snprintf(buf, sizeof(buf), "%06d", score);
	return std::string(buf);
}

static std::string FormatTimeMMSS(int totalSeconds)
{
	// 残り秒数を0以上にして、mm:ss形式に整形する
	if (totalSeconds < 0) totalSeconds = 0;
	const int mm = totalSeconds / 60;
	const int ss = totalSeconds % 60;
	char buf[32];
	std::snprintf(buf, sizeof(buf), "%02d:%02d", mm, ss);
	return std::string(buf);
}

void Hud::Init()
{
	// フォントの準備
	EnsureFonts();
}

void Hud::SetHeartGraph(int heartGraphHandle)
{
	// ハートアイコン画像のグラフハンドルを設定
	s_heartGraph = heartGraphHandle;
}

void Hud::Draw(int score, int timeLeftSeconds, int life)
{
	// 毎フレーム描画：フォントがなければ作成
	EnsureFonts();

	int screenW = 0, screenH = 0;
	// 画面サイズを取得（HUDの配置に使う）
	GetDrawScreenSize(&screenW, &screenH);

	const int pad = 16;

	// 左上パネル（スコア/時間）
	const int panelX = pad;
	const int panelY = pad;
	const int panelW = 360;
	const int panelH = 104;

	DrawPanel(panelX, panelY, panelW, panelH);

	// 色設定（縁取り、ラベル、値）
	const int outline = GetColor(0, 0, 0);
	const int labelC = GetColor(255, 230, 120);
	const int valueC = GetColor(255, 255, 255);

	// 1行目/2行目のY座標
	const int row1Y = panelY + 14;
	const int row2Y = panelY + 54;

	// SCORE ラベル + 値
	DrawTextOutlined(panelX + 14, row1Y, "SCORE", labelC, outline, s_fontSmall);
	const std::string scoreStr = FormatScore6(score);
	// 右寄せするために文字列の幅を取得
	const int scoreW = GetDrawStringWidthToHandle(scoreStr.c_str(), -1, s_fontLarge);
	DrawTextOutlined(panelX + panelW - 14 - scoreW, row1Y - 6, scoreStr.c_str(), valueC, outline, s_fontLarge);

	// TIME ラベル + 値
	DrawTextOutlined(panelX + 14, row2Y, "TIME", labelC, outline, s_fontSmall);
	const std::string timeStr = FormatTimeMMSS(timeLeftSeconds);
	// 右寄せするために文字列の幅を取得
	const int timeW = GetDrawStringWidthToHandle(timeStr.c_str(), -1, s_fontLarge);
	DrawTextOutlined(panelX + panelW - 14 - timeW, row2Y - 6, timeStr.c_str(), valueC, outline, s_fontLarge);

	// 右上パネル（ライフ）
	const int lifePanelW = 240;
	const int lifePanelH = 52;
	const int lifeX = screenW - pad - lifePanelW;
	const int lifeY = pad;

	DrawPanel(lifeX, lifeY, lifePanelW, lifePanelH);
	DrawTextOutlined(lifeX + 14, lifeY + 12, "LIFE", labelC, outline, s_fontSmall);

	if (s_heartGraph != -1)
	{
		const int maxHeartsToShow = 5;

		const int iconW = 24;
		const int iconH = 24;
		const int stepX = 28;   // 間隔
		const int hx = lifeX + 78; // 少し左寄せ
		const int hy = lifeY + 14; // 少し下げて中央寄せ

		const int hearts = min(max(life, 0), maxHeartsToShow);

		for (int i = 0; i < hearts; ++i)
		{
			const int x0 = hx + i * stepX;
			const int y0 = hy;
			DrawExtendGraph(x0, y0, x0 + iconW, y0 + iconH, s_heartGraph, TRUE);
		}

		if (life > maxHeartsToShow)
		{
			char buf[16];
			std::snprintf(buf, sizeof(buf), "x%d", life);
			const int w = GetDrawStringWidthToHandle(buf, -1, s_fontLarge);
			DrawTextOutlined(lifeX + lifePanelW - 14 - w, lifeY + 6, buf,
				GetColor(255, 255, 255), GetColor(0, 0, 0), s_fontLarge);
		}
	}

	else
	{
		// ハート画像がない場合の代替表示
		char buf[16];
		std::snprintf(buf, sizeof(buf), "x%d", max(life, 0));
		const int w = GetDrawStringWidthToHandle(buf, -1, s_fontLarge);
		DrawTextOutlined(lifeX + lifePanelW - 14 - w, lifeY + 6, buf, valueC, outline, s_fontLarge);
	}
}

void Hud::Shutdown()
{
	if (s_fontLarge != -1) { DeleteFontToHandle(s_fontLarge); s_fontLarge = -1; }
	if (s_fontSmall != -1) { DeleteFontToHandle(s_fontSmall); s_fontSmall = -1; }
	s_heartGraph = -1;
}