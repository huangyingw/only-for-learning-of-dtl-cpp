#include "../dtl/variables.hpp"
#include "../dtl/functors.hpp"
#include "../dtl/Sequence.hpp"
#include "../dtl/Ses.hpp"
#include "../dtl/Diff.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

using dtl::Diff;

int main(int argc, char *argv[]){
    string str1, str2;
    Diff< char, std::string> d("abcefghijk", "abdefghijk");
    //d.onOnlyEditDistance();
    d.compose();
    d.composeUnifiedHunks();
    d.printUnifiedFormat();
    return 0;
}
