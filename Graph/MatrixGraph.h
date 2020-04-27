#pragma once

#include "GraphBase.h"

/*邻接矩阵图的父类，实现了一些邻接矩阵图的共有操作*/
template<class T, class W, W NullValue>
class MatrixGraph :public GraphBase<T, W, NullValue>
{
public:

	using typename GraphBase<T, W, NullValue>::VertexPosType;
	using typename GraphBase<T, W, NullValue>::OnPassVertex;
	using typename GraphBase<T, W, NullValue>::OnPassEdge;

	/*插入一条边，对于无向图，两个参数顺序无所谓 O(1)*/
	virtual void InsertEdge(VertexPosType from, VertexPosType to, const W& weight)override;

	/*查找从from到to是否存在边 O(1)*/
	virtual bool ExistEdge(VertexPosType from, VertexPosType to)const override;

	/*删除边 O(1)*/
	virtual void RemoveEdge(VertexPosType from, VertexPosType to)override;

	/*遍历出邻接点 O(VertexNum)*/
	virtual void ForeachOutNeighbor(VertexPosType v, OnPassVertex func)const override;

	/*遍历入邻接点 O(VertexNum)*/
	virtual void ForeachInNeighbor(VertexPosType v, OnPassVertex func)const override;

	/*遍历出邻接点*/
	virtual void ForeachOutNeighbor(VertexPosType v, OnPassEdge func)const override;

	/*遍历入邻接点(在无向图中与@GetOutNeighbor功能相同)*/
	virtual void ForeachInNeighbor(VertexPosType v, OnPassEdge func)const override;

	/*收缩内存占用，在每次移除一个顶点时并不会真的释放内存，从而提高再次插入顶点效率，详见@vector
	如果不需要插入顶点或者需要收缩内存，请调用这个
	内部调用@vector.shrink_to_fit*/
	virtual void Shrink_To_Fit() = 0;

	virtual bool IsMatrix()const;

protected:

	/*获取最小生成树(详见MST.h)，返回最小权值，最小生成树若为空则表示生成失败，采用Prim算法 O(VertexNum^2)*/
	template<class MST_PT, class MST_WT>
	MST_Parent<MST_PT, MST_WT> _GetMST()const;
};

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::InsertEdge(VertexPosType from, VertexPosType to, const W& weight)
{
	if (ExistEdge(from, to))
		return;
	++this->m_edgeNum;
	this->SetWeight(from, to, weight);
}

template<class T, class W, W NullValue>
inline bool MatrixGraph<T, W, NullValue>::ExistEdge(VertexPosType from, VertexPosType to) const
{
	return this->GetWeight(from, to) != NullValue;
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::RemoveEdge(VertexPosType from, VertexPosType to)
{
	if (!ExistEdge(from, to))
		return;
	--this->m_edgeNum;
	this->SetWeight(from, to, NullValue);
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachOutNeighbor(VertexPosType v, OnPassVertex func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		if (ExistEdge(v, i))
			func(i);
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachInNeighbor(VertexPosType v, OnPassVertex func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		if (this->ExistEdge(i, v))
			func(i);
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachOutNeighbor(VertexPosType v, OnPassEdge func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		if (ExistEdge(v, i))
			func(v, i, this->GetWeight(v, i));
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachInNeighbor(VertexPosType v, OnPassEdge func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		if (this->ExistEdge(i, v))
			func(i, v, this->GetWeight(i, v));
}

template<class T, class W, W NullValue>
inline bool MatrixGraph<T, W, NullValue>::IsMatrix() const
{
	return true;
}

template<class T, class W, W NullValue>
template<class MST_PT, class MST_WT>
inline MST_Parent<MST_PT, MST_WT> MatrixGraph<T, W, NullValue>::_GetMST() const
{
	struct Distance
	{
		VertexPosType vertex;	//与哪个点相连
		W minCost = NullValue;  //用NullValue标记没有被访问过
		bool isAdded = false;	//受否被收录
	};
	size_t lastVertexNum = this->GetVertexNum();	 //剩余多少个顶点不在生成树中
	Distance* dist = new Distance[this->GetVertexNum()]; //到各个顶点的距离
	VertexPosType minEdgePos = 0;	//最小边的下标
	VertexPosType newVertex = 0;		//新加入的顶点
	W tmpWeight;						//临时变量
	MST_Parent<MST_PT, MST_WT> mst;		//最小生成树

	mst.SetVertexNum(this->GetVertexNum()); //初始化生成树
	if (lastVertexNum)
	{
		mst.SetParent(0, this->GetVertexNum());
		dist[0].isAdded = true;
	}
	while (lastVertexNum--) //直到所有顶点都进入生成树为止
	{
		for (VertexPosType i = 0; i < this->GetVertexNum(); ++i) //遍历所有顶点记录并求出最小边
		{
			if (newVertex == i || dist[i].isAdded) //跳过自己和被添加过的顶点
				continue;
			tmpWeight = this->GetWeight(newVertex, i);
			//如果没有边则跳过该点，否则如果这个点没有被访问过或者权值比原来的小则更新最小权值
			if (tmpWeight != NullValue && (dist[i].minCost == NullValue || tmpWeight < dist[i].minCost))
			{
				dist[i].vertex = newVertex;
				dist[i].minCost = tmpWeight;
			}
			//判断这个节点本省需不需要更新最小权值边，如果这个节点本身无意义就不需要更新
			if (dist[i].minCost != NullValue && (dist[minEdgePos].isAdded || dist[i].minCost < dist[minEdgePos].minCost))
				minEdgePos = i;
		}
		/*如果算出来的最小权边无法到达该节点，或者这个边对应的节点被添加过了，就可以认为没有节点符合要求了*/
		if (dist[minEdgePos].minCost == NullValue || dist[minEdgePos].isAdded)
			break;
		//否则收录该顶点
		mst.SetParent(minEdgePos, dist[minEdgePos].vertex);
		dist[minEdgePos].isAdded = true;
		newVertex = minEdgePos;
		mst.AddWeight(dist[minEdgePos].minCost);
	}
	if (lastVertexNum) //还有剩余顶点，算法失败
		mst.Clear();
	return mst;
}
