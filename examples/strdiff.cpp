#include <dtl/dtl.hpp>
#include "common.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using dtl::Diff;

int main(int argc, char *argv[]){
    string A("fdadkjfeijdkfsj");
    string B("fdadkjffdkjkewfjdksj");
    
    Diff< char, string> d(A, B);
    d.compose();
    
    d.composeUnifiedHunks();
    d.printUnifiedFormat();
    return 0;
}
