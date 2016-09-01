//========================================================================//
// Copyright (C) 2016 Evan Bowman                                         //
// Liscensed under GPL 3, see: <http://www.gnu.org/licenses/>.            //
//========================================================================//

#include "userInterface.hpp"
#include <cmath>
#include "player.hpp"

ui::Backend::Backend() :
	state(State::closed),
	selectorPosition(0),
	timer(0),
	timerAlt(0),
	powerupTimer(0),
	dispPowerupBar(false),
	blurAmount(0.f),
	desaturateAmount(0.f)
{}

bool ui::Backend::isOpen() const {
	return state != State::closed;
}

ui::Backend::State ui::Backend::getState() const {
	return state;
}

void ui::Backend::draw(sf::RenderWindow & window, ui::Frontend & uIFrontEnd) {
	switch (state) {
	case State::deathScreenEntry:
		{
			uint8_t textAlpha = desaturateAmount * 255 + 38.25f;
			uIFrontEnd.drawDeathText(static_cast<unsigned char>(textAlpha), window);
		}
		break;

	case State::menuScreenEntry:
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::resumeText);
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::settingsText);
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::quitText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::resumeText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::settingsText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::quitText);
		break;

	case State::menuScreenExit:
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::resumeText);
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::settingsText);
		uIFrontEnd.setTextAlpha(255 * blurAmount, ui::Frontend::Text::quitText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::resumeText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::settingsText);
		uIFrontEnd.setTextOffset(0, -0.01 * blurAmount, ui::Frontend::Text::quitText);
		break;
		
	case State::deathScreen:
		uIFrontEnd.drawDeathText(255, window);
		break;

	case State::deathScreenExit:
		{
			uint8_t textAlpha = Easing::easeOut<3>(timer, 300) * 255;
			if (textAlpha > 0) {
				uIFrontEnd.drawDeathText(textAlpha, window);
			}
		}
		break;

	default:
		break;
	}
}

void ui::Backend::resetPowerupBar(Powerup _powerup) {
	dispPowerupBar = true;
	powerup = _powerup;
	powerupTimer = 0;
}

Powerup ui::Backend::getCurrentPowerup() const {
	return powerup;
}

void ui::Backend::update(Player & player, ui::Frontend & uIFrontEnd, InputController * pInput, const sf::Time & elapsed) {
	bool action = pInput->actionPressed();
	bool up = pInput->upPressed();
	bool down = pInput->downPressed();
	bool pause = pInput->pausePressed();
	switch (state) {
	case State::closed:
		if (dispPowerupBar) {
			const static int64_t powerupTimerCeil = 18000000;
			powerupTimer += elapsed.asMicroseconds();
			float barWidth = Easing::easeOut<1>(powerupTimer, powerupTimerCeil);
			uIFrontEnd.setBarWidth(barWidth);
			if (powerupTimer > powerupTimerCeil) {
				dispPowerupBar = false;
				powerup = Powerup::nil;
			}
		}
		if (pause) {
			state = State::menuScreenEntry;
			player.setState(Player::State::deactivated);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::waypointText);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::healthNumText);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::scoreText);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::heart);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::coin);
		}
		break;
		
	case State::menuScreenEntry:
		timer += elapsed.asMilliseconds();
		blurAmount = Easing::easeIn<3>(timer, 280);
		if (blurAmount == 1.f) {
			state = State::menuScreen;
			uIFrontEnd.setTextAlpha(255, ui::Frontend::Text::resumeText);
			uIFrontEnd.setTextAlpha(255, ui::Frontend::Text::settingsText);
			uIFrontEnd.setTextAlpha(255, ui::Frontend::Text::quitText);
		}
		break;
		
	case State::menuScreen:
		if (pause) {
			timer = 0;
			selectorPosition = 0;
			state = State::menuScreenExit;
		} else if (up) {
			if (selectorPosition > 0) {
				--selectorPosition;
			}
		} else if (down) {
		    if (selectorPosition < 2) {
				++selectorPosition;
			}
		} else if (action) {
			switch (selectorPosition) {
			case 0:
				selectorPosition = 0;
				state = State::menuScreenExit;
				timer = 0;
				break;

			case 1:
				// TODO: settings screen
				break;

			case 2:
				throw ShutdownSignal();
				break;
			}
		}
		break;
		
	case State::menuScreenExit:
		timer += elapsed.asMilliseconds();
		blurAmount = Easing::easeOut<3>(timer, 280);
		if (blurAmount == 0.f) {
			state = State::closed;
			timer = 0;
			player.activate();
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::resumeText);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::settingsText);
			uIFrontEnd.setTextAlpha(0, ui::Frontend::Text::quitText);
		}
		break;
			
	case State::deathScreenEntry:
		timer += elapsed.asMilliseconds();
		if (timer > 20) {
			timer -= 20;
			desaturateAmount += 0.01f;
		}
		if (desaturateAmount > 0.85f) {
			desaturateAmount = 0.85f;
			state = State::deathScreen;
			timerAlt = 0;
		}
		break;
			
	case State::deathScreen:
		timer += elapsed.asMilliseconds();
		timerAlt += elapsed.asMilliseconds();
		if (timer > 20) {
			timer -= 20;
			if (timerAlt < 1500) {
				// TODO: This previously zoomed the window... re-implement?
			} else {
				timer = 0;
				state = State::deathScreenExit;
			}
		}
		break;
			
	case State::deathScreenExit:
		timer += elapsed.asMilliseconds();
		blurAmount = Easing::easeIn<3>(timer, 300);
		if (timer >= 300) {
			timer = 0;
			blurAmount = 1.f;
			state = State::statsScreen;
		}
		break;
			
	case State::statsScreen:
		if (action) {
			state = State::complete;
		}
		break;
			
	case State::complete:
		break;

	case State::settingsScreen:
		// TODO: Display various settings, go to customization screens...
		// if (...) -> selectorPosition = 0, state := customizeKeyboardScreen
		// if (...) -> selectorposition = 0, state := customizeJoystickScreen
		break;

	case State::customizeKeyboardScreen:
		// TODO: while in this state display text and select each heading accordingly
		break;

	case State::customizeJoystickScreen:
		// TODO: while in this state display text and select each heading accordingly
		break;
	}
}

void ui::Backend::dispDeathSeq() {
	if (state == ui::Backend::State::closed) {
		timer = 0;
		state = ui::Backend::State::deathScreenEntry;
	}
}

bool ui::Backend::isComplete() {
	return state == State::complete;
}

float ui::Backend::getBlurAmount() {
	return blurAmount;
}

void ui::Backend::reset() {
	state = State::closed;
	desaturateAmount = 0.f;
	blurAmount = 0.f;
	timer = 0;
}

float ui::Backend::getDesaturateAmount() {
	return desaturateAmount;
}

bool ui::Backend::blurEnabled() {
	return blurAmount != 0.f;
}

bool ui::Backend::desaturateEnabled() {
	return desaturateAmount > 0.f;
}

#define HEALTH_TEXT_FADE_SECONDS 3
#define SCORE_TEXT_FADE_SECONDS 3
#define WAYPOINT_TEXT_FADE_SECONDS 3

ui::Frontend::Frontend(sf::View fontView, float x, float y) :
	healthModified(false),
	scoreModified(false),
	barWidth(0.f)
{
	// Store the view to use when drawing fonts
	this->fontView = fontView;
	windowCenterX = x;
	windowCenterY = y;
	maxHealth = 4;
	health = 4;
	score = 0;
	float scale;
	if (fontView.getSize().y < fontView.getSize().x) {
		scale = fontView.getSize().y;
	} else {
		scale = fontView.getSize().x;
	}
	heart.setPointCount(20);
	heart.setPoint(0, sf::Vector2f(38, 72));
	heart.setPoint(1, sf::Vector2f(8, 44));
	heart.setPoint(2, sf::Vector2f(4, 38));
	heart.setPoint(3, sf::Vector2f(0, 30));
	heart.setPoint(4, sf::Vector2f(0, 20));
	heart.setPoint(5, sf::Vector2f(2, 8));
	heart.setPoint(6, sf::Vector2f(8, 2));
	heart.setPoint(7, sf::Vector2f(18, 0));
	heart.setPoint(8, sf::Vector2f(24, 0));
	heart.setPoint(9, sf::Vector2f(32, 4));
	heart.setPoint(10, sf::Vector2f(38, 10));
	heart.setPoint(11, sf::Vector2f(44, 4));
	heart.setPoint(12, sf::Vector2f(52, 0));
	heart.setPoint(13, sf::Vector2f(58, 0));
	heart.setPoint(14, sf::Vector2f(68, 2));
	heart.setPoint(15, sf::Vector2f(74, 8));
	heart.setPoint(16, sf::Vector2f(76, 20));
	heart.setPoint(17, sf::Vector2f(76, 30));
	heart.setPoint(18, sf::Vector2f(72, 38));
	heart.setPoint(19, sf::Vector2f(68, 44));
	heart.setScale(0.0005f * scale, 0.0005f * scale);
	heart.setOrigin(heart.getLocalBounds().width / 2, heart.getLocalBounds().height / 2);
	
	coin.setFillColor(sf::Color::White);
	coin.setPointCount(20);
	coin.setRadius(0.018f * scale);
	coin.setOrigin(coin.getLocalBounds().width / 2, coin.getLocalBounds().height / 2);

	const sf::Font & cornerstone = global::resourceHandlerPtr->getFont(ResourceHandler::Font::cornerstone);

	auto initText = [](const sf::Font & font, sf::Text & text, const std::string string, float size) {
		text.setFont(font);
		text.setCharacterSize(size);
		text.setString(string);
	};
	initText(cornerstone, waypointText, std::string("WAYPOINT-1"), 0.055f * scale);
	waypointText.setPosition(16, 0);

	initText(cornerstone, scoreText, std::string("0: "), 0.032f * scale);
	
	healthNumText.setString(std::to_string(4) + " / " + std::to_string(static_cast<int>(maxHealth)) + ": ");
	healthNumText.setCharacterSize(0.032 * scale);
	healthNumText.setFont(cornerstone);

	initText(cornerstone, resumeText, std::string("RESUME"), 0.085f * scale);
	resumeText.setColor(sf::Color(255, 255, 255, 0));
	
	initText(cornerstone, settingsText, std::string("SETTINGS"), 0.085 * scale);
	settingsText.setColor(sf::Color(255, 255, 255, 0));
	
	initText(cornerstone, quitText, std::string("QUIT"), 0.085 * scale);
	quitText.setColor(sf::Color(255, 255, 255, 0));
	
	initText(cornerstone, titleText, std::string("BLIND JUMP"), 0.115f * scale);
	titleText.setPosition(fontView.getSize().x / 2 - titleText.getLocalBounds().width / 2,
						  fontView.getSize().y / 8 - titleText.getLocalBounds().height / 2);
	titleText.setColor(sf::Color(255, 255, 255, 0));

	initText(cornerstone, deathText, std::string("YOU DIED"), 0.115 * scale);
	deathText.setColor(sf::Color(231, 26, 83));
	deathText.setPosition(fontView.getSize().x / 2 - deathText.getLocalBounds().width / 2,
						  fontView.getSize().y / 12 - deathText.getLocalBounds().height / 2);

	sf::Vector2f barPosition(0.f, fontView.getSize().y - fontView.getSize().y * 0.008f);
	powerupBarFront.setPosition(barPosition);
	powerupBarBack.setPosition(barPosition);
	powerupBarBack.setSize(sf::Vector2f(fontView.getSize().x, fontView.getSize().y * 0.008f));
	powerupBarBack.setFillColor(sf::Color(160, 160, 160, 255));
	powerupBarFront.setFillColor(sf::Color::White);
}

void ui::Frontend::setTextOffset(float xOffset, float yOffset, ui::Frontend::Text text) {
	float scale;
	if (fontView.getSize().y < fontView.getSize().x) {
		scale = fontView.getSize().y;
	} else {
		scale = fontView.getSize().x;
	}
	switch (text) {
	case Text::resumeText:
		resumeText.setPosition(xOffset * scale + fontView.getSize().x / 2 - resumeText.getLocalBounds().width / 2,
							   fontView.getSize().y / 2.8f - resumeText.getLocalBounds().height / 2 + yOffset * scale);
		break;

	case Text::settingsText:
		settingsText.setPosition(xOffset * scale + fontView.getSize().x / 2 - settingsText.getLocalBounds().width / 2,
								 resumeText.getPosition().y + settingsText.getLocalBounds().height * 2 + yOffset * scale);
		break;

	case Text::quitText:
		quitText.setPosition(xOffset * scale + fontView.getSize().x / 2 - quitText.getLocalBounds().width / 2,
							 settingsText.getPosition().y + quitText.getLocalBounds().height * 2 + yOffset * scale);
		break;

	default:
		// TODO: handle other cases...
		break;
	}
}

void ui::Frontend::setTextAlpha(uint8_t alpha, ui::Frontend::Text text) {
	switch (text) {
	case Text::resumeText:
		resumeText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::settingsText:
		settingsText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::quitText:
		quitText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::healthNumText:
		healthNumText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::scoreText:
		scoreText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::waypointText:
		waypointText.setColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::heart:
		heart.setFillColor(sf::Color(255, 255, 255, alpha));
		break;

	case Text::coin:
		coin.setFillColor(sf::Color(255, 255, 255, alpha));
		break;
	}
}

void ui::Frontend::reset() {
	score = 0;
}

void ui::Frontend::setWaypointText(int level) {
	heart.setPosition(fontView.getSize().x - heart.getLocalBounds().width / 2, heart.getLocalBounds().height / 1.8);
	//coin.setPosition(fontView.getSize().x - coin.getLocalBounds().width, heart.getPosition().y + coin.getLocalBounds().height);
	coin.setPosition(heart.getPosition().x, heart.getPosition().y + coin.getLocalBounds().height * 1.25);
	coin.setFillColor(sf::Color(255, 255, 255, 0));
	healthNumText.setPosition(fontView.getSize().x - healthNumText.getLocalBounds().width - heart.getLocalBounds().width,
							  healthNumText.getLocalBounds().height);
	if (health != 1) {
		healthNumText.setColor(sf::Color(255, 255, 255, 0));
		heart.setFillColor(sf::Color(255, 255, 255, 0));
	}
	scoreText.setPosition(fontView.getSize().x
						  - scoreText.getLocalBounds().width
						  - fontView.getSize().x * 0.015
						  - heart.getLocalBounds().width, scoreText.getLocalBounds().height * 3);
	std::string str = "WAYPOINT-";
	str += std::to_string(level);
	waypointText.setString(str);
	// Reset the color, it will fade out
	waypointText.setColor(sf::Color::White);
	scoreText.setColor(sf::Color(255, 255, 255, 4));
	if (level != 0) {
		wpTextDisplayTimer.restart();
		if (health != 1) {
			healthDisplayTimer.restart();
		}
		scoreDisplayTimer.restart();
	}
}

int ui::Frontend::getScore() {
	return score;
}

void ui::Frontend::print(sf::RenderWindow & window) {
	// Set the correct view, so not to blur the fonts
	window.setView(fontView);
	if (barWidth > 0) {
	    powerupBarFront.setSize(sf::Vector2f(fontView.getSize().x * barWidth, fontView.getSize().y * 0.01f));
		window.draw(powerupBarBack, sf::BlendMultiply);
		window.draw(powerupBarFront);
	}
	if (healthModified) {
		healthModified = false;
		healthNumText.setString(std::to_string(static_cast<int>(health))
								+ " / "
								+ std::to_string(static_cast<int>(maxHealth))
								+ " :");
		healthNumText.setPosition(fontView.getSize().x
								  - healthNumText.getLocalBounds().width
								  - fontView.getSize().x * 0.015
								  - heart.getLocalBounds().width, healthNumText.getLocalBounds().height);
	}
	if (scoreModified) {
		scoreModified = false;
		scoreText.setString(std::to_string(score) + " :");
		scoreText.setPosition(fontView.getSize().x
							  - scoreText.getLocalBounds().width
							  - fontView.getSize().x * 0.015
							  - heart.getLocalBounds().width, scoreText.getLocalBounds().height * 3);
	}
	// Slowly fade out the  waypoint text
	sf::Color c = waypointText.getColor();
	if (c.a > 5) {
		if (wpTextDisplayTimer.getElapsedTime().asSeconds() > WAYPOINT_TEXT_FADE_SECONDS) {
			c.a -= 4;
			waypointText.setColor(c);
		}
		window.draw(waypointText);
	}

	if (health != 1) {
		c = healthNumText.getColor();
		if (c.a > 5) {
			if (healthDisplayTimer.getElapsedTime().asSeconds() > HEALTH_TEXT_FADE_SECONDS) {
				c.a -= 4;
				healthNumText.setColor(c);
				heart.setFillColor(c);
				heart.setOutlineColor(c);
			}
			window.draw(healthNumText);
			window.draw(heart);
		}
	} else {
		window.draw(healthNumText);
		window.draw(heart);
	}
	
	c = scoreText.getColor();
	if (c.a > 5) {
		if (scoreDisplayTimer.getElapsedTime().asSeconds() > SCORE_TEXT_FADE_SECONDS) {
			c.a -= 4;
			scoreText.setColor(c);
			coin.setFillColor(c);
		}
		window.draw(scoreText);
		window.draw(coin);
	}

	c = resumeText.getColor();
	if (c.a > 0) {
		window.draw(resumeText);
	}

	c = settingsText.getColor();
	if (c.a > 0) {
		window.draw(settingsText);
	}

	c = quitText.getColor();
	if (c.a > 0) {
		window.draw(quitText);
	}
}

void ui::Frontend::resetWPText() {
	waypointText.setColor(sf::Color::White);
	wpTextDisplayTimer.restart();
}

void ui::Frontend::resetSCText() {
	scoreText.setColor(sf::Color::White);
	coin.setFillColor(sf::Color::White);
	scoreDisplayTimer.restart();
}

void ui::Frontend::updateScore(int offset) {
	score += offset;
	resetSCText();
	scoreModified = true;
}

void ui::Frontend::resetHPText() {
	healthNumText.setColor(sf::Color::White);
	heart.setFillColor(sf::Color::White);
	heart.setOutlineColor(sf::Color::White);
	healthDisplayTimer.restart();
}

void ui::Frontend::drawTitle(unsigned char alpha, sf::RenderWindow& window) {
	titleText.setColor(sf::Color(255, 255, 255, alpha));
	window.setView(fontView);
	window.draw(titleText);
}

void ui::Frontend::drawDeathText(unsigned char alpha, sf::RenderWindow & window) {
	deathText.setColor(sf::Color(colors::Ruby::R * 255, colors::Ruby::G * 255, colors::Ruby::B * 255, alpha));
	window.setView(fontView);
	window.draw(deathText);
}

sf::Text * ui::Frontend::getTitle() {
	return &titleText;
}

void ui::Frontend::updateHealth(char _health) {
	health = _health;
	// Can't update the actual text from here, since it needs to happen on the main thread.
	// Instead mark it as modifed and handle it later
	healthModified = true;
	resetHPText();
}

void ui::Frontend::updateMaxHealth(char _maxHealth) {
	maxHealth = _maxHealth;
	healthModified = true;
	resetHPText();
}

char ui::Frontend::getMaxHealth() const {
	return maxHealth;
}

sf::Text * ui::Frontend::getDeathText() {
	return &deathText;
}

void ui::Frontend::setBarWidth(float _barWidth) {
    barWidth = _barWidth;
}
