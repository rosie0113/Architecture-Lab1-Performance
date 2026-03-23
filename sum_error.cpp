#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

int main()
{
    int N = 10000000; // 1000 万规模
    vector<float> a(N);

    // 初始化数组，构造递增序列以触发严重误差
    for (int i = 0; i < N; i++)
    {
        a[i] = (float)i;
    }

    // 理论真值 (高斯求和公式: N*(N-1)/2)
    double theoretical_true_value = 4999999500000.0;

    // === Level 1: 平凡求和 ===
    volatile float sum_L1 = 0.0f; // 使用 volatile 强迫写回内存，防止寄存器优化
    for (int i = 0; i < N; i++)
    {
        sum_L1 += a[i];
    }

    // === Level 3: 四路并行求和 ===
    volatile float sum0 = 0.0f, sum1 = 0.0f, sum2 = 0.0f, sum3 = 0.0f;
    int limit = N - (N % 4);
    for (int i = 0; i < limit; i += 4)
    {
        sum0 += a[i];
        sum1 += a[i + 1];
        sum2 += a[i + 2];
        sum3 += a[i + 3];
    }
    volatile float sum_L3 = sum0 + sum1 + sum2 + sum3;
    for (int i = limit; i < N; i++)
        sum_L3 += a[i];

    cout << "--- 浮点数运算次序影响探究 ---" << endl;
    cout << fixed << setprecision(1); // 固定显示 1 位小数
    cout << "理论真值      : " << theoretical_true_value << endl;
    cout << "L1 算出的总和 : " << sum_L1 << endl;
    cout << "L3 算出的总和 : " << sum_L3 << endl;

    // 计算并打印误差
    cout << "L1 的绝对误差 : " << abs(sum_L1 - theoretical_true_value) << endl;
    cout << "L3 的绝对误差 : " << abs(sum_L3 - theoretical_true_value) << endl;

    return 0;
}