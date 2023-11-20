#include <iostream>
#include <bitset>
#include <iomanip>
#include <vector>
//#include <NTL.lib>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h> 
#include <algorithm>

NTL_CLIENT

using namespace std;
using namespace NTL;

int BerlekampMassey(vector<byte> s, int n)
{
    //int s[] = { 1,1,0,0,0,0,1,0,1,0,0,1,1 }; // bits of the stream to analyse
    //#define n (sizeof(s)/sizeof(*s))     // how many bits there are
    int b[5], c[5], t[5], d, j, N, L = 0, m = -1;
    for (j = n; --j > 0;)
        b[j] = c[j] = 0;
    b[0] = c[0] = 1;
    for (N = 0; N < n; ++N) {                 // For N=0 step 1 while N<n
        d = (int)s[N];                          // first term  of discrepancy
        for (j = L; j > 0; --j)               // other terms of discrepancy
            d ^= c[j] & (int)s[N - j];
        if (d != 0) {                      // non-zero discrepancy
            for (j = n; --j >= 0;)            // copy c to t
                t[j] = c[j];
            for (j = n - N + m; --j >= 0;)        // XOR b (reversed) into c
                c[N - m + j] ^= b[j];
            if (L + L <= N) {                 // if L<=N/2
                L = N + 1 - L;
                m = N;
                for (j = n; --j >= 0;)        // copy t to b
                    b[j] = t[j];
            }
        }
    }
    return L;
}


void printTable(int n, GF2X& poly)
{
    GF2X seq = GF2X();
    int minComplexity = n + 1;
    int maxComplexity = -1;
    

    cout << "  i    Sequence    Minimal Polynomial    Linear Comple5xity    Spherical Complexity" << endl;

    // Generate all possible sequences of length n
    for (int i = 2; i < (1 << n); i++) {
        // Convert the integer i to a binary string of length n
        string binaryString = bitset<20>(i).to_string().substr(20 - n);
        string binaryStringCopy = binaryString;
        unsigned char* p = (unsigned char*)binaryString.c_str();

        // Convert the binary string to a GF2X polynomial
        BytesFromGF2X(p, seq, n);
        

        // Create sequence (vector) of bytes from binary string
        vector<byte> sequence;
        sequence.reserve(std::size(binaryStringCopy));
        std::transform(std::begin(binaryStringCopy), std::end(binaryStringCopy), std::back_inserter(sequence), [](char c) {
            return std::byte(c);
            });


        // Compute the linear complexity of the sequence
        int linearComplexity = BerlekampMassey(sequence, std::size(binaryStringCopy));

        // Compute the spherical complexity of the sequence with the given linear complexity
        int sphericalComplexity = n - linearComplexity + 1;

        // Check if the current sequence has a lower linear complexity than the previous ones
        if (linearComplexity < minComplexity) {
            minComplexity = linearComplexity;
            poly = GF2X();
            const vec_GF2 a = to_vec_GF2(poly);
            //MinPolySeq(poly, seq, linearComplexity);
        }

        // Check if the current sequence has a higher spherical complexity than the previous ones
        if (sphericalComplexity > maxComplexity) {
            maxComplexity = sphericalComplexity;
        }

        // Print the current sequence and its properties
        cout << setw(3) << i << "    " << binaryStringCopy << "    ";
        cout << poly << "    " << linearComplexity << "    " << sphericalComplexity << endl;
    }

    cout << endl << "Minimum linear complexity: " << minComplexity << endl;
    cout << "Maximum spherical complexity: " << maxComplexity << endl;
}

int main()
{
    int n;
    cout << "Please insert the size of sequence: ";
    cin >> n;

    if (n <= 0 || n > 20) {
        cout << "Invalid input. The size of sequence must be between 1 and 20." << endl;
        return 0;
    }

    GF2X poly;

    if (n == 0) {
        string binaryString;
        cout << "Please insert your own sequence: ";
        cin >> binaryString;

        // Convert the binary string to a GF2X polynomial
        BytesFromGF2X((unsigned char*)binaryString.c_str(), poly, binaryString.length());
        vector<byte> sequence;
        for (int i = 0; i < n; i++)
        {
            sequence.push_back((byte)0);
        }
        int linearComplexity = BerlekampMassey(sequence, n);
        int sphericalComplexity = n - linearComplexity + 1;

        cout << "Sequence: " << binaryString << endl;
        cout << "Minimal polynomial: " << poly << endl;
        cout << "Linear complexity: " << linearComplexity << endl;
        cout << "Spherical complexity: " << sphericalComplexity << endl;
    }
    else {
        printTable(n, poly);
    }

    return 0;
}
