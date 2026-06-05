#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main() {

   string str;

   cout << "enter input string: ";
   getline(cin, str);

   if (str.empty()) {
      cout << "empty input" << endl;
      return 0;
   }
   if (str.length() < 2) {
      cout << "short input" << endl;
      return 0;
   }

   cout << str << endl << str [0] << endl;

    if(str[0] == 41 && str[str.length() - 1] == 41 && str[str.length()/2] == 41) {
        abort();
    }
    else {
        char prev_num = 'x';
        for (size_t count = 0; count + 1 < str.length(); ++count) {
            char c = str[count];
            if(c >= 48 && c <= 57) {
                if(c == prev_num + 1) {
                    abort();
                }
                prev_num = c;
            } 
        }
    }
    
    return 0;
}
