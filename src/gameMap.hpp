//
//  mapInit.hpp
//  Blind Jump
//
//  Created by Evan Bowman on 10/15/15.
//  Copyright © 2015 Evan Bowman. All rights reserved.
//

#pragma once
#ifndef gameMap_hpp
#define gameMap_hpp

#include <stdio.h>
#include "SFML/Graphics.hpp"
#include "player.hpp"
#include "backgroundHandler.hpp"
#include "effectsController.hpp"
#include "detailController.hpp"
#include "tileController.hpp"
#include "fontController.hpp"
#include "userInterface.hpp"
#include "enemyController.hpp"
#include "SFML/Audio.hpp"
#include "screenShakeController.hpp"
#include "soundController.hpp"
#include "initLoot.hpp"
#include "inputController.hpp"
#include "GradientCircle.hpp"
#include "RenderType.hpp"

class GameMap {
	
private:
	// Instaintiate a player object
	Player player;
	
	// Set the world type
	short worldType;
	
	// Declare a user Interface object
	userInterface UI;
	
	// Define an object to control sounds
	SoundController sndCtrl;
	
	// Store the current level
	int level;
	
	// The engine creates the illusion of motion by offsetting the background during key presses
	float xOffset;
	float yOffset;
	
	// Controls input from the keyboard and joystick
	InputController *pInput;
	
	sf::Texture vignetteTexture;
	sf::Sprite vignetteSprite;
	
	char itemArray[48][3];
	
	// Add a detail controller
	detailController details;
	
	// Instantiate an effects controller to handle drawing sfx
	effectsController effects;
	
	// Instantiate a background controller
	backgroundHandler bkg;
	
	sf::Texture blurred;
	sf::Sprite finalSprite;
	
	// A boolean variable that is true when the player is nearby a teleporter
	bool teleporterCond;
	
	sf::Texture vignetteShadowTxtr;
	sf::Sprite vignetteShadowSpr;
	
	// A vector to hold text captions for details
	std::vector<sf::Text*> captions;
	
	// Number of enemies on the map
	unsigned char turretCount;
	unsigned char dasherCount;
	unsigned char scootCount;
	
	// Create a GL rectangle primitive for the teleporter effect
	sf::RectangleShape teleporterBeam;
	sf::RectangleShape entryBeam;
	GradientCircle gradientCircle;
	
	// For the beam to cast a glow to the map
	sf::Texture beamGlowTxr;
	sf::Sprite beamGlowSpr;
	
	// State variables for the teleporter effects at the end of the levels
	bool animationBegin;
	bool beamExpanding;
	bool dispEntryBeam;
	bool transitionIn;
	
	sf::Shader colorShader, blurShader, desaturateShader;
	
	sf::View worldView, hudView;
	
	// RenderTexture and shapes for lighting effects
	sf::RenderTexture lightingMap;
	sf::RectangleShape shadowShape;
	
	sf::RenderTexture target, secondPass, thirdPass;
	
	// Create a font controller
	FontController* pFonts;
	
	// Locations to place lights
	std::vector<Coordinate> lightPositions;
	
	sf::RectangleShape transitionShape;
	
	sf::Texture titleTxtr;
	sf::Sprite titleSpr;
	
	sf::Color objectShadeColor;
	
	// Locations to place pillars
	std::vector<Coordinate> rockPositions;
	
	// Create a vector of pairs with enemy index and placement probability for enemy creation
	std::vector<std::pair<int, int>> enemySelectVec;
	int idealLvs[4] = {4/*Scoot*/, 5/*Critter Swarms*/, 28/*Dasher*/, 34/*Turrets*/};
	
	// Stack of sprites with y-position and height, for z-ordering
	std::vector<std::tuple<sf::Sprite, float, Rendertype, float>> gameObjects;
	std::vector<std::tuple<sf::Sprite, float, Rendertype, float>> gameShadows;
	
	// Create a screenshake controller for that effect
	ScreenShakeController ssc;
	
	bool transitioning;
	short int transitionDelay;
	
public:
	GameMap(float, float, sf::Texture*, InputController*, FontController*);
	//Pass in the render window and draw sprites to it
	void update(sf::RenderWindow&, sf::Time&);
	// The opening map does not follow the procedural generation approach, have a separate function for this
	void updateLv1(sf::RenderWindow&);
	Player getPlayer();
	detailController getDetails();
	void Reset();
	bool getTeleporterCond();
	std::vector<std::pair<int, int>>* getEnemySelectVec();
	int* getIdealLvs();
	int getLevel();
 
	
	// Store the window width and window height for map resets
	float windowW;
	float windowH;
	
	// Add an enemy controller to take care of moving, updating, and drawing the enemies
	enemyController en;
	
	// Instantiate a tile controller for the drawing the background tiles to the window
	tileController tiles;
};

// The first room is not procedurally generated so the positions of the walls need to be hard coded
const static std::array<std::pair<float, float>, 59> global_levelZeroWalls {
	{std::make_pair(-20, 500),
		std::make_pair(-20, 526),
		std::make_pair(-20, 474),
		std::make_pair(-20, 448),
		std::make_pair(-20, 422),
		std::make_pair(-20, 396),
		std::make_pair(-20, 370),
		std::make_pair(-20, 552),
		std::make_pair(-20, 578),
		
		std::make_pair(196, 500),
		std::make_pair(196, 526),
		std::make_pair(196, 474),
		std::make_pair(196, 448),
		std::make_pair(196, 422),
		std::make_pair(196, 396),
		std::make_pair(196, 370),
		std::make_pair(196, 552),
		std::make_pair(196, 578),
		
		std::make_pair(12, 604),
		std::make_pair(44, 604),
		std::make_pair(76, 604),
		std::make_pair(108, 604),
		std::make_pair(140, 604),
		std::make_pair(172, 604),
		
		std::make_pair(12, 370),
		std::make_pair(34, 370),
		std::make_pair(120, 370),
		std::make_pair(152, 370),
		std::make_pair(184, 370),
		
		std::make_pair(34, 344),
		std::make_pair(120, 344),
		std::make_pair(34, 318),
		std::make_pair(120, 318),
		std::make_pair(34, 292),
		std::make_pair(120, 292),
		std::make_pair(34, 266),
		std::make_pair(120, 266),
		
		std::make_pair(12, 266),
		std::make_pair(-20, 266),
		std::make_pair(152, 266),
		std::make_pair(-20, 240),
		std::make_pair(172, 240),
		std::make_pair(-20, 214),
		std::make_pair(172, 214),
		std::make_pair(-20, 188),
		std::make_pair(172, 188),
		std::make_pair(-20, 162),
		std::make_pair(172, 162),
		std::make_pair(-20, 136),
		std::make_pair(172, 136),
		std::make_pair(-20, 110),
		std::make_pair(172, 110),
		std::make_pair(-20, 84),
		std::make_pair(172, 84),
		
		std::make_pair(12, 58),
		std::make_pair(44, 58),
		std::make_pair(76, 58),
		std::make_pair(108, 58),
		std::make_pair(140, 58)}
};

#endif /* mapInit_hpp */