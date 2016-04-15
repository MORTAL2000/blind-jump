//
//  Hearts.cpp
//  Blind Jump
//
//  Created by Evan Bowman on 4/2/16.
//  Copyright © 2016 Evan Bowman. All rights reserved.
//

#include "Powerup.hpp"
#include <cmath>

#define PI 3.1415926535

Powerup::Powerup(sf::Sprite* inpSpr, sf::Sprite glow, float xInit, float yInit) {
    this->xInit = xInit;
    this->yInit = yInit;
    this->glow = glow;
    spr = *inpSpr;
    clock.restart();
}

void Powerup::update(float xoffset, float yoffset) {
    xPos = xInit + xoffset;
    yPos = yInit + yoffset;
}

sf::Sprite Powerup::getSprite() {
    spr.setPosition(xPos, yPos + (3 * sinf(2 * PI * 0.001 * clock.getElapsedTime().asMilliseconds() + 180)));
    return spr;
}

sf::Sprite* Powerup::getGlow() {
    glow.setPosition(xPos, yPos + 8);
    return &glow;
}

bool Powerup::getKillFlag() {
    return killFlag;
}

void Powerup::setKillFlag(bool killFlag) {
    this->killFlag = killFlag;
}

float Powerup::getXpos() {
    return xPos;
}

float Powerup::getYpos() {
    return yPos;
}