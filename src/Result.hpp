#ifndef SPROUT_RESULT_HEADER
#define SPROUT_RESULT_HEADER

#include <vector>
#include <iterator>
#include <algorithm>

#include "Cursor.hpp"

namespace sprout {

/**
 * Result is used as a container of tokens in Sprout. I wish I could
 * have used a STL container instead of using a wrapper, but I desired
 * something that could both act as a container, as well as an iterator.
 */
template <class Token>
class Result : public std::iterator<std::input_iterator_tag, Token>
{
    typedef std::vector<Token> Tokens;
    Tokens _data;
    mutable int _pos;

    int _insertionPos;

    bool _suppress;

public:
    Result() :
        _data(),
        _pos(0),
        _insertionPos(0),
        _suppress(false)
    {
    }

    Result(const Tokens& tokens) :
        _data(tokens),
        _pos(0),
        _insertionPos(0),
        _suppress(false)
    {
    }

    template <class Iterator>
    Result(Iterator begin, Iterator end) :
        Result(Tokens())
    {
        insert(begin, end);
    }

    template <class T>
    Result(const Result<T>& begin, const Result<T>& end) :
        _data(*begin._data),
        _pos(0)
    {
    }

    void suppress(const bool suppressed = true)
    {
        _suppress = suppressed;
    }

    bool suppressed() const
    {
        return _suppress;
    }

    const Tokens empty_tokens;

    Tokens data() const
    {
        return _data;
    }

    typename Tokens::const_iterator begin() const
    {
        return std::begin(_data);
    }

    typename Tokens::const_iterator end() const
    {
        return std::end(_data);
    }

    typename Tokens::iterator begin()
    {
        return std::begin(_data);
    }

    typename Tokens::iterator end()
    {
        return std::end(_data);
    }

    int pos() const
    {
        return _pos;
    }

    void moveHead(const int pos)
    {
        _insertionPos = pos >= _data.size() ? _data.size() : pos;
    }

    int head() const
    {
        return _insertionPos;
    }

    int size() const
    {
        return head() - pos();
    }

    template <class T>
    void insert(const T& value)
    {
        if (suppressed()) {
            return;
        }
        if (_insertionPos >= _data.size()) {
            _data.push_back(value);
        } else {
            _data[_insertionPos] = value;
        }
        ++_insertionPos;
    }

    template <class T>
    void insert(const Result<T>& value)
    {
        if (suppressed()) {
            return;
        }
        for (auto v : value) {
            insert(v);
        }
    }

    template <class Iterator>
    void insert(Iterator begin, Iterator end)
    {
        if (suppressed()) {
            return;
        }
        for (auto i = begin; i != end; ++i) {
            insert(*i);
        }
    }

    void clear()
    {
        _data.clear();
        _pos = 0;
        _insertionPos = 0;
    }

    operator bool() const
    {
        return !_data.empty() && _pos < head();
    }

    const Token& get() const
    {
        if (_pos >= head()) {
            throw std::runtime_error("Position must not be dereferenced out of range");
        }
        return _data[_pos];
    }

    Token& get()
    {
        if (_pos >= head()) {
            throw std::runtime_error("Position must not be dereferenced out of range");
        }
        return _data[_pos];
    }

    const Token operator*() const
    {
        // Convert to a value to ensure the memory is not destroyed if our vector is reallocated
        return get();
    }

    const Token* operator->() const
    {
        return &get();
    }

    Token& operator[](const int pos)
    {
        *this += pos;
        Token& ref = get();
        *this -= pos;
        return ref;
    }

    const Result<Token>& operator++() const
    {
        ++_pos;
        return *this;
    }

    const Result<Token> operator++(int) const
    {
        Result<Token> tmp(*this);
        ++(*this);
        return tmp;
    }

    const Result<Token>& operator+=(const int delta) const
    {
        _pos += delta;
        return *this;
    }

    const Result<Token>& operator-=(const int delta) const
    {
        return *this += -delta;
    }

    const Result<Token> operator+(const int delta) const
    {
        Result<Token> tmp(*this);
        tmp += delta;
        return tmp;
    }

    const Result<Token> operator-(const int delta) const
    {
        Result<Token> tmp(*this);
        tmp -= delta;
        return tmp;
    }

    template <class T>
    bool operator<(const Result<T>& other) const
    {
        return _data == other._data && pos() < other.pos();
    }

    template <class T>
    bool operator>(const Result<T>& other) const
    {
        return _data == other._data && pos() > other.pos();
    }

    template <class T>
    bool operator<=(const Result<T>& other) const
    {
        return _data == other._data && pos() <= other.pos();
    }

    template <class T>
    bool operator>=(const Result<T>& other) const
    {
        return _data == other._data && pos() >= other.pos();
    }

    template <class T>
    bool operator==(const Result<T>& other) const
    {
        if (!*this && !other) {
            return true;
        }
        return _data == other._data && pos() == other.pos();
    }

    template <class T>
    bool operator!=(const Result<T>& other) const
    {
        return !(*this == other);
    }
};

template <class Token, class T>
Result<Token>& operator<<(Result<Token>& result, const T& value)
{
    result.insert(value);
    return result;
}

template <class Data, class Token>
Cursor<Data> makeCursor(Result<Token>& stream)
{
    return Cursor<Data>(stream.begin(), stream.end());
}

template <class Data, class Token>
Cursor<Data> makeCursor(const Result<Token>& stream)
{
    return Cursor<Data>(stream.begin(), stream.end());
}

} // namespace sprout

#endif // SPROUT_RESULT_HEADER

// vim: set ft=cpp ts=4 sw=4 :
