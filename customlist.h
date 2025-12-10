// customlist.h
#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDataStream>
#include <QList>  // Добавляем для совместимости
#include <algorithm>

template<typename T>
class CustomList
{
public:
    // Конструкторы
    CustomList();
    CustomList(const CustomList<T> &other);
    CustomList(const QList<T> &other);  // НОВЫЙ: конструктор из QList
    ~CustomList();

    // Операторы присваивания
    CustomList<T> &operator=(const CustomList<T> &other);
    CustomList<T> &operator=(const QList<T> &other);  // НОВЫЙ: присваивание из QList

    // Операторы сравнения
    bool operator==(const CustomList<T> &other) const;

    // Методы доступа к элементам
    T &operator[](int i);
    const T &operator[](int i) const;
    T &at(int i);
    const T &at(int i) const;
    T &first();
    const T &first() const;
    T &last();
    const T &last() const;

    // Информация о списке
    bool isEmpty() const;
    int size() const;
    int count() const;
    bool contains(const T &value) const;
    int indexOf(const T &value) const;

    // Изменение списка
    void append(const T &value);
    void insert(int i, const T &value);
    void removeAt(int i);
    bool removeOne(const T &value);
    void clear();

    // STL-совместимые итераторы
    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    // Методы для совместимости с findChildren
    void fromQList(const QList<T> &qlist);  // НОВЫЙ: преобразование из QList
    QList<T> toQList() const;  // НОВЫЙ: преобразование в QList

    // Методы, специфичные для проекта
    void swap(CustomList<T> &other);

private:
    void reallocate(int newCapacity);

    T *m_data = nullptr;
    int m_size = 0;
    int m_capacity = 0;
};

// Реализация шаблона

template<typename T>
CustomList<T>::CustomList() : m_data(nullptr), m_size(0), m_capacity(0) {}

template<typename T>
CustomList<T>::CustomList(const CustomList<T> &other)
    : m_data(nullptr), m_size(0), m_capacity(0)
{
    if (other.m_size > 0) {
        m_data = new T[other.m_size];
        for (int i = 0; i < other.m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
        m_size = other.m_size;
        m_capacity = other.m_size;
    }
}

// НОВЫЙ: Конструктор из QList
template<typename T>
CustomList<T>::CustomList(const QList<T> &other)
    : m_data(nullptr), m_size(0), m_capacity(0)
{
    fromQList(other);
}

template<typename T>
CustomList<T>::~CustomList()
{
    delete[] m_data;
}

template<typename T>
CustomList<T> &CustomList<T>::operator=(const CustomList<T> &other)
{
    if (this != &other) {
        delete[] m_data;
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;

        if (other.m_size > 0) {
            m_data = new T[other.m_size];
            for (int i = 0; i < other.m_size; ++i) {
                m_data[i] = other.m_data[i];
            }
            m_size = other.m_size;
            m_capacity = other.m_size;
        }
    }
    return *this;
}

// НОВЫЙ: Оператор присваивания из QList
template<typename T>
CustomList<T> &CustomList<T>::operator=(const QList<T> &other)
{
    fromQList(other);
    return *this;
}

template<typename T>
bool CustomList<T>::operator==(const CustomList<T> &other) const
{
    if (m_size != other.m_size) return false;
    for (int i = 0; i < m_size; ++i) {
        if (m_data[i] != other.m_data[i]) return false;
    }
    return true;
}

template<typename T>
T &CustomList<T>::operator[](int i)
{
    return m_data[i];
}

template<typename T>
const T &CustomList<T>::operator[](int i) const
{
    return m_data[i];
}

template<typename T>
T &CustomList<T>::at(int i)
{
    return m_data[i];
}

template<typename T>
const T &CustomList<T>::at(int i) const
{
    return m_data[i];
}

template<typename T>
T &CustomList<T>::first()
{
    return m_data[0];
}

template<typename T>
const T &CustomList<T>::first() const
{
    return m_data[0];
}

template<typename T>
T &CustomList<T>::last()
{
    return m_data[m_size - 1];
}

template<typename T>
const T &CustomList<T>::last() const
{
    return m_data[m_size - 1];
}

template<typename T>
bool CustomList<T>::isEmpty() const
{
    return m_size == 0;
}

template<typename T>
int CustomList<T>::size() const
{
    return m_size;
}

template<typename T>
int CustomList<T>::count() const
{
    return m_size;
}

template<typename T>
bool CustomList<T>::contains(const T &value) const
{
    return indexOf(value) != -1;
}

template<typename T>
int CustomList<T>::indexOf(const T &value) const
{
    for (int i = 0; i < m_size; ++i) {
        if (m_data[i] == value) return i;
    }
    return -1;
}

template<typename T>
void CustomList<T>::append(const T &value)
{
    if (m_size >= m_capacity) {
        int newCapacity = (m_capacity == 0) ? 4 : m_capacity * 2;
        reallocate(newCapacity);
    }
    m_data[m_size++] = value;
}

template<typename T>
void CustomList<T>::insert(int i, const T &value)
{
    if (m_size >= m_capacity) {
        int newCapacity = (m_capacity == 0) ? 4 : m_capacity * 2;
        reallocate(newCapacity);
    }

    for (int j = m_size; j > i; --j) {
        m_data[j] = m_data[j - 1];
    }
    m_data[i] = value;
    m_size++;
}

template<typename T>
void CustomList<T>::removeAt(int i)
{
    for (int j = i; j < m_size - 1; ++j) {
        m_data[j] = m_data[j + 1];
    }
    m_size--;
}

template<typename T>
bool CustomList<T>::removeOne(const T &value)
{
    int index = indexOf(value);
    if (index != -1) {
        removeAt(index);
        return true;
    }
    return false;
}

template<typename T>
void CustomList<T>::clear()
{
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

template<typename T>
typename CustomList<T>::iterator CustomList<T>::begin()
{
    return m_data;
}

template<typename T>
typename CustomList<T>::const_iterator CustomList<T>::begin() const
{
    return m_data;
}

template<typename T>
typename CustomList<T>::iterator CustomList<T>::end()
{
    return m_data + m_size;
}

template<typename T>
typename CustomList<T>::const_iterator CustomList<T>::end() const
{
    return m_data + m_size;
}

// НОВЫЙ: Преобразование из QList
template<typename T>
void CustomList<T>::fromQList(const QList<T> &qlist)
{
    clear();
    if (!qlist.isEmpty()) {
        m_data = new T[qlist.size()];
        for (int i = 0; i < qlist.size(); ++i) {
            m_data[i] = qlist[i];
        }
        m_size = qlist.size();
        m_capacity = qlist.size();
    }
}

// НОВЫЙ: Преобразование в QList
template<typename T>
QList<T> CustomList<T>::toQList() const
{
    QList<T> result;
    for (int i = 0; i < m_size; ++i) {
        result.append(m_data[i]);
    }
    return result;
}

template<typename T>
void CustomList<T>::swap(CustomList<T> &other)
{
    std::swap(m_data, other.m_data);
    std::swap(m_size, other.m_size);
    std::swap(m_capacity, other.m_capacity);
}

template<typename T>
void CustomList<T>::reallocate(int newCapacity)
{
    T *newData = new T[newCapacity];
    for (int i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];
    }

    delete[] m_data;
    m_data = newData;
    m_capacity = newCapacity;
}

// Сериализация
template<typename U>
QDataStream &operator<<(QDataStream &out, const CustomList<U> &list)
{
    out << static_cast<quint32>(list.m_size);
    for (int i = 0; i < list.m_size; ++i) {
        out << list.m_data[i];
    }
    return out;
}

template<typename U>
QDataStream &operator>>(QDataStream &in, CustomList<U> &list)
{
    list.clear();
    quint32 size;
    in >> size;

    if (size > list.m_capacity) {
        delete[] list.m_data;
        list.m_data = new U[size];
        list.m_capacity = size;
    }

    for (quint32 i = 0; i < size; ++i) {
        U value;
        in >> value;
        list.m_data[i] = value;
    }
    list.m_size = size;

    return in;
}

#endif // CUSTOMLIST_H
