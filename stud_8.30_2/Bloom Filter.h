#pragma once
#include <bitset>        
#include <string>       
#include <iostream>     
#include <vector>        
#include <ctime>         
#include <cstdlib>     
#include <algorithm>     


namespace luow
{
	// BKDRHash 哈希函数实现
	struct BKDRHash
	{
		size_t operator()(const string& key)
		{
			size_t hash = 0; // 初始化哈希值
			for (auto ch : key) // 遍历字符串中的每个字符
			{
				hash *= 131; // 乘以一个质数
				hash += ch;  // 加上当前字符的 ASCII 值
			}
			return hash; // 返回计算得到的哈希值
		}
	};

	// APHash 哈希函数实现
	struct APHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 0; // 初始化哈希值
			int i = 0; // 计数器

			for (auto ch : key) // 遍历字符串中的每个字符
			{
				if ((i & 1) == 0) // 如果 i 是偶数
				{
					hash ^= ((hash << 7) ^ (ch) ^ (hash >> 3)); // 进行哈希计算
				}
				else // 如果 i 是奇数
				{
					hash ^= (~((hash << 11) ^ (ch) ^ (hash >> 5))); // 进行哈希计算
				}

				++i; // 计数器加一
			}

			return hash; // 返回计算得到的哈希值
		}
	};

	// DJBHash 哈希函数实现
	struct DJBHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 5381; // 初始化哈希值

			for (auto ch : key) // 遍历字符串中的每个字符
			{
				hash += (hash << 5) + ch; // 进行哈希计算
			}

			return hash; // 返回计算得到的哈希值
		}
	};

	// JSHash 哈希函数实现
	struct JSHash
	{
		size_t operator()(const string& s)
		{
			size_t hash = 1315423911; // 初始化哈希值
			for (auto ch : s) // 遍历字符串中的每个字符
			{
				hash ^= ((hash << 5) + ch + (hash >> 2)); // 进行哈希计算
			}
			return hash; // 返回计算得到的哈希值
		}
	};

	// 布隆过滤器模板类
	// N 是最多存储的数据个数，X 是位数的倍数
	template<size_t N,
		size_t X = 6,
		class K = string,
		class HashFunc1 = BKDRHash,
		class HashFunc2 = APHash,
		class HashFunc3 = DJBHash,
		class HashFunc4 = JSHash>
	class BloomFilter
	{
	public:
		// 添加元素到布隆过滤器
		void set(const K& key)
		{
			// 计算四个哈希值
			size_t hash1 = HashFunc1()(key) % (N * X);
			size_t hash2 = HashFunc2()(key) % (N * X);
			size_t hash3 = HashFunc3()(key) % (N * X);
			size_t hash4 = HashFunc4()(key) % (N * X);

			// 设置对应的位
			_bs.set(hash1);
			_bs.set(hash2);
			_bs.set(hash3);
			_bs.set(hash4);
		}

		// 测试元素是否可能存在于布隆过滤器中
		bool test(const K& key)
		{
			// 计算四个哈希值并测试对应的位
			size_t hash1 = HashFunc1()(key) % (N * X);
			if (!_bs.test(hash1)) // 如果对应位为0，返回false
			{
				return false;
			}

			size_t hash2 = HashFunc2()(key) % (N * X);
			if (!_bs.test(hash2)) // 如果对应位为0，返回false
			{
				return false;
			}

			size_t hash3 = HashFunc3()(key) % (N * X);
			if (!_bs.test(hash3)) // 如果对应位为0，返回false
			{
				return false;
			}

			size_t hash4 = HashFunc4()(key) % (N * X);
			if (!_bs.test(hash4)) // 如果对应位为0，返回false
			{
				return false;
			}

			// 如果所有位都为1，可能存在误判
			return true; // 可能存在误判，映射几个位置都冲突，就会误判
		}

	private:
		std::bitset<N* X> _bs; // 位集合，用于存储布隆过滤器的状态
	};

	// 测试布隆过滤器的函数1
	void test_bloomfilter1()
	{
		// 测试数据
		string str[] = { "猪八戒", "孙悟空", "沙悟净", "唐三藏", "白龙马1","1白龙马","白1龙马","白11龙马","1白龙马1" };
		BloomFilter<10> bf; // 创建布隆过滤器实例
		for (auto& str : str) // 将测试数据添加到布隆过滤器
		{
			bf.set(str);
		}

		for (auto& s : str) // 测试已添加的字符串
		{
			cout << bf.test(s) << endl; // 输出测试结果
		}
		cout << endl;

		srand(time(0)); // 设置随机种子
		for (const auto& s : str) // 测试随机字符串
		{
			cout << bf.test(s + to_string(rand())) << endl; // 输出测试结果
		}
	}

	// 测试布隆过滤器的函数2
	void test_bloomfilter2()
	{
		srand(time(0)); // 设置随机种子
		const size_t N = 100000; // 设置数据个数
		BloomFilter<N> bf; // 创建布隆过滤器实例

		std::vector<std::string> v1; // 存储第一个字符串集
		std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";

		for (size_t i = 0; i < N; ++i) // 添加字符串到第一个集合
		{
			v1.push_back(url + std::to_string(i));
		}

		for (auto& str : v1) // 将第一个集合的字符串添加到布隆过滤器
		{
			bf.set(str);
		}

		// 创建与第一个集合相似但不相同的字符串集
		std::vector<std::string> v2;
		for (size_t i = 0; i < N; ++i)
		{
			std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";
			url += std::to_string(999999 + i);
			v2.push_back(url);
		}

		size_t n2 = 0; // 记录误判数量
		for (auto& str : v2) // 测试第二个集合的字符串
		{
			if (bf.test(str)) // 如果布隆过滤器返回可能存在
			{
				++n2; // 误判数量加一
			}
		}
		cout << "相似字符串误判率:" << (double)n2 / (double)N << endl; // 输出误判率

		// 创建不相似字符串集
		std::vector<std::string> v3;
		for (size_t i = 0; i < N; ++i)
		{
			string url = "zhihu.com";
			url += std::to_string(i + rand());
			v3.push_back(url);
		}

		size_t n3 = 0; // 记录误判数量
		for (auto& str : v3) // 测试第三个集合的字符串
		{
			if (bf.test(str)) // 如果布隆过滤器返回可能存在
			{
				++n3; // 误判数量加一
			}
		}
		cout << "不相似字符串误判率:" << (double)n3 / (double)N << endl; // 输出误判率
	}
}