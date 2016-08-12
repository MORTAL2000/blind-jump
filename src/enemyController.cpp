//========================================================================//
// Copyright (C) 2016 Evan Bowman                                         //
//                                                                        //
// This program is free software: you can redistribute it and/or modify   //
// it under the terms of the GNU General Public License as published by   //
// the Free Software Foundation, either version 3 of the License, or      //
// (at your option) any later version.                                    //
//                                                                        //
// This program is distributed in the hope that it will be useful,        //
// but WITHOUT ANY WARRANTY; without even the implied warranty of         //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
// GNU General Public License for more details.                           //
//                                                                        //
// You should have received a copy of the GNU General Public License      //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.  //
//========================================================================//

#include "enemyController.hpp"
#include "ResourcePath.hpp"
#include <cmath>
#include "wall.hpp"
#include "tileController.hpp"
#include "detailController.hpp"
#include "screenShakeController.hpp"
#include "camera.hpp"

enemyController::enemyController() {}

void enemyController::draw(drawableVec & gameObjects, drawableVec & gameShadows, Camera & camera) {
	const sf::View & cameraView = camera.getView();
	sf::Vector2f viewCenter = cameraView.getCenter();
	sf::Vector2f viewSize = cameraView.getSize();
	for (auto & element : turrets) {
		if (element.getXpos() > viewCenter.x - viewSize.x / 2 - 48 && element.getXpos() < viewCenter.x + viewSize.x / 2 + 48 && element.getYpos() > viewCenter.y - viewSize.y / 2 - 48 && element.getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
			// Get the enemy's shadow
			std::tuple<sf::Sprite, float, Rendertype, float> shadow;
			std::get<0>(shadow) = element.getShadow();
			gameShadows.push_back(shadow);
			std::tuple<sf::Sprite, float, Rendertype, float> tSpr;
			std::get<0>(tSpr) = element.getSprite();
			std::get<1>(tSpr) = element.getYpos();
			if (element.colored()) {
				std::get<2>(tSpr) = Rendertype::shadeWhite;
			} else {
				std::get<2>(tSpr) = Rendertype::shadeDefault;
			}
			gameObjects.push_back(tSpr);
		}
	}

	for (auto & element : critters) {
		gameShadows.emplace_back(element.getShadow(), 0.f, Rendertype::shadeDefault, 0.f);
		std::tuple<sf::Sprite, float, Rendertype, float> tSpr;
		std::get<0>(tSpr) = element.getSprite();
		std::get<1>(tSpr) = element.getYpos() - 16;
		// If the enemy should be colored, let the rendering code know to pass it through a fragment shader
		if (element.isColored()) {
			gameObjects.emplace_back(element.getSprite(), element.getYpos() - 16, Rendertype::shadeWhite, element.getColorAmount());
		} else {
			gameObjects.emplace_back(element.getSprite(), element.getYpos() - 16, Rendertype::shadeDefault, 0.f);
		}		
	}
	
	for (auto & element : scoots) {
		if (element.getXpos() > viewCenter.x - viewSize.x / 2 - 48 && element.getXpos() < viewCenter.x + viewSize.x / 2 + 48 && element.getYpos() > viewCenter.y - viewSize.y / 2 - 48 && element.getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
			gameShadows.emplace_back(element.getShadow(), 0.f, Rendertype::shadeDefault, 0.f);
			if (element.isColored()) {
				gameObjects.emplace_back(element.getSprite(), element.getYpos() - 16, Rendertype::shadeWhite, element.getColorAmount());
			} else {
				gameObjects.emplace_back(element.getSprite(), element.getYpos() - 16, Rendertype::shadeDefault, 0.f);
			}
		}
	}
	
	for (auto & element : dashers) {
		if (element.getXpos() > viewCenter.x - viewSize.x / 2 - 48 && element.getXpos() < viewCenter.x + viewSize.x / 2 + 48 && element.getYpos() > viewCenter.y - viewSize.y / 2 - 48 && element.getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
			auto state = element.getState();
			if (state != Dasher::State::dying && state != Dasher::State::dead) {
				gameShadows.emplace_back(element.getShadow(), 0.f, Rendertype::shadeDefault, 0.f);
			}		
			for (auto & blur : *element.getBlurEffects()) {
				gameObjects.emplace_back(*blur.getSprite(), blur.yInit + 200, Rendertype::shadeDefault, 0.f);
			}			
			if (element.isColored()) {
				gameObjects.emplace_back(element.getSprite(), element.getYpos(), Rendertype::shadeWhite, element.getColorAmount());
			} else {
				gameObjects.emplace_back(element.getSprite(), element.getYpos(), Rendertype::shadeDefault, 0.f);
			}
		}
	}
}

void enemyController::update(float x, float y, EffectGroup & ef, std::vector<wall> w, bool enabled, tileController* pTiles, sf::Time & elapsedTime, Camera & camera, std::vector<sf::Vector2f> & cameraTargets) {
	const sf::View & cameraView = camera.getView();
	sf::Vector2f viewCenter = cameraView.getCenter();
	sf::Vector2f viewSize = cameraView.getSize();
	if (!turrets.empty()) {
		for (auto it = turrets.begin(); it != turrets.end();) {
			if (it->getKillFlag() == 1) {
				it = turrets.erase(it);
			} else {
				if (enabled) {
					it->update(elapsedTime, x, y, ef);
				}
				++it;
			}
		}
	}
	
	if (!scoots.empty()) {
		for (auto it = scoots.begin(); it != scoots.end();) {
			if (it->getKillFlag()) {
				util::sleep(milliseconds(45));
				camera.shake(0.1f);
				it = scoots.erase(it);
			} else {
				if (it->getXpos() > viewCenter.x - viewSize.x / 2 - 48 &&
					it->getXpos() < viewCenter.x + viewSize.x / 2 + 48 &&
					it->getYpos() > viewCenter.y - viewSize.y / 2 - 48 &&
					it->getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
					if (enabled) {
						it->update(x, y, w, ef, elapsedTime);
					}
					cameraTargets.emplace_back(it->getXpos(), it->getYpos());
				}
				++it;
			}
		}
	}
	
	if (!critters.empty()) {
		// Need to check if each enemy overlaps with any other enemies so that they don't bunch up
		for (size_t i = 0; i < critters.size(); i++) {
			for (size_t j = 0; j < critters.size(); j++) {
				// Obviously it would be bad to compare an object to itself.
				if (i != j) {
					// If the enemy at index i is active and overlaps with another enemy that is also active...
					if (fabs(critters[i].getXpos() - critters[j].getXpos()) < 12 && fabs(critters[i].getYpos() - critters[j].getYpos()) < 12 && critters[i].isActive() && critters[j].isActive()) {
						critters[i].deActivate();
					}
				}
			}
		}
		
		for (auto it = critters.begin(); it != critters.end();) {
			if (it->getKillFlag()) {
				util::sleep(milliseconds(45));
				camera.shake(0.1f);
				it = critters.erase(it);
			} else {
				if (it->getXpos() > viewCenter.x - viewSize.x / 2 - 48 &&
					it->getXpos() < viewCenter.x + viewSize.x / 2 + 48 &&
					it->getYpos() > viewCenter.y - viewSize.y / 2 - 48 &&
					it->getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
					cameraTargets.emplace_back(it->getXpos(), it->getYpos());
				}
				if (enabled) {
					it->critterUpdate(x, y, ef, elapsedTime, pTiles);
				}
				++it;
			}
		}
		
		// Now we have to reactivate all of the objects. Perhaps there is a smarter way than this brute force approach...
		for (auto & element : critters) {
			element.activate();
		}
	}
	
	if (!dashers.empty()) {
		for (auto & element : dashers) {
			if (element.getXpos() > viewCenter.x - viewSize.x / 2 - 48 &&
				element.getXpos() < viewCenter.x + viewSize.x / 2 + 48 &&
				element.getYpos() > viewCenter.y - viewSize.y / 2 - 48 &&
				element.getYpos() < viewCenter.y + viewSize.y / 2 + 48) {
				if (enabled) {
					element.update(x, y, w, ef, elapsedTime);
					if (element.getState() != Dasher::State::dead) {
						cameraTargets.emplace_back(element.getXpos(), element.getYpos());
					}
				}
				if (element.getKillFlag()) {
					util::sleep(milliseconds(45));
					camera.shake(0.1f);
					element.setKillFlag(false);
				}
			}
		}
	}
}

void enemyController::clear() {
	turrets.clear();
	scoots.clear();
	dashers.clear();
	critters.clear();
}

void enemyController::addTurret(tileController * pTiles) {
	auto pCoordVec = pTiles->getEmptyLocations();
	int locationSelect = (std::abs(static_cast<int>(globalRNG())) % 2) ? std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size() / 2) : std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size());
	float xInit = (*pCoordVec)[locationSelect].x * 32 + pTiles->getPosX();
	float yInit = (*pCoordVec)[locationSelect].y * 26 + pTiles->getPosY();
	turrets.emplace_back(globalResourceHandlerPtr->getTexture(ResourceHandler::Texture::gameObjects), xInit, yInit);
	(*pCoordVec)[locationSelect] = pCoordVec->back();
	pCoordVec->pop_back();
}

void enemyController::addScoot(tileController * pTiles) {
	auto pCoordVec = pTiles->getEmptyLocations();
	int locationSelect = (std::abs(static_cast<int>(globalRNG())) % 2) ? std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size() / 2) : std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size());
	float xInit = (*pCoordVec)[locationSelect].x * 32 + pTiles->getPosX();
	float yInit = (*pCoordVec)[locationSelect].y * 26 + pTiles->getPosY();
	scoots.emplace_back(globalResourceHandlerPtr->getTexture(ResourceHandler::Texture::gameObjects),
						globalResourceHandlerPtr->getTexture(ResourceHandler::Texture::scootShadow),
						xInit, yInit);
	(*pCoordVec)[locationSelect] = pCoordVec->back();
	pCoordVec->pop_back();
}

void enemyController::addDasher(tileController * pTiles) {
	auto pCoordVec = pTiles->getEmptyLocations();
	int locationSelect = (std::abs(static_cast<int>(globalRNG())) % 2) ? std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size() / 2) : std::abs(static_cast<int>(globalRNG())) % (pCoordVec->size() / 2);
	float xInit = (*pCoordVec)[locationSelect].x * 32 + pTiles->getPosX();
	float yInit = (*pCoordVec)[locationSelect].y * 26 + pTiles->getPosY();
	dashers.emplace_back(globalResourceHandlerPtr->getTexture(ResourceHandler::Texture::gameObjects),
						 xInit, yInit);
	(*pCoordVec)[locationSelect] = pCoordVec->back();
	pCoordVec->pop_back();
}

void enemyController::addCritter(tileController * pTiles) {
	auto pCoordVec = pTiles->getEmptyLocations();
	int locationSelect = std::abs(static_cast<int>(globalRNG())) % pCoordVec->size();
	float xInit = (*pCoordVec)[locationSelect].x * 32 + pTiles->getPosX();
	float yInit = (*pCoordVec)[locationSelect].y * 26 + pTiles->getPosY();
	critters.emplace_back(globalResourceHandlerPtr->getTexture(ResourceHandler::Texture::gameObjects),
						  pTiles->mapArray, xInit, yInit);
	(*pCoordVec)[locationSelect] = pCoordVec->back();
	pCoordVec->pop_back();
}

void enemyController::setWindowSize(float windowW, float windowH) {
	this->windowW = windowW;
	this->windowH = windowH;
}

std::vector<Critter> & enemyController::getCritters() {
	return critters;
}

std::vector<Dasher> & enemyController::getDashers() {
	return dashers;
}

std::vector<Scoot> & enemyController::getScoots() {
	return scoots;
}

std::vector<Turret> & enemyController::getTurrets() {
	return turrets;
}
