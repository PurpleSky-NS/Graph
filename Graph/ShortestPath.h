#pragma once

#include <type_traits>
#include <functional>
#include <vector>
#include <stack>
#include "GraphBase.h"

/*WT是权重累加和类型，一般是一个比较大的类型*/
template<class WT>
class SSSP
{
	struct _VertexInfo
	{
		WT dist;
		size_t prevVertex;
	};
public:

	static_assert(std::is_arithmetic<WT>::value, "类型WT必须为算数类型");
	static_assert(std::is_integral<size_t>::value, "类型size_t必须为整型");
	static_assert(sizeof(size_t) <= sizeof(size_t), "类型size_t太大了，不需要这么大");

	static constexpr auto NullValue = static_cast<WT>(-1);

	/*执行sssp，根据图的类型不同，选择dijkstra还是bfs改造算法，权重为负数的图会导致算法出错*/
	template<class T, class W>
	void Execute(const GraphBase<T, W>& g, size_t src);

	/*顶点数量 O(1)*/
	size_t GetVertexNum()const;

	/*清除*/
	void Clear();

	/*是否为空O(1)*/
	bool IsEmpty()const;

	/*获取源点 O(1)*/
	size_t GetSrc()const;

	/*获取到某节点的最短距离，如果target!=src但是返回值为0，说明此路不通 O(1)*/
	WT GetDistance(size_t target)const;

	/*遍历到某节点的最短路径 O(Path)*/
	void ForeachPath(size_t target, std::function<void(size_t)> func)const;

private:

	size_t m_src;
	std::vector<_VertexInfo> m_info;

	/*初始化顶点，dist全初始化为 NullValue，prev全是num*/
	void Init(size_t num, size_t src);

	/*获取单源最短路径，BFS改进算法*/
	template<class T, class W>
	void UnweightedSSSP(const GraphBase<T, W>& g, size_t src);

	/*获取单源最短路径，使用dijkstra算法*/
	template<class T, class W>
	void WeightedSSSP(const GraphBase<T, W>& g, size_t src);
};

/*权重为非负整数的SSSP*/
typedef SSSP<unsigned long long> IntegerSSSP;
/*权重为非负小数的SSSP*/
typedef SSSP<double> DecimalSSSP;

template<class WT>
inline size_t SSSP<WT>::GetVertexNum() const
{
	return m_info.size();
}

template<class WT>
inline void SSSP<WT>::Clear()
{
	m_info.clear();
	m_info.shrink_to_fit();
}

template<class WT>
inline bool SSSP<WT>::IsEmpty() const
{
	return m_info.empty();
}

template<class WT>
inline size_t SSSP<WT>::GetSrc() const
{
	return m_src;
}

template<class WT>
inline WT SSSP<WT>::GetDistance(size_t target) const
{
	return m_info[target].dist;
}

template<class WT>
inline void SSSP<WT>::ForeachPath(size_t target, std::function<void(size_t)> func) const
{
	std::stack<size_t> stack;
	while (m_info[target].prevVertex != GetVertexNum())
	{
		stack.push(target);
		target = m_info[target].prevVertex;
	}
	if (stack.empty())
		return;
	func(m_src);
	while (!stack.empty())
	{
		func(stack.top());
		stack.pop();
	}
}

template<class WT>
inline void SSSP<WT>::Init(size_t num, size_t src)
{
	Clear();
	m_info.resize(num, { NullValue, (size_t)num });
	m_src = src;
}


template<class WT>
template<class T, class W>
inline void SSSP<WT>::Execute(const GraphBase<T, W>& g, size_t src)
{
	Clear();
	if (!g.GetVertexNum())
		return;
	Init(g.GetVertexNum(), src);
	m_info[src].dist = 0;
	if (g.IsWeighted())
		WeightedSSSP(g, src);
	else
		UnweightedSSSP(g, src);
}

template<class WT>
template<class T, class W>
inline void SSSP<WT>::UnweightedSSSP(const GraphBase<T, W>& g, size_t src)
{
	std::queue<size_t> q;

	q.push(src);

	while (!q.empty())
	{
		auto pos = q.front();
		q.pop();
		g.ForeachOutNeighbor(pos, [&](auto i) //遍历所有邻接点
			{
				if (m_info[i].dist == NullValue)
				{
					q.push(i);
					m_info[i].dist = m_info[pos].dist + 1;
					m_info[i].prevVertex = (size_t)pos;
				}
			});
	}
}

template<class WT>
template<class T, class W>
inline void SSSP<WT>::WeightedSSSP(const GraphBase<T, W>& g, size_t src)
{
	struct _VertexInfo //查找未收录顶点需要用到
	{
		size_t pos;
		WT dist;

		_VertexInfo(size_t pos, WT dist) :
			pos(pos), dist(dist) {}
		bool operator<(const _VertexInfo& v)const
		{
			if (dist == v.dist)
				return pos > v.pos;
			if (dist < 0)
				return true;
			else if (v.dist < 0)
				return false;
			return dist > v.dist;
		}
	};

	std::vector<bool> collected(GetVertexNum(), false); //判断是否收录过，因为并不是像bfs一样每个顶点遍历一次，所以需要记录
	size_t unClNum = GetVertexNum(); //未收录顶点个数

	/*初始化顶点*/
	collected[src] = true;

	g.ForeachOutNeighbor(src, [&](auto from, auto to, auto w) //遍历所有邻接点
		{
			m_info[to].dist = w;
			m_info[to].prevVertex = from;
		});

	while (--unClNum)
	{
		std::priority_queue<_VertexInfo> pq;//最小堆，用来找最小未收录顶点
		/*查找一个最小收录顶点*/
		for (size_t i = 0; i < collected.size(); ++i)
			if (!collected[i])
				pq.emplace(i, m_info[i].dist);

		if (pq.top().dist == NullValue) //如果没有联通的点，就退出
			break;

		collected[pq.top().pos] = true;
		g.ForeachOutNeighbor(pq.top().pos, [&](auto i, auto j, auto w) //遍历所有邻接点
			{
				if (!collected[j]) //如果没收录
					if (m_info[j].dist < 0 || m_info[i].dist + w < m_info[j].dist) //如果没访问过或者距离可以更新
					{
						m_info[j].dist = m_info[i].dist + w;
						m_info[j].prevVertex = i;
					}
			});
	}
}
/*WT是权重累加和类型，一般是一个比较大的类型*/
template<class WT>
class MSSP
{
	struct _VertexInfo
	{
		WT dist;
		size_t prevVertex;
	};
public:

	static_assert(std::is_arithmetic<WT>::value, "类型WT必须为算数类型");
	static_assert(std::is_integral<size_t>::value, "类型size_t必须为整型");
	static_assert(sizeof(size_t) <= sizeof(size_t), "类型size_t太大了，不需要这么大");

	static constexpr auto NullValue = static_cast<WT>(-1);

	/*执行MSSP，使用Floyd算法，权重为负数的图会导致算法出错 O(VertexNum^3)*/
	template<class T, class W>
	void Execute(const GraphBase<T, W>& g);

	/*顶点数量 O(1)*/
	size_t GetVertexNum()const;

	/*清除*/
	void Clear();

	/*是否为空O(1)*/
	bool IsEmpty()const;

	/*获取到某节点的最短距离，如果target!=src但是返回值为0，说明此路不通 O(1)*/
	WT GetDistance(size_t src, size_t target)const;

	/*遍历从某节点到某节点的最短路径 O(Path)*/
	void ForeachPath(size_t src, size_t target, std::function<void(size_t)> func)const;

private:

	size_t m_size = 0;
	std::vector<_VertexInfo> m_info;

	/*初始化顶点，dist全初始化为 NullValue，prev全是num*/
	void Init(size_t num);

	_VertexInfo& GetInfo(size_t from, size_t to);

	const _VertexInfo& GetInfo(size_t from, size_t to)const;

};

/*权重为非负整数的MSSP*/
typedef MSSP<unsigned long long> IntegerMSSP;
/*权重为非负小数的MSSP*/
typedef MSSP<double> DecimalMSSP;

template<class WT>
inline size_t MSSP<WT>::GetVertexNum() const
{
	return m_size;
}

template<class WT>
inline void MSSP<WT>::Clear()
{
	m_info.clear();
	m_info.shrink_to_fit();
	m_size = 0;
}

template<class WT>
inline bool MSSP<WT>::IsEmpty() const
{
	return m_size == 0;
}

template<class WT>
inline WT MSSP<WT>::GetDistance(size_t src, size_t target) const
{
	return m_info[src * m_size + target].dist;
}

template<class WT>
inline void MSSP<WT>::ForeachPath(size_t src, size_t target, std::function<void(size_t)> func) const
{
	if (GetDistance(src, target) == NullValue) //此路不通
		return;
	//想想，要从i->j，那么就要从i->k->j，其中k为中转点存在m_info[i][j].prevVertex里
	//然后，要从i->k，那么就要从i->l->k，其中l为中转点.......
	//最后，要从i->x，那么.............，然而m_info[i][x].prevVertex并没有点，说明了i->x本身就是最短路径，不需要中转点
	//所以，从i->j，就变成了i->x->...->l->k->j，从x->...->k是个逆序过程
	std::stack<size_t> path;
	auto j = target;
	size_t k;
	while ((k = GetInfo(src, j).prevVertex) != m_size)
	{
		path.push(k); //经过k
		j = k; //更新j
	}
	func(src);
	while (!path.empty())
	{
		func(path.top());
		path.pop();
	}
	func(target);
}

template<class WT>
inline void MSSP<WT>::Init(size_t num)
{
	Clear();
	m_info.resize(num * num, { NullValue, (size_t)num });
	m_size = num;
}

template<class WT>
inline typename MSSP<WT>::_VertexInfo& MSSP<WT>::GetInfo(size_t from, size_t to)
{
	return m_info[from * m_size + to];
}

template<class WT>
inline const typename MSSP<WT>::_VertexInfo& MSSP<WT>::GetInfo(size_t from, size_t to) const
{
	return m_info[from * m_size + to];
}

template<class WT>
template<class T, class W>
inline void MSSP<WT>::Execute(const GraphBase<T, W>& g)
{
	Clear();
	if (!g.GetVertexNum())
		return;
	Init(g.GetVertexNum());
	g.ForeachEdge([&](auto from, auto to, auto w) //初始化距离为权重大小
		{
			GetInfo(from, to).dist = w;
		});

	for (size_t k = 0; k < m_size; ++k)
		for (size_t i = 0; i < m_size; ++i)
			for (size_t j = 0; j < m_size; ++j)
			{
				//如果i->k或者k->j不存在通路，那么i->j就不可能经过k有最短路径
				if (GetInfo(i, k).dist == NullValue || GetInfo(k, j).dist == NullValue)
					continue;
				if (GetInfo(i, k).dist + GetInfo(k, j).dist < GetInfo(i, j).dist)
				{
					GetInfo(i, j).dist = GetInfo(i, k).dist + GetInfo(k, j).dist;
					GetInfo(i, j).prevVertex = k;
				}
			}
}
