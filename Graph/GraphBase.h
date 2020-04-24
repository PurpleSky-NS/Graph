#pragma once

#include <vector>
#include <functional>
#include <stdexcept>
#include <cstring>
#include <queue>
#include "MST.h"

/*
T为顶点类型，W为权重类型，NullValue为权重无效值(比如整数可以用无穷大)，如果指定权重类型则该参数也必须修改
*/
template<class T, class W, W NullValue>
class GraphBase
{
public:

	static_assert(std::is_arithmetic<W>::value, "权重W类型必须是可运算的算数类型");

	/*顶点类型*/
	using VertexType = T;
	/*权重类型*/
	using WeightType = W;
	/*顶点下标，在删除某个顶点后该数值会变动*/
	using VertexPosType = size_t;
	/*顶点遍历的回调函数*/
	using OnPassVertex = std::function<void(VertexPosType)>;
	/*边遍历的回调函数*/
	using OnPassEdge = std::function<void(VertexPosType from, VertexPosType to, const W& weight)>;
	/*无效下标，仅用来标记下标查找的结果*/
	static constexpr W NullValue = NullValue;
	/*无效下标，仅用来标记下标查找的结果*/
	static constexpr auto NPOS = static_cast<size_t>(-1);

	/*插入一个顶点*/
	virtual void InsertVertex(const T& v) = 0;

	/*插入或删除一条边，对于无向图，两个参数顺序无所谓*/
	virtual void InsertEdge(VertexPosType from, VertexPosType to, const W& weight) = 0;

	/*查找是否存在顶点 O(VertexNum)*/
	virtual bool ExistVertex(const T& v)const;

	/*查找从from到to是否存在边*/
	virtual bool ExistEdge(VertexPosType from, VertexPosType to)const = 0;

	/*获取从from到to的权重 */
	virtual W GetWeight(VertexPosType from, VertexPosType to)const = 0;

	/*设置从from到to的权重 weight=NullValue删除该边，如果没有则添加*/
	virtual void SetWeight(VertexPosType from, VertexPosType to, const W& weight) = 0;

	/*删除顶点，删完后下标会改变*/
	virtual void RemoveVertex(VertexPosType v) = 0;

	/*删除边*/
	virtual void RemoveEdge(VertexPosType from, VertexPosType to) = 0;

	/*获取顶点所在下标，这个慢 O(VertexNum)*/
	virtual size_t GetVertexPos(const T& v)const;

	/*获取下标所在顶点，这个快，不过应该先检查一下pos以免越界 O(1)*/
	virtual T& GetVertex(VertexPosType pos);
	virtual const T& GetVertex(VertexPosType pos)const;

	/*遍历出邻接点*/
	virtual void ForeachOutNeighbor(VertexPosType v, OnPassVertex func)const = 0;

	/*遍历入邻接点(在无向图中与@GetOutNeighbor功能相同)*/
	virtual void ForeachInNeighbor(VertexPosType v, OnPassVertex func)const = 0;

	/*获取顶点数 O(1)*/
	virtual size_t GetVertexNum()const;

	/*获取边数 O(1)*/
	virtual size_t GetEdgeNum()const;

	/*从v开始DFS遍历*/
	virtual void DFS(VertexPosType v, OnPassVertex func)const;

	/*从v开始BFS遍历*/
	virtual void BFS(VertexPosType v, OnPassVertex func)const;

	/*遍历所有顶点 O(VertexNum)*/
	virtual void ForeachVertex(OnPassVertex func)const;

	/*遍历所有边*/
	virtual void ForeachEdge(OnPassEdge func)const = 0;

	/*获取完整邻接矩阵，二维的邻接矩阵会以行为单位，存储在一维线性表中*/
	virtual std::vector<W> GetAdjacencyMatrix()const = 0;

	/*获取图的主要内存占用量(byte)，不包括顶点信息(无法精确测量)以及其容器等次要因素的占用量
	该占用量与权重类型以及该类的实现类的密切相关*/
	virtual unsigned long long GetMemoryUsage()const = 0;

	/*获取最小生成树(详见MST.h)，返回最小权值，最小生成树若为空则表示生成失败*/
	virtual unsigned long long GetMST(MSTBase& mst)const = 0;

protected:
	std::vector<T> m_vertexData;
	size_t m_edgeNum = 0;
};

template<class T, class W, W NullValue>
inline bool GraphBase<T, W, NullValue>::ExistVertex(const T& v) const
{
	for (auto i : m_vertexData)
		if (i == v)
			return true;
	return false;
}

template<class T, class W, W NullValue>
inline size_t GraphBase<T, W, NullValue>::GetVertexPos(const T& v)const
{
	for (size_t i = 0; i < m_vertexData.size(); ++i)
		if (m_vertexData[i] == v)
			return i;
	return NPOS;
}

template<class T, class W, W NullValue>
inline T& GraphBase<T, W, NullValue>::GetVertex(VertexPosType pos)
{
	return m_vertexData[pos];
}

template<class T, class W, W NullValue>
inline const T& GraphBase<T, W, NullValue>::GetVertex(VertexPosType pos) const
{
	return m_vertexData[pos];
}

template<class T, class W, W NullValue>
inline size_t GraphBase<T, W, NullValue>::GetVertexNum() const
{
	return m_vertexData.size();
}

template<class T, class W, W NullValue>
inline size_t GraphBase<T, W, NullValue>::GetEdgeNum() const
{
	return m_edgeNum;
}

template<class T, class W, W NullValue>
inline void GraphBase<T, W, NullValue>::DFS(VertexPosType v, OnPassVertex func)const
{
	bool* isVisited = new bool[GetVertexNum()];
	std::function<void(VertexPosType)> dfs;
	dfs = [&](VertexPosType v)
	{
		isVisited[v] = true;
		func(v);
		ForeachOutNeighbor(v, [&](auto i)
			{
				if (!isVisited[i])
					dfs(i);
			});
	};
	memset(isVisited, false, sizeof(bool) * GetVertexNum());
	dfs(v);
	delete[] isVisited;
}

template<class T, class W, W NullValue>
inline void GraphBase<T, W, NullValue>::BFS(VertexPosType v, OnPassVertex func)const
{
	bool* isVisited = new bool[GetVertexNum()];
	std::queue<VertexPosType> q;
	memset(isVisited, false, sizeof(bool) * GetVertexNum());
	q.push(v);
	isVisited[v] = true; //初始已经访问过
	while (!q.empty())
	{
		auto pos = q.front();
		q.pop();
		func(pos);
		ForeachOutNeighbor(pos, [&](auto i) //遍历所有邻接点
			{
				if (!isVisited[i])
				{
					q.push(i);
					isVisited[i] = true;
				}
			});
	}
	delete[] isVisited;
}

template<class T, class W, W NullValue>
inline void GraphBase<T, W, NullValue>::ForeachVertex(OnPassVertex func) const
{
	for (size_t i = 0; i < m_vertexData.size(); ++i)
		func(i);
}
