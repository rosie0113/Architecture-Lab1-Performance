#include <iostream>
#include <vector>
#include <windows.h>
#include <iomanip>

using namespace std;

// 使用 vector 动态分配内存，防止 N=4000 时栈溢出 (Stack Overflow)
void test_matrix(int N) {
    vector<double> a(N, 1.0);
    vector<double> sum_result(N, 0.0);
    vector<vector<double>> b(N, vector<double>(N, 2.0));

    LARGE_INTEGER head, tail, freq;
    QueryPerformanceFrequency(&freq);

    // ==========================================
    // 1. 平凡算法（按列访问，破坏局部性）
    // ==========================================
    QueryPerformanceCounter(&head);
    for (int i = 0; i < N; i++) {
        sum_result[i] = 0.0;
        for (int j = 0; j < N; j++) {
            sum_result[i] += b[j][i] * a[j]; // 步长极大
        }
    }
    QueryPerformanceCounter(&tail);
    double time_bad = (tail.QuadPart - head.QuadPart) * 1000.0 / freq.QuadPart;

    // 重置结果数组
    fill(sum_result.begin(), sum_result.end(), 0.0);

    // ==========================================
    // 2. 优化算法（按行访问，契合局部性）
    // ==========================================
    QueryPerformanceCounter(&head);
    for (int i = 0; i < N; i++) {
        sum_result[i] = 0.0;
        for (int j = 0; j < N; j++) {
            sum_result[i] += b[i][j] * a[j]; // 连续访问
        }
    }
    QueryPerformanceCounter(&tail);
    double time_good = (tail.QuadPart - head.QuadPart) * 1000.0 / freq.QuadPart;

    cout << "规模 N = " << setw(4) << N 
         << " | 平凡耗时: " << fixed << setprecision(4) << time_bad << " ms"
         << " | 优化耗时: " << fixed << setprecision(4) << time_good << " ms" << endl;
}

int main() {
    cout << "开始矩阵内积性能测试..." << endl;
    cout << "--------------------------------------------------------" << endl;
    int test_scales[] = {100, 300, 500, 800, 1500, 2000, 2500, 3000, 4000};
    for (int N : test_scales) {
        test_matrix(N);
    }
    return 0;
}