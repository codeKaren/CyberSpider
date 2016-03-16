//
//  DiskMultiMap.hpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        Iterator(bool valid, BinaryFile::Offset current, BinaryFile* bfile);  // can specify if an iterator is valid or not and where it points
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        // Your private member declarations will go here
        bool m_valid;
        BinaryFile::Offset m_current;
        BinaryFile* m_binaryFile;
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    
private:
    // Your private member declarations will go here
    
    struct Header
    {
        int m_numBuckets;    // number of buckets in the hash table
        BinaryFile::Offset m_end;  // points to the end of the file (so you can put more data there)
        BinaryFile::Offset m_startOfNodes;  // points to the part of the file past the buckets where the nodes start
        BinaryFile::Offset m_listOfFreeSpots;   // points to the first empty spot that's already been allocated (reuse memory)
    };
    
    struct Node
    {
        char m_key[121];
        char m_value[121];
        char m_context[121];
        bool isDeleted;
        BinaryFile::Offset m_next;  // points to the next node in the list
    };
    
    struct Bucket
    {
        BinaryFile::Offset m_node;  // points to the first node in the list
        int m_numNodes;   // the number of nodes in the list 
    };
    
    BinaryFile m_file;
    Header m_header;
    std::hash<std::string> m_stringHasher;
};

#endif // DISKMULTIMAP_H_