//
//  main.cpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#include <iostream>
#include "DiskMultiMap.h"

int main()
{
    DiskMultiMap myMap;
    myMap.createNew("Karen.dat", 5);
    myMap.printAll();
    myMap.insert("apple", "banana", "carrot");
    myMap.insert("durian", "eggplant", "fruit");
    myMap.insert("grape", "hawthorne", "igloo");
    myMap.printAll();
}