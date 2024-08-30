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
	// BKDRHash ��ϣ����ʵ��
	struct BKDRHash
	{
		size_t operator()(const string& key)
		{
			size_t hash = 0; // ��ʼ����ϣֵ
			for (auto ch : key) // �����ַ����е�ÿ���ַ�
			{
				hash *= 131; // ����һ������
				hash += ch;  // ���ϵ�ǰ�ַ��� ASCII ֵ
			}
			return hash; // ���ؼ���õ��Ĺ�ϣֵ
		}
	};

	// APHash ��ϣ����ʵ��
	struct APHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 0; // ��ʼ����ϣֵ
			int i = 0; // ������

			for (auto ch : key) // �����ַ����е�ÿ���ַ�
			{
				if ((i & 1) == 0) // ��� i ��ż��
				{
					hash ^= ((hash << 7) ^ (ch) ^ (hash >> 3)); // ���й�ϣ����
				}
				else // ��� i ������
				{
					hash ^= (~((hash << 11) ^ (ch) ^ (hash >> 5))); // ���й�ϣ����
				}

				++i; // ��������һ
			}

			return hash; // ���ؼ���õ��Ĺ�ϣֵ
		}
	};

	// DJBHash ��ϣ����ʵ��
	struct DJBHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 5381; // ��ʼ����ϣֵ

			for (auto ch : key) // �����ַ����е�ÿ���ַ�
			{
				hash += (hash << 5) + ch; // ���й�ϣ����
			}

			return hash; // ���ؼ���õ��Ĺ�ϣֵ
		}
	};

	// JSHash ��ϣ����ʵ��
	struct JSHash
	{
		size_t operator()(const string& s)
		{
			size_t hash = 1315423911; // ��ʼ����ϣֵ
			for (auto ch : s) // �����ַ����е�ÿ���ַ�
			{
				hash ^= ((hash << 5) + ch + (hash >> 2)); // ���й�ϣ����
			}
			return hash; // ���ؼ���õ��Ĺ�ϣֵ
		}
	};

	// ��¡������ģ����
	// N �����洢�����ݸ�����X ��λ���ı���
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
		// ���Ԫ�ص���¡������
		void set(const K& key)
		{
			// �����ĸ���ϣֵ
			size_t hash1 = HashFunc1()(key) % (N * X);
			size_t hash2 = HashFunc2()(key) % (N * X);
			size_t hash3 = HashFunc3()(key) % (N * X);
			size_t hash4 = HashFunc4()(key) % (N * X);

			// ���ö�Ӧ��λ
			_bs.set(hash1);
			_bs.set(hash2);
			_bs.set(hash3);
			_bs.set(hash4);
		}

		// ����Ԫ���Ƿ���ܴ����ڲ�¡��������
		bool test(const K& key)
		{
			// �����ĸ���ϣֵ�����Զ�Ӧ��λ
			size_t hash1 = HashFunc1()(key) % (N * X);
			if (!_bs.test(hash1)) // �����ӦλΪ0������false
			{
				return false;
			}

			size_t hash2 = HashFunc2()(key) % (N * X);
			if (!_bs.test(hash2)) // �����ӦλΪ0������false
			{
				return false;
			}

			size_t hash3 = HashFunc3()(key) % (N * X);
			if (!_bs.test(hash3)) // �����ӦλΪ0������false
			{
				return false;
			}

			size_t hash4 = HashFunc4()(key) % (N * X);
			if (!_bs.test(hash4)) // �����ӦλΪ0������false
			{
				return false;
			}

			// �������λ��Ϊ1�����ܴ�������
			return true; // ���ܴ������У�ӳ�伸��λ�ö���ͻ���ͻ�����
		}

	private:
		std::bitset<N* X> _bs; // λ���ϣ����ڴ洢��¡��������״̬
	};

	// ���Բ�¡�������ĺ���1
	void test_bloomfilter1()
	{
		// ��������
		string str[] = { "��˽�", "�����", "ɳ��", "������", "������1","1������","��1����","��11����","1������1" };
		BloomFilter<10> bf; // ������¡������ʵ��
		for (auto& str : str) // ������������ӵ���¡������
		{
			bf.set(str);
		}

		for (auto& s : str) // ��������ӵ��ַ���
		{
			cout << bf.test(s) << endl; // ������Խ��
		}
		cout << endl;

		srand(time(0)); // �����������
		for (const auto& s : str) // ��������ַ���
		{
			cout << bf.test(s + to_string(rand())) << endl; // ������Խ��
		}
	}

	// ���Բ�¡�������ĺ���2
	void test_bloomfilter2()
	{
		srand(time(0)); // �����������
		const size_t N = 100000; // �������ݸ���
		BloomFilter<N> bf; // ������¡������ʵ��

		std::vector<std::string> v1; // �洢��һ���ַ�����
		std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";

		for (size_t i = 0; i < N; ++i) // ����ַ�������һ������
		{
			v1.push_back(url + std::to_string(i));
		}

		for (auto& str : v1) // ����һ�����ϵ��ַ�����ӵ���¡������
		{
			bf.set(str);
		}

		// �������һ���������Ƶ�����ͬ���ַ�����
		std::vector<std::string> v2;
		for (size_t i = 0; i < N; ++i)
		{
			std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";
			url += std::to_string(999999 + i);
			v2.push_back(url);
		}

		size_t n2 = 0; // ��¼��������
		for (auto& str : v2) // ���Եڶ������ϵ��ַ���
		{
			if (bf.test(str)) // �����¡���������ؿ��ܴ���
			{
				++n2; // ����������һ
			}
		}
		cout << "�����ַ���������:" << (double)n2 / (double)N << endl; // ���������

		// �����������ַ�����
		std::vector<std::string> v3;
		for (size_t i = 0; i < N; ++i)
		{
			string url = "zhihu.com";
			url += std::to_string(i + rand());
			v3.push_back(url);
		}

		size_t n3 = 0; // ��¼��������
		for (auto& str : v3) // ���Ե��������ϵ��ַ���
		{
			if (bf.test(str)) // �����¡���������ؿ��ܴ���
			{
				++n3; // ����������һ
			}
		}
		cout << "�������ַ���������:" << (double)n3 / (double)N << endl; // ���������
	}
}