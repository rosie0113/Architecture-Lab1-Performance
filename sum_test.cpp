#include <iostream>
#include <vector>
#include <windows.h>
#include <iomanip>

using namespace std;

void test_sum(int N) {
    vector<float> a(N, 1.0f);
    LARGE_INTEGER head, tail, freq;
    QueryPerformanceFrequency(&freq);
    
    // 为了防止程序跑得太快测不到时间，我们跑多次取平均
    int repeat = 100; 

    // === Level 1: 平凡算法 ===
    QueryPerformanceCounter(&head);
    for (int step = 0; step < repeat; step++) {
        float sum = 0.0f;
        for (int i = 0; i < N; i++) {
            sum += a[i]; // 强数据依赖
        }
    }
    QueryPerformanceCounter(&tail);
    double time_L1 = (tail.QuadPart - head.QuadPart) * 1000.0 / freq.QuadPart / repeat;

    // === Level 2: 纯循环展开 (未消除依赖) ===
    QueryPerformanceCounter(&head);
    for (int step = 0; step < repeat; step++) {
        float sum = 0.0f;
        int limit = N - (N % 4);
        for (int i = 0; i < limit; i += 4) {
            sum += a[i];
            sum += a[i+1];
            sum += a[i+2];
            sum += a[i+3];
        }
        for (int i = limit; i < N; i++) sum += a[i];
    }
    QueryPerformanceCounter(&tail);
    double time_L2 = (tail.QuadPart - head.QuadPart) * 1000.0 / freq.QuadPart / repeat;

    // === Level 3: 多路并行 (打破依赖) ===
    QueryPerformanceCounter(&head);
    for (int step = 0; step < repeat; step++) {
        float sum0 = 0.0f, sum1 = 0.0f, sum2 = 0.0f, sum3 = 0.0f;
        int limit = N - (N % 4);
        for (int i = 0; i < limit; i += 4) {
            sum0 += a[i];
            sum1 += a[i+1];
            sum2 += a[i+2];
            sum3 += a[i+3];
        }
        float total_sum = sum0 + sum1 + sum2 + sum3;
        for (int i = limit; i < N; i++) total_sum += a[i];
    }
    QueryPerformanceCounter(&tail);
    double time_L3 = (tail.QuadPart - head.QuadPart) * 1000.0 / freq.QuadPart / repeat;

    cout << "规模 N = " << setw(8) << N 
         << " | L1(平凡): " << fixed << setprecision(4) << time_L1 << " ms"
         << " | L2(展开): " << time_L2 << " ms"
         << " | L3(并行): " << time_L3 << " ms" << endl;
}

int main() {
    cout << "开始 n 个数求和 ILP 性能测试..." << endl;
    cout << "--------------------------------------------------------" << endl;
    int test_scales[] = {10000, 50000, 100000, 500000, 1000000, 5000000, 10000000};
    for (int N : test_scales) {
        test_sum(N);
    }
    return 0;
}