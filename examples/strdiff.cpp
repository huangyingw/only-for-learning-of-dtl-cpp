
#include <dtl/dtl.hpp>
#include "common.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

using dtl::Diff;

int main(int argc, char *argv[]){
    
    
    
    typedef char   elem;
    typedef string sequence;

    string A("fdadkjfeijdkfsj");
    string B("fdadkjffdkjkewfjdksj");
    
    Diff< char, string> d(A, B);
    //d.onOnlyEditDistance();
    d.compose();
    
    // editDistance
    cout << "editDistance:" << d.getEditDistance() << endl;
    
    // Shortest Edit Script
    cout << "UifiedFormat" << endl;
    d.composeUnifiedHunks();
    d.printUnifiedFormat();
    
    return 0;
}
