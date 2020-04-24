#pragma once

#include "MatrixGraph.h"

/*有向邻接矩阵图，内存占用较大*/
template<class T, class W = int, W NullValue = -1>
class WeightedDirectedMatrixGraph :public MatrixGraph<T, W, NullValue>
{
public:
	using typename GraphBase<T, W, NullValue>::VertexType;
	using typename GraphBase<T, W, NullValue>::WeightType;
	using typename GraphBase<T, W, NullValue>::VertexPosType;
	using typename GraphBase<T, W, NullValue>::OnPassVertex;
	using typename GraphBase<T, W, NullValue>::OnPassEdge;

	/*插入一个顶点 O(VertexNum)*/
	virtual void InsertVertex(const T& v)override;

	/*获取从from到to的权重 O(1)*/
	virtual W GetWeight(VertexPosType from, VertexPosType to)const override;

	/*设置从from到to的权重 weight=NullValue删除该边，如果没有则添加 O(1)*/
	virtual void SetWeight(VertexPosType from, VertexPosType to, const W& weight);

	/*删除顶点，删完后下标会改变 O(1)-O(Ele) (下标越大速度越快)*/
	virtual void RemoveVertex(VertexPosType v)override;

	/*遍历所有边 O(Ele)*/
	virtual void ForeachEdge(OnPassEdge func)const override;

	/*获取完整邻接矩阵，二维的邻接矩阵会以行为单位，存储在一维线性表中 O(Ele-) (经过vector优化过应该介于Ele和VertexNum之间)*/
	virtual std::vector<W> GetAdjacencyMatrix()const override;

	/*收缩内存占用，在每次移除一个顶点时并不会真的释放内存，从而提高再次插入顶点效率，详见@vector
	如果不需要插入顶点或者需要收缩内存，请调用这个
	内部调用@vector.shrink_to_fit*/
	virtual void Shrink_To_Fit()override;

	/*实际占用要大于该数值，因为vector会预留空间，调用Shrink_To_Fit函数后可能更趋近于该数值*/
	virtual unsigned long long GetMemoryUsage()const override;

protected:
	std::vector<std::vector<W>> m_adjaMetrix;
};

template<class T, class W, W NullValue>
inline void WeightedDirectedMatrixGraph<T, W, NullValue>::InsertVertex(const T& v)
{
	this->m_vertexData.push_back(v);
	for (auto& i : m_adjaMetrix) //扩展一列
		i.push_back(NullValue);
	std::vector<W> last(this->GetVertexNum(), NullValue);
	m_adjaMetrix.push_back(std::move(last)); //扩展一行
}

template<class T, class W, W NullValue>
inline W WeightedDirectedMatrixGraph<T, W, NullValue>::GetWeight(VertexPosType from, VertexPosType to)const
{
	return m_adjaMetrix[from][to];
}

template<class T, class W, W NullValue>
inline void WeightedDirectedMatrixGraph<T, W, NullValue>::SetWeight(VertexPosType from, VertexPosType to, const W& weight)
{
	m_adjaMetrix[from][to] = weight;
}

template<class T, class W, W NullValue>
inline void WeightedDirectedMatrixGraph<T, W, NullValue>::RemoveVertex(VertexPosType v)
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)//减去相关边的数量
		if (this->ExistEdge(v, i))
			--this->m_edgeNum;
	this->m_vertexData.erase(this->m_vertexData.begin() + v);
	m_adjaMetrix.erase(m_adjaMetrix.begin() + v);
	for (auto& i : m_adjaMetrix)
		i.erase(i.begin() + v);
}

template<class T, class W, W NullValue>
inline void WeightedDirectedMatrixGraph<T, W, NullValue>::ForeachEdge(OnPassEdge func) const
{
	for (VertexPosType i = 0; i < this->m_vertexData.size(); ++i)
		for (VertexPosType j = 0; j < this->m_vertexData.size(); ++j)
			if (this->ExistEdge(i, j))
				func(i, j, GetWeight(i, j));
}

template<class T, class W, W NullValue>
inline std::vector<W> WeightedDirectedMatrixGraph<T, W, NullValue>::GetAdjacencyMatrix() const
{
	//事实证明在图比较大的时候，遍历边是很慢的
	std::vector<W> adja;
	adja.reserve(this->GetVertexNum() * this->GetVertexNum());
	for (auto& i : m_adjaMetrix)
		adja.insert(adja.end(), i.begin(), i.end());
	return adja;
}

template<class T, class W, W NullValue>
inline void WeightedDirectedMatrixGraph<T, W, NullValue>::Shrink_To_Fit()
{
	m_adjaMetrix.shrink_to_fit();
	for (auto& i : m_adjaMetrix)
		i.shrink_to_fit();
}

template<class T, class W, W NullValue>
inline unsigned long long WeightedDirectedMatrixGraph<T, W, NullValue>::GetMemoryUsage() const
{
	return this->GetVertexNum() * ((unsigned long long)this->GetVertexNum() * sizeof(W) + sizeof(std::vector<W>)) + sizeof(m_adjaMetrix);
}
