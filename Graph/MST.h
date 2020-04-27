#pragma once

#include <type_traits>
#include <vector>

/*双亲表示树，简单包装了一下vector，所有操作复杂度都是O(1)，只能查找某一结点的双亲，存储和查找效率都很高，不能查找孩子和兄弟
模板PT为顶点下标类型，只能为整形，类型越小占用的空间越小
模板WT为计算出来的总权重类型，只能为算数类型
*/
template<class PT, class WT>
class MST_Parent
{
public:

	static_assert(std::is_arithmetic<WT>::value, "类型WT必须为算数类型");
	static_assert(std::is_integral<PT>::value, "类型PT必须为整型");
	static_assert(sizeof(PT) <= sizeof(size_t), "类型PT太大了，不需要这么大");

	/*获取顶点数量*/
	size_t GetVertexNum()const;

	/*清空*/
	void Clear();

	/*该树是否为空*/
	bool IsEmpty()const;

	/*获取双亲节点，如果是根节点返回vertexSize，如果vertex越界，返回NPOS*/
	PT GetParent(PT vertex)const;

	/*获取总权值*/
	WT GetTotalWeight()const;

	/*获取数据容器*/
	const std::vector<PT>& GetData()const;

private:

	template<class X, class Y, Y Z>
	friend class MatrixGraph;

	WT m_totalWeight = 0;
	std::vector<PT> m_vertexes;

	/*设置顶点数量，作为初始化*/
	void SetVertexNum(size_t size);

	/*设置双亲结点，parent=VertexSize表示根节点*/
	void SetParent(PT vertex, PT parent);

	/*累加总权值*/
	void AddWeight(WT w);

};

template<class PT, class WT>
inline size_t MST_Parent<PT, WT>::GetVertexNum() const
{
	return m_vertexes.size();
}

template<class PT, class WT>
inline void MST_Parent<PT, WT>::Clear()
{
	m_vertexes.clear();
	m_vertexes.shrink_to_fit();
	m_totalWeight = 0;
}

template<class PT, class WT>
inline bool MST_Parent<PT, WT>::IsEmpty()const
{
	return m_vertexes.empty();
}

template<class PT, class WT>
inline PT MST_Parent<PT, WT>::GetParent(PT vertex) const
{
	return m_vertexes[vertex];
}

template<class PT, class WT>
inline WT MST_Parent<PT, WT>::GetTotalWeight() const
{
	return m_totalWeight;
}

template<class PT, class WT>
inline const std::vector<PT>& MST_Parent<PT, WT>::GetData() const
{
	return m_vertexes;
}

template<class PT, class WT>
inline void MST_Parent<PT, WT>::SetVertexNum(size_t size)
{
	Clear();
	m_vertexes.resize(size);
}

template<class PT, class WT>
inline void MST_Parent<PT, WT>::SetParent(PT vertex, PT parent)
{
	m_vertexes[vertex] = parent;
}

template<class PT, class WT>
inline void MST_Parent<PT, WT>::AddWeight(WT w)
{
	m_totalWeight += w;
}

/*边集
边结构体内存分布为：PT PT W
模板PT为顶点下标类型，只能为整形，类型越小占用的空间越小
模板WT为计算出来的总权重类型，只能为算数类型
模板W为边的权重类型，不用手动设置
*/
template<class PT, class WT, class W>
class MST_Edge
{
	struct Edge
	{
		PT v1, v2;
		W w;
		Edge(PT v1, PT v2, W w) :
			v1(v1), v2(v2), w(w) {}
	};

public:

	static_assert(std::is_arithmetic<WT>::value, "类型WT必须为算数类型");
	static_assert(std::is_arithmetic<W>::value, "类型W必须为算数类型");
	static_assert(std::is_integral<PT>::value, "类型PT必须为整型");

	/*获取顶点数量*/
	size_t GetEdgeNum()const;

	/*清空*/
	void Clear();

	/*该树是否为空*/
	bool IsEmpty()const;

	/*获取总权值*/
	WT GetTotalWeight()const;

	/*获取数据容器*/
	void Foreach(std::function<void(PT, PT, W)> func)const;

	/*获取数据容器*/
	const std::vector<Edge>& GetData()const;

private:

	template<class X, class E, class Y, Y Z>
	friend class UnweightedDirectedLinkGraph;

	WT m_totalWeight = 0;
	std::vector<Edge> m_edges;

	/*设置顶点数量，作为初始化*/
	void SetEdgeNum(size_t size);

	/*设置双亲结点，parent=VertexSize表示根节点*/
	void AddEdge(PT v1, PT v2, W w);

	/*累加总权值*/
	void AddWeight(WT w);

};

template<class PT, class WT, class W>
inline size_t MST_Edge<PT, WT, W>::GetEdgeNum() const
{
	return m_edges.size();
}

template<class PT, class WT, class W>
inline void MST_Edge<PT, WT, W>::Clear()
{
	m_edges.clear();
	m_edges.shrink_to_fit();
	m_totalWeight = 0;
}

template<class PT, class WT, class W>
inline bool MST_Edge<PT, WT, W>::IsEmpty()const
{
	return m_edges.empty();
}

template<class PT, class WT, class W>
inline WT MST_Edge<PT, WT, W>::GetTotalWeight() const
{
	return m_totalWeight;
}

template<class PT, class WT, class W>
inline void MST_Edge<PT, WT, W>::Foreach(std::function<void(PT, PT, W)> func) const
{
	for (auto i : m_edges)
		func(i.v1, i.v2, i.w);
}

template<class PT, class WT, class W>
inline const std::vector<typename MST_Edge<PT, WT, W>::Edge>& MST_Edge<PT, WT, W>::GetData() const
{
	return m_edges;
}

template<class PT, class WT, class W>
inline void MST_Edge<PT, WT, W>::SetEdgeNum(size_t size)
{
	Clear();
	m_edges.reserve(size);
}

template<class PT, class WT, class W>
inline void MST_Edge<PT, WT, W>::AddEdge(PT v1, PT v2, W w)
{
	m_edges.emplace_back(v1, v2, w);
}

template<class PT, class WT, class W>
inline void MST_Edge<PT, WT, W>::AddWeight(WT w)
{
	m_totalWeight += w;
}

/*并查集，用来实现Kruskal算法*/
class MST_SearchUnion
{
public:

	inline MST_SearchUnion() = default;
	inline MST_SearchUnion(size_t size);
	inline MST_SearchUnion(const MST_SearchUnion&) = delete;
	inline MST_SearchUnion(MST_SearchUnion&&) = default;
	inline ~MST_SearchUnion();

	/*从0--size-1*/
	inline void Init(size_t size);

	/*合并*/
	inline void Unite(size_t x, size_t y);

	/*多调用这个函数效率会更快哦*/
	inline size_t  FindRoot(size_t x);

	inline bool Same(size_t x, size_t y);

	inline void Clear();

private:
	size_t* m_data = nullptr;
};
inline MST_SearchUnion::MST_SearchUnion(size_t size)
{
	Init(size);
}
inline MST_SearchUnion::~MST_SearchUnion()
{
	Clear();
}
inline void MST_SearchUnion::Init(size_t size)
{
	Clear();
	m_data = new unsigned[size];
	for (unsigned i = 0; i < size; ++i)
		m_data[i] = i;
}
inline void MST_SearchUnion::Unite(size_t x, size_t y)
{
	unsigned x_root = FindRoot(x);
	unsigned y_root = FindRoot(y);

	if (x_root == y_root)
		return;

	if (x_root > y_root)
		m_data[x] = m_data[x_root] = y_root;
	else
		m_data[y] = m_data[y_root] = x_root;
}
inline size_t  MST_SearchUnion::FindRoot(size_t x)
{
	unsigned fd = x;
	while (m_data[fd] != fd)
		fd = m_data[fd];
	return m_data[x] = fd;
}
inline bool MST_SearchUnion::Same(size_t x, size_t y)
{
	return FindRoot(x) == FindRoot(y);
}
inline void MST_SearchUnion::Clear()
{
	if (m_data != nullptr)
		delete[] m_data;
	m_data = nullptr;
}
