
/** @file		logic.h
 *	@brief		Handles the logic workers message handling
 *	@details	
 *	@author		[Scott R Howell (Bombshell)](http://scottrhowell.com/) [\@GIT](https://github.com/thebombshell)
 *	@date		15th July 2018
 */

#ifndef LOGIC_H
#define LOGIC_H

#include "smol.h"

void logic_callback(const smol_message* t_message);

#endif