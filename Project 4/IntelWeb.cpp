//
//  IntelWeb.cpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#include "IntelWeb.h"

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
    
    // garbage
    return false;
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators, unsigned int minPrevalenceToBeGood, std::vector<std::string>& badEntitiesFound, std::vector<InteractionTuple>& interactions)
{

    // garbage
    return false;
}

bool purge(const std::string& entity)
{
    
    // garbage
    return false;
}