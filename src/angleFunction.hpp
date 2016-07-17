//
//  angleFunction.hpp
//  Blind Jump
//
//  Created by Evan Bowman on 10/27/15.
//  Copyright © 2015 Evan Bowman. All rights reserved.
//

#pragma once
#ifndef angleFunction_hpp
#define angleFunction_hpp

#include <cmath>

//A function to calculate the angle between two objects
inline float angleFunction(float x, float y, float fixedX, float fixedY) {
	float angle = atan2(y - fixedY, x - fixedX);	//Rounding to zero, so there's no error in this cast
	return angle * (180 / 3.14);
}

#endif /* angleFunction_hpp */
