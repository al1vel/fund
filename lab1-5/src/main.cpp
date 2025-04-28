#include "vector.h"
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> vec(10);
    cout << vec.size() << endl;
    cout << vec.capacity() << endl;
    for (int x : vec) {
        cout << x << " ";
    }
    cout << endl;

    vector<int> a;
    auto it = a.begin();
    ++it;
    ++it;
    a.insert(it, 10);

    cout << a.capacity() << endl;
    cout << a.size() << endl;
}
