#include <iostream>
#define N 101
using namespace std;
template<class T>
class Heap       // 大根堆
{
private:
	T arr[N];
	int max_len;
	int count;
public:
	Heap(int len)
	{
		this->max_len = len;
		this->count = 0;
	}

	/***建堆**/
	bool static buildHeap(T a[], int n)
	{
		for (int i = (n-1) / 2; i >= 0; --i) {   // 只堆化所有内部节点
			heapify(a, n, i);
		}
		return true;
	}

	/*****插入节点*****/
	bool insert(T data)
	{
		if (count >= max_len)
			return false;

		// 添加插入数据到末尾
		arr[count++] = data;
		int i = count-1;
		// 从下往上堆化
		while ((i-1)/2>=0 && arr[i]>arr[(i - 1) / 2]) {  // 有父亲节点并且大小关系不满足堆条件
			T temp = arr[i];
			arr[i] = arr[(i - 1) / 2];
			arr[(i - 1) / 2] = temp;
			i = (i-1)/2;
		}
		return true;
	}

	/*****删除顶点*****/
	bool remove()
	{
		if (count == 0) return false;
		arr[0] = arr[count-1];
		--count;
		heapify(arr, count,0);
		return true;
	}

	/*****堆化*****/
	void static heapify(T arr[], int count,int i)
	{
		int maxPos = i;
		while (true) {
			// 比较哪个子节点更大
			if (i * 2+1 <= count-1&&arr[i * 2+1]>arr[i]) maxPos = i * 2+1;
			if (i * 2 + 2 <= count-1&&arr[i * 2 + 2]>arr[maxPos]) maxPos = i * 2 + 2;

			// 如果这个节点是本身,则结束堆化
			if (maxPos == i) break;

			T temp = arr[i];
			arr[i] = arr[maxPos];
			arr[maxPos] = temp;
			i = maxPos;
		}
	}

	// n 表示数据的个数，数组 a 中的数据从下标 1 到 n
	void static sort(T a[], int n) 
	{
		buildHeap(a, n);
		int k = n;
		while (k > 0) {
			T temp = a[k-1];
			a[k-1] = a[0];
			a[0] = temp;
			k--;
			heapify(a, k, 0);
		}

	}
};

int main()
{
	    Heap<int> h(10);
	    h.insert(2);
	    h.remove();
	    int a[10]={2,3,1,10,5,6,7,8,9,4};
	    h.sort(a, 10);
	    for(auto i:a)
	        cout<<i<<" ";
		system("pause");
}
