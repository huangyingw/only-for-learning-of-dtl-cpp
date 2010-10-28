#include "../dtl/variables.hpp"
#include "../dtl/functors.hpp"
#include "../dtl/Sequence.hpp"
#include "../dtl/Ses.hpp"
#include "../dtl/Diff.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

using dtl::Diff;

int main(int argc, char *argv[]){
    string str1, str2;
    Diff< char, std::string> d("11111111abcefghijk11111111", "11111111abdefghijk11111111");
    d.compose();
    d.composeUnifiedHunks();
    d.printUnifiedFormat();
    return 0;
}
