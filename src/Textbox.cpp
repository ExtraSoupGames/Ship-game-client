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

Textbox::Textbox(string defaultText, int x, int y, int pScreenScaling, TextureManager* textureManager, int fontSize)
	: ClickableUIElement(x * pScreenScaling, y * pScreenScaling, 160 * pScreenScaling, 16 * pScreenScaling, [this] { this->Select(); }, [this] {this->Deselect(); })
{
	selected = false;
	currentText = defaultText;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, pScreenScaling);
	hoverTexture = textureManager->GetTexture("UI\\ButtonHover");
	unHoverTexture = textureManager->GetTexture("UI\\Button");
	currentTexture = unHoverTexture;
	screenScaling = pScreenScaling;
}

Textbox::~Textbox()
{
	TTF_CloseFont(font);
}

void Textbox::Render(SDL_Renderer* renderer)
{
	SDL_Rect* destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, currentTexture, NULL, destRect);
	UIRendering::RenderText(renderer, currentText, x + (3 * screenScaling), y + (3 * screenScaling), font);
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

void Textbox::OnHover()
{
	currentTexture = hoverTexture;
}

void Textbox::OnUnHover()
{
	currentTexture = unHoverTexture;
}
