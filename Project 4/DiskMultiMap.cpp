//
//  DiskMultiMap.cpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#include "DiskMultiMap.h"
#include "MultiMapTuple.h"
#include <functional>         // include this for the hash function
#include <string>             // include this for strcpy()

// ITERATOR IMPLEMENTATION =========================================================================================

DiskMultiMap::Iterator::Iterator()
{
    m_valid = false;
}

DiskMultiMap::Iterator::Iterator(bool valid, BinaryFile::Offset current)
{
    m_valid = valid;
    m_current = current;
}

bool DiskMultiMap::Iterator::isValid() const
{
    return m_valid;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    if (isValid())
    {
//        ++ operator MUST advance the Iterator
//        to the next association in the DiskMultiMap with the same key as the association the
//        Iterator currently points to, if there is one; if there is no next association with the same
//        key, then the ++ operator MUST change the Iterator’s state to invalid.
    }
    
    return *this;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    MultiMapTuple tuple;
    if (!isValid())  // iterator is invalid
    {
        tuple.key = "";
        tuple.value = "";
        tuple.context = "";
    }
    else
    {
//        the key, value and context strings of the MultiMapTuple
//        returned have values equal to the key, value, and context components of the association
//        the Iterator points to.
    }
    
    return tuple;
}

// DISKMULTIMAP IMPLEMENTATION =====================================================================================

DiskMultiMap::DiskMultiMap()
{
    // must initialize the object???
}

DiskMultiMap::~DiskMultiMap()
{
    // must close the diskfile associated with the hashtable
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
//    If your DiskMultiMap object already has been used to open or create a disk-based hash
//    table, and the createNew() method is called, your method MUST first close the currently open
//    data file, and then proceed with creating the newly specified data file.
    if (m_file.isOpen())
        m_file.close();
    
    m_file.createNew(filename);   // make a new disk file for the hashtable
    
    // initialize the header
    m_header.m_numBuckets = numBuckets;
    m_header.m_end = sizeof(Header);
    m_header.m_listOfFreeSpots = -1;   // -1 acts like a nullptr (indicates that there is no previously deleted space to use)
    
    // write default empty buckets
    Bucket emptyBucket;
    emptyBucket.m_node = -1;
    emptyBucket.m_numNodes = 0;
    
    // initialize the "array" that contains offsets to each list
    for (int i = 0; i < numBuckets; i++)
    {
        m_file.write(emptyBucket, m_header.m_end);   // initialize the "array" with default values
        m_header.m_end += sizeof(Bucket);  // move the end pointer over
    }
    
    m_header.m_startOfNodes = m_header.m_end;
    m_file.write(m_header, 0);
    
    // garbage
    return true;
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
//    If your DiskMultiMap object already has been used to open or create a disk-based hash
//    table, and the openExisting() method is called, your method MUST first close the
//    currently-open data file, and then proceed with opening the newly specified data file
    
    if (m_file.openExisting(filename))
        return true;
    else
    return false;
}

void DiskMultiMap::close()
{
    // is this correct at all??? WTF AM I DOING
    m_file.close();
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    cout << "preinsertion length: " << m_file.fileLength() << endl;
    
    
    // invalid size for input
    if (key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    
    // hash the key to find what bucket to put it in
    std::hash<std::string> str_hash;
    unsigned int hashValue = str_hash(key);
    unsigned int bucket = hashValue % m_header.m_numBuckets;
    
    // initialize a new node
    Node newNode;
    strcpy(newNode.m_value, value.c_str());
    strcpy(newNode.m_context, context.c_str());
    
    // find the data inside the correct bucket
    Bucket dataGetter;
    BinaryFile::Offset posOfBucket = sizeof(Header) + sizeof(Bucket)*bucket;
    m_file.read(dataGetter, posOfBucket);
    newNode.m_next = dataGetter.m_node;  // link the new node to the front of the list in the bucket
    
    // TO DO: check to see if there is a spot already allocated in memory where you can put the node
    if (m_header.m_listOfFreeSpots != -1)
    {
        
    }
    
    // no previously allocated spots that you can put the node
    else
    {
        // put the new node in the end of the file
        m_file.write(newNode, m_header.m_end);   // put the node at the end of the file
        dataGetter.m_node = m_header.m_end; // reinitialize bucket to point to new front of the list
        dataGetter.m_numNodes++;  // increment # of nodes in list
        m_file.write(dataGetter, posOfBucket);
        // must change the header since the end of the file is somewhere new
        Header tempHeader = m_header;
        tempHeader.m_end = m_file.fileLength();
        m_header.m_end = tempHeader.m_end;  // reinitialize the header to contain the correct pointer to the end
        m_file.write(tempHeader, 0);
    }
    
    // TESTING
    cout << "inserted and here is the new length: " << m_file.fileLength() << endl;
    
    
    return true;
}

//DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
//{
//    
//}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    
    // garbage
    return 0;
}

// TESTING CODE BELOW

void DiskMultiMap::printAll()
{
    Node tempNode;
    BinaryFile::Offset iterator = m_header.m_startOfNodes;
    while( iterator < m_file.fileLength())
    {
        m_file.read(tempNode, iterator);
        cout << "Value: " << tempNode.m_value << " " << "Context: " << tempNode.m_context << endl;
        iterator += sizeof(Node);
    }
}
