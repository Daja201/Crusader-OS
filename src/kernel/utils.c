unsigned long long __udivdi3(unsigned long long a, unsigned long long b) {
    unsigned long long q = 0;
    while (a >= b) {
        a -= b;
        q++;
    }
    return q;
}

unsigned long long __umoddi3(unsigned long long a, unsigned long long b) {
    while (a >= b) {
        a -= b;
    }
    return a;
}
