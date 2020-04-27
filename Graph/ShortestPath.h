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
	template<class T, class W, W NV>
	void Execute(const GraphBase<T, W, NV>& g, size_t src);

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
	template<class T, class W, W NV>
	void UnweightedSSSP(const GraphBase<T, W, NV>& g, size_t src);

	/*获取单源最短路径，使用dijkstra算法*/
	template<class T, class W, W NV>
	void WeightedSSSP(const GraphBase<T, W, NV>& g, size_t src);
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
template<class T, class W, W NV>
inline void SSSP<WT>::Execute(const GraphBase<T, W, NV>& g, size_t src)
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
template<class T, class W, W NV>
inline void SSSP<WT>::UnweightedSSSP(const GraphBase<T, W, NV>& g, size_t src)
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
template<class T, class W, W NV>
inline void SSSP<WT>::WeightedSSSP(const GraphBase<T, W, NV>& g, size_t src)
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
