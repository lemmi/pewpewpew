#include <game/Game.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

void WinScreen_Init(struct Gamestate*);
extern void OnEnter(struct Gamestate*);
extern void OnLeave(struct Gamestate*);
void WinScreen_Update(uint32_t);
void WinScreen_Draw(Bitmap* surface);

Gamestate WinScreen = { WinScreen_Init, OnEnter, OnLeave, WinScreen_Update, WinScreen_Draw };

void WinScreen_Init(struct Gamestate* state)
{
	setFont(fontwhite8);
}

void WinScreen_Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState1();
	if (state.buttons.Start) {
		ExitState();
	}
}

void WinScreen_Draw(Bitmap* b)
{
	ClearBitmap(b);
	DrawText(b, "Game Over", 120, 100);
}
