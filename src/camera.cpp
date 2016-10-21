//========================================================================//
// Copyright (C) 2016 Evan Bowman                                         //
// Liscensed under GPL 3, see: <http://www.gnu.org/licenses/>.            //
//========================================================================//

#include "camera.hpp"

Camera::Camera(Player * _pTarget, const sf::Vector2f & viewPort)  :
		pTarget(_pTarget),
		view(sf::Vector2f(viewPort.x / 2, viewPort.y / 2), viewPort),
		startPosition(view.getCenter()),
		isShaking(false),
		shakeIndex(0),
		shakeTimer(0),
		trackingTimer(0),
		shakeIntensity(0.f),
		state(State::followPlayer)
{}

void Camera::update(const sf::Time & elapsedTime, const std::vector<sf::Vector2f> & targets) {
	float lerpSpeed;
	sf::Vector2f cameraPos;
	if ((state == State::trackMidpoint || state == State::foundEnemy) && targets.empty()) {
		state = State::followPlayer;
		trackingTimer = 0;
	}
	if (pTarget->getState() != Player::State::deactivated &&
		pTarget->getState() != Player::State::dead) {
		switch (state) {
		case State::followPlayer:
			lerpSpeed = math::clamp(elapsedTime.asMicroseconds() * 0.0000025f, 0.f, 1.f);
			cameraPos = math::lerp(pTarget->getPosition(), view.getCenter(), lerpSpeed);
			if (!targets.empty()) {
				state = State::foundEnemy;
				trackingTimer = 0;
			}
			break;

		case State::trackMidpoint:
			{
				lerpSpeed = math::clamp(elapsedTime.asMicroseconds() * 0.0000025f, 0.f, 1.f);
				sf::Vector2f aggregate;
				float divisor = 0;
				for (const auto & vec : targets) {
					aggregate += vec;
					++divisor;
				}
				aggregate /= divisor;
				// Enemies entering and leaving the camera view creates jarring shifts
				// so I'm using a buffer of the average enemy positions
				buffer = math::lerp(buffer, aggregate, lerpSpeed * 0.1f);
				midpoint = math::lerp(pTarget->getPosition(), buffer, 0.78);
				cameraPos = math::lerp(midpoint, view.getCenter(), lerpSpeed);
			}
			break;

		case State::foundEnemy:
			{
				lerpSpeed = math::clamp(elapsedTime.asMicroseconds() * 0.0000025f, 0.f, 1.f);
				sf::Vector2f aggregate;
				float divisor = 0;
				for (const auto & vec : targets) {
					aggregate += vec;
					++divisor;
				}
				aggregate /= divisor;
				trackingTimer += elapsedTime.asMicroseconds();
				float targetWeight = 1.f - 0.22f * Easing::easeIn<1>(trackingTimer, static_cast<int64_t>(900000));
				midpoint = math::lerp(pTarget->getPosition(), aggregate, targetWeight);
				cameraPos = math::lerp(midpoint, view.getCenter(), lerpSpeed);
				if (trackingTimer > 900000) {
					state = State::trackMidpoint;
					buffer = aggregate;
				}
			}
			break;
		}
	} else {
		state = State::followPlayer;
		lerpSpeed = math::clamp(elapsedTime.asMicroseconds() * 0.0000055f, 0.f, 1.f);
		cameraPos = math::lerp(pTarget->getPosition(), view.getCenter(), lerpSpeed);
	}
	if (isShaking) {
		shakeTimer += elapsedTime.asMicroseconds();
		if (shakeTimer > 50000) {
			shakeTimer = 0;
			shakeIndex += 1;
			if (shakeIndex > 4) {
				shakeIndex = 4;
				isShaking = false;
				shakeTimer = 0;
			}
		}
		const static std::array<int, 5> shakeConstants = {{3, -5, 3, -2, 1}};
		float shakeOffset = shakeConstants[shakeIndex];
		cameraPos.y += shakeOffset * shakeIntensity;
	}
	view.setCenter(cameraPos);
}

const sf::View & Camera::getView() const {
	return view;
}

void Camera::panDown() {
	float placementOffset = pTarget->getPosition().y / 2;
	view.setCenter(sf::Vector2f(pTarget->getPosition().x, placementOffset));
	startPosition = view.getCenter();
}

void Camera::snapToTarget() {
	view.setCenter(pTarget->getPosition());
	startPosition = view.getCenter();
}

void Camera::setView(const sf::View & _view) {
	view = _view;
}

bool Camera::moving() const {
	const sf::Vector2f playerPos = pTarget->getPosition();
	const sf::Vector2f & cameraPos = view.getCenter();			
	if (std::abs(playerPos.x - cameraPos.x) < 0.5f &&
		std::abs(playerPos.y - cameraPos.y) < 0.5f) {
		return false;
	}
	return true;
}

void Camera::shake(float _shakeIntensity) {
	if (!isShaking) {
		shakeIntensity = _shakeIntensity;
		isShaking = true;
		shakeIndex = 0;
	}
}

sf::Vector2f Camera::getOffset() const {
	return view.getCenter() - startPosition;
}
