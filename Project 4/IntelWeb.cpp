//
//  IntelWeb.cpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#include "IntelWeb.h"
#include "DiskMultiMap.h"
#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>

IntelWeb::IntelWeb()
{
    
}

IntelWeb::~IntelWeb()
{
    close();
}

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
//    The createNew() method MUST close any DiskMultiMaps this IntelWeb object might
//    have created or opened before creating any new ones. If successful, createNew() MUST
//    have overwritten any existing DiskMultiMap disk files with new, empty versions.

    close();
    
    m_filePrefix = filePrefix;
    
    // since the load factor is ~0.75, get the number of buckets you should have
    unsigned int numBuckets = maxDataItems * 4/3;
    if (m_forwardMap.createNew(filePrefix+"forward", numBuckets) && m_reverseMap.createNew(filePrefix+"reverse", numBuckets))
        return true;  // successfully created the diskfiles
    else
    {
        close();
        return false;
    }
}

bool IntelWeb::openExisting(const std::string& filePrefix)
{
    close();
    
    m_filePrefix = filePrefix;
    
    if (m_forwardMap.openExisting(m_filePrefix+"forward") && m_reverseMap.openExisting(m_filePrefix+"reverse"))
        return true;
    else
    {
        close();
        return false;
    }
}

void IntelWeb::close()
{
    m_forwardMap.close();
    m_reverseMap.close();
}

bool IntelWeb::ingest(const std::string& telemetryFile)
{
    // Open the file for input
    ifstream inf(telemetryFile);
    // Test for failure to open
    if ( ! inf)
    {
        cout << "Cannot open telemetry file!" << endl;
        return 1;
    }
    string key;
    string value;
    string context;
    string wholeLine;  // hold the data of the whole line
    // Read each line.  The return value of getline is treated
		  // as true if a line was read, false otherwise (e.g., because
		  // the end of the file was reached).
    while (getline(inf, wholeLine))
    {
        // To extract the information from the line, we'll
        // create an input stringstream from it, which acts
        // like a source of input for operator>>
        istringstream iss(wholeLine);
        // The return value of operator>> acts like false
        // if we can't extract a word followed by a number
        if ( ! (iss >> context >> key >> value) )
        {
            cout << "Ignoring badly-formatted input line: " << wholeLine << endl;
            continue;
        }
        // If we want to be sure there are no other non-whitespace
        // characters on the line, we can try to continue reading
        // from the stringstream; if it succeeds, extra stuff
        // is after the double.
        char dummy;
        if (iss >> dummy) // succeeds if there a non-whitespace char
            cout << "Ignoring extra data in line: " << wholeLine << endl;
        
        // Add data to diskMultiMaps
        m_forwardMap.insert(key, value, context);
        m_reverseMap.insert(value, key, context);
    }
    
    // WHAT IS INGEST SUPPOSED TO RETURN????
    return true;
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators, unsigned int minPrevalenceToBeGood, std::vector<std::string>& badEntitiesFound, std::vector<InteractionTuple>& interactions)
{
    // make sure that the badEntitiesFound vector and the interactions vector starts off empty
    badEntitiesFound.clear();
    interactions.clear();
    
    set<std::string> possibleBadThings;             // use this to store the bad entities
    set<InteractionTuple> setOfBadInteractions;    // use this to store the bad interactions
    
    DiskMultiMap::Iterator forwardSearcher, reverseSearcher;
    MultiMapTuple currentTuple;
    
    for (int i = 0; i < indicators.size(); i++)
    {
        string currentBadThing = indicators[i];
        forwardSearcher = m_forwardMap.search(currentBadThing);
        reverseSearcher = m_reverseMap.search(currentBadThing);
        while (forwardSearcher.isValid())
        {
            currentTuple = *forwardSearcher;
            possibleBadThings.insert(currentTuple.key);    // insert the key since it was actually found in the telemetry
            possibleBadThings.insert(currentTuple.value);   // insert the thing associated with the bad thing
            ++forwardSearcher;
        }
        while (reverseSearcher.isValid())
        {
            currentTuple = *reverseSearcher;
            possibleBadThings.insert(currentTuple.key);
            possibleBadThings.insert(currentTuple.value);
            ++reverseSearcher;
        }
    }
    
    // our set now contains all possible bad things (things that were in interactions wtih the known bad things)
    // check to see if the items are legit (less # than min prevalence) and if they are, add them to the vector
    set<std::string>::iterator curr = possibleBadThings.begin();
    int count = 0;   // counts how many times an item appears in the telemetry logs
    while (curr != possibleBadThings.end())
    {
        forwardSearcher = m_forwardMap.search(*curr);
        reverseSearcher = m_reverseMap.search(*curr);
        while ((forwardSearcher.isValid() || reverseSearcher.isValid()) && count < minPrevalenceToBeGood)
        {
            if (forwardSearcher.isValid())  // item is found in the forward map
            {
                count++;
                ++forwardSearcher;
            }
            if (reverseSearcher.isValid())  // item is found in the reverse map
            {
                count++;
                ++reverseSearcher;
            }
        }
        if (count >= minPrevalenceToBeGood)
        {
            curr = possibleBadThings.erase(curr);  // item is good, so delete it from the set of bad things
            count = 0;  // reinitialize the count for the next element in the possible bads set
        }
        else   // item is bad so add it to the bad interactions vector
        {
            forwardSearcher = m_forwardMap.search(*curr);
            reverseSearcher = m_reverseMap.search(*curr);
            while (forwardSearcher.isValid())
            {
                currentTuple = *forwardSearcher;
                InteractionTuple currInteraction(currentTuple.key, currentTuple.value, currentTuple.context);
                setOfBadInteractions.insert(currInteraction);
                ++forwardSearcher;
            }
            while (reverseSearcher.isValid())
            {
                currentTuple = *reverseSearcher;
                InteractionTuple currInteraction(currentTuple.value, currentTuple.key, currentTuple.context);
                setOfBadInteractions.insert(currInteraction);
                ++reverseSearcher;
            }
            curr++;  // increment the iterator to point to the next bad item to check inside the telemetry
            count = 0;  // reinitialize the count for the next element in the possible bads set
        }
    }
    
    // now set only contains confirmed bad things and the interaction only contains confirmed bad interactions

    // define operator less than
    // stick the things from the set into the vector
    
    curr = possibleBadThings.begin();  // start at the beginning of the set
    for (int i = 0; i < possibleBadThings.size(); i++)
    {
        badEntitiesFound.push_back(*curr);  // insert all of the bad strings
        curr++;
    }

    set<InteractionTuple>::iterator currTupleIt = setOfBadInteractions.begin();
    for (int i = 0; i < setOfBadInteractions.size(); i++)
    {
        interactions.push_back(*currTupleIt);
        currTupleIt++;
    }
    
    return badEntitiesFound.size();   // returns the number of bad entities found in the telemetry logs
}

bool IntelWeb::purge(const std::string& entity)
{
    // delete all instances by searching for that entity as the key in both diskMultiMaps, saving the values, and then deleting them from both maps
    
    // search for entity as the key inside m_frontMap first
    
    MultiMapTuple currentTuple;
    bool erasedSomething = false;
    
    DiskMultiMap::Iterator forwardSearcher = m_forwardMap.search(entity);
    while (forwardSearcher.isValid())
    {
        currentTuple = *forwardSearcher;
        if (m_forwardMap.erase(currentTuple.key, currentTuple.value, currentTuple.context))
            erasedSomething = true;
        if (m_reverseMap.erase(currentTuple.value, currentTuple.key, currentTuple.context))
            erasedSomething = true;
        ++forwardSearcher;
    }

    // search for entity as the key inside m_reverseMap
    
    DiskMultiMap::Iterator reverseSearcher = m_reverseMap.search(entity);
    while (reverseSearcher.isValid())
    {
        currentTuple = *reverseSearcher;
        if (m_reverseMap.erase(currentTuple.key, currentTuple.value, currentTuple.context))
            erasedSomething = true;
        if (m_forwardMap.erase(currentTuple.value, currentTuple.key, currentTuple.context))
            erasedSomething = true;
        ++reverseSearcher;
    }
    
    return erasedSomething;
}

// OPERATOR LESS THAN FOR INTERACTION TUPLES
bool operator<(const InteractionTuple& a, const InteractionTuple& b)
{
    if (a.context < b.context)
        return true;
    if (a.context > b.context)
        return false;
    if (a.from < b.from)
        return true;
    if (a.from > b.from)
        return false;
    if (a.to < b.to)
        return true;
    if (a.to > b.to)
        return false;
    return false;       // since everything is equal, then just return false
}
