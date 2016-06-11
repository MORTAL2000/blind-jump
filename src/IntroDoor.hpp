
//  Pillar.hpp
//  Blind Jump
//
//  Created by Evan Bowman on 3/15/16.
//  Copyright © 2016 Evan Bowman. All rights reserved.
//

#pragma once
#ifndef IntroDoor_hpp
#define IntroDoor_hpp

#include "detailParent.hpp"
#include "inputController.hpp"
#include "spriteSheet.hpp"

class ScreenShakeController;

class IntroDoor : public detailParent {
	
	enum class State { dormant, opening, opened };
	
private:
	SpriteSheet<200, 95> doorSheet;
	uint8_t frameIndex;
	int32_t timer;
	State state;
	
public:
	void update(float, float, InputController * pInput, ScreenShakeController * pscr, const sf::Time & elapsedTime);
	IntroDoor(float, float, sf::Texture *, int, float, float);
	sf::Sprite* getSprite();
};

#endif /* Pillar_hpp */
