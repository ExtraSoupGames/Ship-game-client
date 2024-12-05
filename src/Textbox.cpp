#include "Textbox.h"

void Textbox::Select()
{
	selected = true;
}

void Textbox::Deselect()
{
	selected = false;
}

char Textbox::GetCharFromEvent(SDL_Event& e)
{
	switch (e.key.keysym.sym) {
	default:
		return (char)e.key.keysym.sym;
	}
}

Textbox::Textbox(string defaultText, int x, int y, int width, int height, int pScreenScaling, int fontSize)
	: ClickableUIElement(x * pScreenScaling, y * pScreenScaling, width * pScreenScaling, height * pScreenScaling, [this] { this->Select(); }, [this] {this->Deselect(); })
{
	selected = false;
	currentText = defaultText;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, pScreenScaling);
}

Textbox::~Textbox()
{
	TTF_CloseFont(font);
}

void Textbox::Render(SDL_Renderer* renderer)
{
	UIRendering::RenderText(renderer, currentText, x, y, font);
}


void Textbox::Input(SDL_Event& e)
{
	HandleClickInput(e);
	if (selected) {
		//if selected -> keyboard input
		if (e.type == SDL_KEYDOWN) {
			//special keys
			switch (e.key.keysym.sym) {
			case SDLK_BACKSPACE:
				if (currentText.size() > 0) {
					currentText.erase(currentText.size() - 1);
				}
				break;
			default:
				currentText += GetCharFromEvent(e);
				break;
			}
		}
	}
}

string Textbox::GetText()
{
	return currentText;
}
