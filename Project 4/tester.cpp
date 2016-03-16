//
//  main.cpp
//  Project 4
//
//  Created by Karen Li on 2016-03-05.
//  Copyright © 2016 Karen Li. All rights reserved.
//

#include <iostream>
#include "DiskMultiMap.h"

// Personal test of insertion
//int main()
//{
//    DiskMultiMap myMap;
//    myMap.createNew("Karen.dat", 3);
//    myMap.insert("apple", "banana", "carrot");
//    cout << "first print" << endl;
//    myMap.printAll();
//    myMap.insert("durian", "eggplant", "fruit");
//    myMap.insert("apple", "banana", "carrot");
//    myMap.insert("grape", "hawthorne", "igloo");
//    myMap.insert("apple", "bread", "chocolate");
//    myMap.insert("apple", "beef", "chicken");
//    myMap.insert("apple", "banana", "carrot");
//    myMap.insert("apple", "banana", "carrot");
//    cout << "second print" << endl;
//    myMap.printAll();
//    if (myMap.erase("apple", "beef", "chicken") == 1)
//        cout << "erased beef correctly" << endl;
//    if (myMap.erase("apple", "banana", "carrot") == 4)
//        cout << "erased banana correctly" << endl;
//    cout << "final (post deletion) print" << endl;
//    myMap.printAll();
//}

// Personal test of deletion mostly (seeing if actually using memory efficiently)
//int main() {
//    DiskMultiMap myMap;
//    myMap.createNew("Karen.dat", 100);
//    myMap.insert("a", "b", "c");
//    cout << 1 << endl;
//    myMap.printAll();
//    myMap.insert("a", "d", "e");
//    cout << 2 << endl;
//    myMap.printAll();
//    myMap.erase("a", "d", "e");
//    cout << 3 << endl;
//    myMap.printAll();
//    myMap.insert("a", "f", "g");
//    cout << 4 << endl;
//    myMap.printAll();
//    myMap.insert("a", "h", "i");
//    cout << 5 << endl;
//    myMap.printAll();
//    myMap.insert("e", "i", "j");  // should increase size
//    cout << 6 << endl;
//    myMap.printAll();
//    myMap.erase("e", "e", "e");  // shouldn't do anything
//    cout << 7 << endl;
//    myMap.printAll();
//}


// Spec test 1
//int main()
//{
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    x.printAll();
//    DiskMultiMap::Iterator it = x.search("hmm.exe");
//    if (it.isValid())
//    {
//        cout << "I found at least 1 item with a key of hmm.exe\n";
//        do
//        {
//            MultiMapTuple m = *it; // get the association
//            cout << "The key is: " << m.key << endl;
//            cout << "The value is: " << m.value << endl;
//            cout << "The context is: " << m.context << endl;
//            cout << endl;
//            ++it; // advance iterator to the next matching item
//        } while (it.isValid());
//    }
//}

// Spec test 2
//int main()
//{
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    DiskMultiMap::Iterator it = x.search("goober.exe");
//    if ( ! it.isValid())
//        cout << "I couldn’t find goober.exe\n";
//}

// Spec test 3
//int main()
//{
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    // line 1
//    if (x.erase("hmm.exe", "pfft.exe", "m52902") == 2)
//        cout << "Just erased 2 items from the table!\n";
//    // line 2
//    if (x.erase("hmm.exe", "pfft.exe", "m10001") > 0)
//        cout << "Just erased at least 1 item from the table!\n";
//    // line 3
//    if (x.erase("blah.exe", "bletch.exe", "m66666") == 0)
//        cout << "I didn't erase this item cause it wasn't there\n";
//}