/* ========================================
 *
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#ifndef MAP_H
#define MAP_H

#include "project.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int map[15][19] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
{1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
{1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
{1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1},
{1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1},
{1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
{1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
{1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

int food_list[5][2]= {{1,9},
{5,5},
{7,1},
{13,5},
{9,9}};

#endif /* MAP_H */