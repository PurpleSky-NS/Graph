#pragma once

#include <type_traits>

/*最小生成树基类，最小生成树为不可修改树，只能执行获取和清空操作*/
class MSTBase
{
public:

	/*获取顶点数量*/
	virtual size_t GetVertexNum()const = 0;

	/*获取顶点数量*/
	virtual void Clear() = 0;

	/*该树是否为空*/
	virtual bool IsEmpty() = 0;

private:

	template<class T, class W, W NullValue>
	friend class MatrixGraph;

	/*设置顶点数量，作为初始化*/
	virtual void SetVertexNum(size_t size) = 0;

	/*设置双亲结点*/
	virtual void SetParent(size_t vertex, size_t parent) = 0;
};

/*双亲表示树，所有操作复杂度都是O(1)，只能查找某一结点的双亲，存储和查找效率都很高，不能查找孩子和兄弟
模板T为下标类型，只能为整形，类型越小所占空间越小*/
template<class T>
class MST_Parent :public MSTBase
{
public:

	static_assert(std::is_integral<T>::value, "类型T必须为整形");

	/*NPOS表示无效顶点下标*/
	static constexpr T NPOS = static_cast<T>(-1);

	MST_Parent() = default;
	MST_Parent(const MST_Parent&) = delete;
	MST_Parent(MST_Parent&&);
	virtual ~MST_Parent();

	/*获取顶点数量*/
	virtual size_t GetVertexNum()const;

	/*获取顶点数量*/
	virtual void Clear();

	/*该树是否为空*/
	virtual bool IsEmpty();

	/*获取双亲节点，如果是根节点或者vertex越界，返回NPOS*/
	T GetParent(T vertex)const;

private:

	T* m_vertex = nullptr;
	size_t m_size;

	/*设置顶点数量，作为初始化*/
	virtual void SetVertexNum(size_t size);

	/*设置双亲结点*/
	virtual void SetParent(size_t vertex, size_t parent);

};

template<class T>
inline MST_Parent<T>::MST_Parent(MST_Parent&& mst)
{
	m_vertex = mst.m_vertex;
	mst.m_vertex = nullptr;
	m_size = mst.m_size;
	mst.m_size = 0;
}

template<class T>
inline MST_Parent<T>::~MST_Parent()
{
	Clear();
}

template<class T>
inline size_t MST_Parent<T>::GetVertexNum() const
{
	return m_size;
}

template<class T>
inline void MST_Parent<T>::Clear()
{
	if (m_vertex != nullptr)
		delete[] m_vertex;
	m_vertex = nullptr;
	m_size = 0;
}

template<class T>
inline bool MST_Parent<T>::IsEmpty()
{
	return m_size == 0;
}

template<class T>
inline T MST_Parent<T>::GetParent(T vertex) const
{
	return (vertex < m_size ? m_vertex[vertex] : (T)0 - (T)1);
}

template<class T>
inline void MST_Parent<T>::SetVertexNum(size_t size)
{
	Clear();
	if (!size)
		return;
	m_size = size;
	m_vertex = new T[size];
}

template<class T>
inline void MST_Parent<T>::SetParent(size_t vertex, size_t parent)
{
	if (vertex < m_size)
		m_vertex[vertex] = (parent == static_cast<size_t>(-1) ? NPOS : (T)parent);
}
