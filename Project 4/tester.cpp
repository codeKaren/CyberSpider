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
    myMap.createNew("Karen.dat", 100);
    myMap.insert("apple", "banana", "carrot");
    myMap.printAll();
    myMap.insert("durian", "eggplant", "fruit");
    myMap.insert("grape", "hawthorne", "igloo");
    myMap.insert("apple", "bread", "chocolate");
    myMap.insert("apple", "beef", "chicken");
    myMap.printAll();
}