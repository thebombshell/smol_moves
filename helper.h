
/** @file		helper.h
 *	@brief		Handles the helper workers message handling
 *	@details	
 *	@author		[Scott R Howell (Bombshell)](http://scottrhowell.com/) [\@GIT](https://github.com/thebombshell)
 *	@date		15th July 2018
 */

#ifndef HELPER_H
#define HELPER_H

#include "smol.h"

void helper_callback(const smol_message* t_message);

#endif