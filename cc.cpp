#include <bits/stdc++.h>
using namespace std;

int main()
{
    // your code goes here
    int t;
    cin >> t;
    while (t--)
    {
        int n;
        cin >> n;

        int arr[n];

        for (int i = 0; i < n; i++)
        {
            cin >> arr[i];
        }

        if (n == 1)
        {
            cout << 0 << endl;
            break;
        }

        sort(arr, arr + n);

        int count = 0;
        for (int i = 0; i < n; i++)
        {
            if (arr[i] != arr[0])
            {
                count++;
            }
        }
        cout << count << endl;
    }
    return 0;
}