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

DiskMultiMap::Iterator::Iterator(bool valid, BinaryFile::Offset current, BinaryFile* bfile)
{
    m_valid = valid;
    m_current = current;
    m_binaryFile = bfile;
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

        Node tempNode, nextNode;
        m_binaryFile->read(tempNode, m_current);  // get the data inside the node
        string key = tempNode.m_key;
        BinaryFile::Offset nextNodeOffset = tempNode.m_next;
        while (nextNodeOffset != -1)
        {
            m_binaryFile->read(nextNode, nextNodeOffset);
            // check for collisions
            if (strcmp(nextNode.m_key,key.c_str()) == 0)   // found the next item with the same key
            {
                m_current = nextNodeOffset;
                return *this;
            }
            nextNodeOffset = nextNode.m_next;  // move over to next one
        }
        m_valid = false; // gone through all of the nodes and haven't found the correct item
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
        // how is the iterator supposed to figure out the key?? eg. how to unhash the bucket?
        Node tempNode;
        m_binaryFile->read(tempNode, m_current);  // get data inside the current node
        tuple.key = tempNode.m_key;
        tuple.value = tempNode.m_value;
        tuple.context = tempNode.m_context;
    }
    return tuple;
}

// DISKMULTIMAP IMPLEMENTATION =====================================================================================

DiskMultiMap::DiskMultiMap()
{ }

DiskMultiMap::~DiskMultiMap()
{
    // must close the diskfile associated with the hashtable
    close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
//    If your DiskMultiMap object already has been used to open or create a disk-based hash
//    table, and the createNew() method is called, your method MUST first close the currently open
//    data file, and then proceed with creating the newly specified data file.
    
    
    if (m_file.isOpen())
        m_file.close();
    
    if (! m_file.createNew(filename))   // make a new disk file for the hashtable
        return false;
        
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
    
    return true;
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
//    If your DiskMultiMap object already has been used to open or create a disk-based hash
//    table, and the openExisting() method is called, your method MUST first close the
//    currently-open data file, and then proceed with opening the newly specified data file
    
    if (m_file.openExisting(filename))
    {
        m_file.read(m_header, 0);
        return true;
    }
    
    else
    return false;
}

void DiskMultiMap::close()
{
    m_file.close();
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    // invalid size for input
    if (key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    
    // hash the key to find what bucket to put it in
    unsigned int hashValue = m_stringHasher(key);
    unsigned int bucket = hashValue % m_header.m_numBuckets;
    // testing
    
    // initialize a new node
    Node newNode;
    strcpy(newNode.m_key, key.c_str());
    strcpy(newNode.m_value, value.c_str());
    strcpy(newNode.m_context, context.c_str());
    newNode.isDeleted = false;
    
    // find the data inside the correct bucket
    Bucket dataGetter;
    BinaryFile::Offset posOfBucket = sizeof(Header) + sizeof(Bucket)*bucket;
    m_file.read(dataGetter, posOfBucket);
    newNode.m_next = dataGetter.m_node;  // link the new node to the front of the list in the bucket
    
    // TO DO: check to see if there is a spot already allocated in memory where you can put the node
    if (m_header.m_listOfFreeSpots != -1)
    {
        // get the data from the currently empty spot for the position of the next empty spot
        Node tempNode;
        m_file.read(tempNode, m_header.m_listOfFreeSpots);
        BinaryFile::Offset nextEmptySpot = tempNode.m_next;  // get the position of the next empty spot
        // put the new node in the free slot
        m_file.write(newNode, m_header.m_listOfFreeSpots);
        dataGetter.m_node = m_header.m_listOfFreeSpots;   // make the bucket list's first element be the new node
        dataGetter.m_numNodes++;   // incremement # nodes in list
        m_file.write(dataGetter, posOfBucket);
        // change the header since the first element of the list of empty spots is somewhere new
        m_header.m_listOfFreeSpots = nextEmptySpot;  // initialize the head pointer of free spots
        m_file.write(m_header, 0);
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
        
    return true;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
    unsigned int hashValue = m_stringHasher(key);
    unsigned int bucket = hashValue % m_header.m_numBuckets;
    BinaryFile::Offset posOfBucket = sizeof(Header) + sizeof(Bucket)*bucket;   // find the bucket
    
    Bucket dataGetter;  // get data from the bucket
    m_file.read(dataGetter, posOfBucket);
    
    if (dataGetter.m_numNodes == 0)  // no nodes
    {
        Iterator invalidIt;   // the default constructor creates an invalid iterator
        return invalidIt;
    }
    else
    {
        // check for collisions
        int numNodes = dataGetter.m_numNodes;
        BinaryFile::Offset currentNode = dataGetter.m_node;
        Node tempNode;
        while (numNodes > 0)
        {
            m_file.read(tempNode, currentNode);
            if (strcmp(tempNode.m_key, key.c_str()) == 0)    // found the correct node
            {
                Iterator validIt(true, currentNode, &m_file);  // return an iterator to the correct node
                return validIt;
            }
            else
            {
                numNodes--;
                currentNode = tempNode.m_next;
            }
        }
        Iterator it;   // couldn't find the ndoe, so just return an invalid node
        return it;
    }
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    unsigned int hashValue = m_stringHasher(key);
    unsigned int bucket = hashValue % m_header.m_numBuckets;  // find which bucket to search in
    
    Bucket dataGetter;
    BinaryFile::Offset posOfBucket = sizeof(Header) + sizeof(Bucket)*bucket;
    m_file.read(dataGetter, posOfBucket);   // get the data inside of the bucket corresponding to the key
    
    bool haveSetBucketHead = false;  // check to see if the bucket's head pointer has been reset
    int numErased = 0;
    BinaryFile::Offset currentNode = dataGetter.m_node;   // set iterator to the first element in the list
    BinaryFile::Offset previousNode = -1;   // no previous node in the beginning
    int initialNumNodes = dataGetter.m_numNodes;
    
    string nodeKey;                // variables to store data in
    string nodeValue;
    string nodeContext;
    BinaryFile::Offset nodeNext;
    Bucket finalBucket;            // initialize this to hold the values of the bucket after deleting everything
    Node tempNode;    // contains the current node
    Node prevNode;   // contains the previous node (or the current node if at the very beginning)
    
    for (int i = 0; i < initialNumNodes; i++)  // look through all of the nodes in the list in that bucket
    {
        // delete the nodes starting from the first one and then keep deleting until you hit the first one you don't delete
        // once you hit that one, set the bucket's m_node variable to that
        // check outside the loop if the bucket's numNodes == 0, and if it does, just set the bucket's m_node to -1
        m_file.read(tempNode, currentNode);  // get all of the data from the current node we're examining
        nodeKey = tempNode.m_key;
        nodeValue = tempNode.m_value;
        nodeContext = tempNode.m_context;
        nodeNext = tempNode.m_next;
        if (previousNode != -1)
            m_file.read(prevNode, previousNode);   // get the data from the previous node if valid previous node
        
        if (nodeKey == key && nodeValue == value && nodeContext == context)  // found a match
        {
            tempNode.isDeleted = true;
            Header tempHeader;
            m_file.read(tempHeader, 0);
            tempNode.m_next = tempHeader.m_listOfFreeSpots;  // link the new empty spot to the old list of empty spots
            tempHeader.m_listOfFreeSpots = currentNode;   // edit the header so it points to the new empty spot
            m_header = tempHeader;   // change the actual header member variable too
            m_file.write(tempNode, currentNode);
            m_file.write(tempHeader, 0);
            numErased++;
        }
        else
        {
            if (haveSetBucketHead == false)  // didn't find a node and we haven't initialized the bucket's m_node yet
            {
                finalBucket.m_node = currentNode;   // initialize the bucket's m_node variable, initialize m_numNodes later
                haveSetBucketHead = true;
            }
            if (previousNode != -1)
            {
                prevNode.m_next = currentNode;   // link the previous node to the current node
                m_file.write(prevNode, previousNode);
            }
            previousNode = currentNode;  // move the previous node
        }
        
        currentNode = nodeNext;  // move the iterator over
    }

    finalBucket.m_numNodes = initialNumNodes - numErased;
    
    // in case we deleted everything in the list and so, haven't set the bucket's m_node variable to a new value yet
    if (finalBucket.m_numNodes == 0)
        finalBucket.m_node = -1;       // means there's nothing in the bucket's list
    // since we haven't deleted everything, there must be a last node in the list
    else
    {
        Node lastNode;
        m_file.read(lastNode, previousNode);
        lastNode.m_next = -1;
        m_file.write(lastNode, previousNode);
    }
    
    m_file.write(finalBucket, posOfBucket);   // write the new values of the bucket onto disk
    
    // garbage
    return numErased;
}