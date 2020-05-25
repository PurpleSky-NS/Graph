#pragma once

#include "MatrixGraph.h"

/*无向图采用对角矩阵存储，请不要将W设置为bool，若要使用无权图，请使用UnweighedUndirectedMatrixGraph"*/
template<class T, class W = int>
class WeightedUndirectedMatrixGraph :public MatrixGraph<T, W>
{
public:

	using typename GraphBase<T, W>::VertexType;
	using typename GraphBase<T, W>::WeightType;
	using typename GraphBase<T, W>::VertexPosType;
	using typename GraphBase<T, W>::OnPassVertex;
	using typename GraphBase<T, W>::OnPassEdge;

	/*插入一个顶点 O(Pos)-O(Ele+Pos-)(可能会牵扯到vector重新申请内存) */
	virtual VertexPosType InsertVertex(const T& v)override;

	/*获取从v1到v2的权重 O(1)*/
	virtual W GetWeight(VertexPosType v1, VertexPosType v2)const override;

	/*设置从from到to的权重 weight=0删除该边，如果没有则添加 O(1)*/
	virtual void SetWeight(VertexPosType v1, VertexPosType v2, const W& weight)override;

	/*删除顶点，删完后下标会改变 O(1)-O(Ele) (下标越大速度越快)*/
	virtual void RemoveVertex(VertexPosType v)override;

	/*遍历所有边 O(Ele)*/
	virtual void ForeachEdge(OnPassEdge func)const override;

	/*获取完整邻接矩阵，二维的邻接矩阵会以行为单位，存储在一维线性表中 O(Ele)*/
	virtual std::vector<W> GetAdjacencyMatrix()const override;

	/*收缩内存占用，在每次移除一个顶点时并不会真的释放内存，从而提高再次插入顶点效率，详见@vector
	如果不需要插入顶点或者需要收缩内存，请调用这个
	内部调用@vector.shrink_to_fit*/
	virtual void Shrink_To_Fit()override;

	/*实际占用要大于该数值，因为vector会预留空间，调用Shrink_To_Fit函数后可能更趋近于该数值*/
	virtual unsigned long long GetMemoryUsage()const override;

	virtual constexpr bool IsDirected()const override;

	virtual constexpr bool IsWeighted()const override;

protected:
	std::vector<W> m_adjaMetrix; //对角矩阵
};

template<class T, class W>
inline typename WeightedUndirectedMatrixGraph<T, W>::VertexPosType WeightedUndirectedMatrixGraph<T, W>::InsertVertex(const T& v)
{
	this->m_vertexData.push_back(v);
	m_adjaMetrix.reserve(m_adjaMetrix.size() + this->m_vertexData.size());
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		m_adjaMetrix.push_back((W)0);
	return this->m_vertexData.size() - 1;
}

template<class T, class W>
inline W WeightedUndirectedMatrixGraph<T, W>::GetWeight(VertexPosType v1, VertexPosType v2)const
{
	return (v1 > v2 ? m_adjaMetrix[v1 * (v1 + 1) / 2 + v2] : m_adjaMetrix[v2 * (v2 + 1) / 2 + v1]);
}

template<class T, class W>
inline void WeightedUndirectedMatrixGraph<T, W>::SetWeight(VertexPosType v1, VertexPosType v2, const W& weight)
{
	(v1 > v2 ? m_adjaMetrix[v1 * (v1 + 1) / 2 + v2] : m_adjaMetrix[v2 * (v2 + 1) / 2 + v1]) = weight;
}

template<class T, class W>
inline void WeightedUndirectedMatrixGraph<T, W>::RemoveVertex(VertexPosType v)
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)//减去相关边的数量
		if (this->ExistEdge(v, i))
			--this->m_edgeNum;
	this->m_vertexData.erase(this->m_vertexData.begin() + v);
	/*将该顶点所在行列数据收缩*/
	/*如删除v1
	  1列 向上 + 3列 右下
		0          0          0          0
	  x	1 0        | \		  0 \		 0 0
		0 1 0      0 \ 0   	  0 \ 0      0 1 0
		0 0 1 0    0 \ 1 0	  1 \ 1 0	 1 0 1 0
		1 0 0 1 0  1   0 1 0      0 1 0
		  x

	删除v2
	  2列 向上 + 2列 右下
		0          0          0          0
		1 0        1 0		  1 0		 1 0
	  x	0 1 0      | | \	  0 0 \		 0 0 0
		0 0 1 0    0 0 \ 0	  1 0 \ 0	 1 0 1 0
		1 0 0 1 0  1 0   1 0        1 0
			x
	*/
	//收缩列数据
	for (VertexPosType i = 0; i < v; ++i)
		for (VertexPosType j = v; j < this->m_vertexData.size(); ++j)
			SetWeight(i, j, GetWeight(i, j + 1));
	//收缩右下数据
	for (VertexPosType i = v; i < this->m_vertexData.size(); ++i)
		for (VertexPosType j = i; j < this->m_vertexData.size(); ++j)
			SetWeight(i, j, GetWeight(i + 1, j + 1));
	//进行erase操作（虽然并不会真正释放vector内存，在类中已经提供一个操作来真正释放内存）
	m_adjaMetrix.resize((1 + this->m_vertexData.size()) * this->m_vertexData.size() / 2);
}

template<class T, class W>
inline std::vector<W> WeightedUndirectedMatrixGraph<T, W>::GetAdjacencyMatrix() const
{
	std::vector<W> adjaMetrix(this->m_vertexData.size() * this->m_vertexData.size(), (W)0);
	ForeachEdge(
		[&](auto v1, auto v2, auto w)
		{
			adjaMetrix[v1 * this->GetVertexNum() + v2] = w;
			adjaMetrix[v2 * this->GetVertexNum() + v1] = w;
		});
	return adjaMetrix;
}

template<class T, class W>
inline void WeightedUndirectedMatrixGraph<T, W>::ForeachEdge(OnPassEdge func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		for (VertexPosType j = i; j < this->m_vertexData.size(); ++j)
			if (this->ExistEdge(i, j))
				func(i, j, GetWeight(i, j));
}

template<class T, class W>
inline void WeightedUndirectedMatrixGraph<T, W>::Shrink_To_Fit()
{
	m_adjaMetrix.shrink_to_fit();
}

template<class T, class W>
inline unsigned long long WeightedUndirectedMatrixGraph<T, W>::GetMemoryUsage() const
{
	return (unsigned long long)m_adjaMetrix.size() * sizeof(W) + sizeof(m_adjaMetrix);
}

template<class T, class W>
inline constexpr bool WeightedUndirectedMatrixGraph<T, W>::IsDirected() const
{
	return false;
}

template<class T, class W>
inline constexpr bool WeightedUndirectedMatrixGraph<T, W>::IsWeighted() const
{
	return true;
}
