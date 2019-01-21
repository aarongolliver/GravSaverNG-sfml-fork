#pragma once

void DarkenScreen(sf::RenderTexture &renderTexture);

void Update();

void SetDeltaT();

void HandleEvents(sf::RenderWindow &window);

void DrawToWindow(sf::RenderTexture &renderTexture, sf::RenderWindow &window);
