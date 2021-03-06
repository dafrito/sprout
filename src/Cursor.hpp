#ifndef SPROUT_CURSOR_HEADER
#define SPROUT_CURSOR_HEADER

#include <cassert>
#include <sstream>
#include <memory>
#include <cstring>

#include "StreamIterator.hpp"

namespace sprout {

template <class Data>
class CursorData
{
public:
    virtual Data get(int pos)=0;
    virtual bool atEnd()=0;
    virtual void advanceTo(int pos)=0;
    virtual int head() const=0;
};

template <class Data, class Iterator>
class IteratorCursorData : public CursorData<Data>
{
    Iterator _iter;
    Iterator _end;

    std::vector<Data> _buffer;
    int _head;

    Data next()
    {
        // Don't use post-increment for iterators that don't support copying
        Data value = *_iter;
        ++_iter;
        return value;
    }

public:
    IteratorCursorData(Iterator begin, Iterator end) :
        _iter(begin),
        _end(end),
        _head(0)
    {
    }

    int head() const
    {
        return _head;
    }

    int tail() const
    {
        return head() - buffered();
    }

    int buffered() const
    {
        return _buffer.size();
    }

    bool hasTokens()
    {
        return !atEnd();
    }

    std::string state() const
    {
        std::stringstream str;
        str << "[tail: " << tail() << ", head: " << head() << "]";
        return str.str();
    }

    Data get(int pos)
    {
        if (pos < 0) {
            std::stringstream str;
            str << "pos must be non-negative, but I was given " << pos << ". " << state();
            throw std::range_error(str.str());
        }
        if (pos < tail()) {
            std::stringstream str;
            str << "pos must not refer to discarded elements, but I was given " << pos << ". " << state();
            throw std::range_error(str.str());
        }
        advanceTo(pos);
        return _buffer.at(pos);
    }

    void advance()
    {
        if (atEnd()) {
            return;
        }
        _buffer.push_back(next());
        ++_head;
    }

    void advanceTo(int pos)
    {
        while (head() <= pos) {
            if (atEnd()) {
                return;
            }
            advance();
        }
    }

    bool atEnd()
    {
        return _iter == _end;
    }
};

class QStringCursorData : public CursorData<QChar>
{
    const QString& _str;

public:
    QStringCursorData(const QString& str) :
        _str(str)
    {
    }

    int head() const
    {
        return _str.size();
    }

    QChar get(int pos)
    {
        return _str.at(pos);
    }

    void advanceTo(int pos)
    {
        if (pos < 0) {
            std::stringstream str;
            str << "pos must be non-negative, but I was given " << pos << ". ";
            throw std::range_error(str.str());
        }
    }

    bool atEnd()
    {
        return true;
    }
};

class StdStringCursorData : public CursorData<char>
{
    const std::string& _str;

public:
    StdStringCursorData(const std::string& str) :
        _str(str)
    {
    }

    int head() const
    {
        return _str.size();
    }

    char get(int pos)
    {
        return _str.at(pos);
    }

    void advanceTo(int pos)
    {
        if (pos < 0) {
            std::stringstream str;
            str << "pos must be non-negative, but I was given " << pos << ". ";
            throw std::range_error(str.str());
        }
    }

    bool atEnd()
    {
        return true;
    }
};

template <class Data>
class Cursor
{
public:
    typedef Data value_type;

private:
    std::shared_ptr<CursorData<Data>> _data;
    int _pos;

public:
    Cursor(const Cursor<Data>& other) :
        _data(other._data),
        _pos(other._pos)
    {
    }

    template <class Iterator>
    Cursor(Iterator begin, Iterator end) :
        _data(new IteratorCursorData<Data, Iterator>(begin, end)),
        _pos(0)
    {
    }

    Cursor(CursorData<Data>* data) :
        _data(data),
        _pos(0)
    {
    }

    int pos() const
    {
        return _pos;
    }

    Data get()
    {
        return _data->get(pos());
    }

    Data operator*()
    {
        return get();
    }

    operator bool() const
    {
        return !_data->atEnd() || pos() < _data->head();
    }

    Cursor& operator++()
    {
        _data->advanceTo(++_pos);
        return *this;
    }

    Cursor operator++(int)
    {
        Cursor tmp(*this);
        ++(*this);
        return tmp;
    }

    Cursor& operator--()
    {
        return *this -= 1;
    }

    Cursor operator--(int)
    {
        Cursor tmp(*this);
        --(*this);
        return tmp;
    }

    Cursor& operator+=(const int delta)
    {
        _data->advanceTo(_pos += delta);
        return *this;
    }

    Cursor& operator-=(const int delta)
    {
        return *this += -delta;
    }

    Cursor operator+(const int delta) const
    {
        Cursor tmp(*this);
        tmp += delta;
        return tmp;
    }

    Cursor operator-(const int delta) const
    {
        Cursor tmp(*this);
        tmp -= delta;
        return tmp;
    }

    bool operator<(const Cursor& other) const
    {
        return _data == other._data && pos() < other.pos();
    }

    bool operator>(const Cursor& other) const
    {
        return _data == other._data && pos() > other.pos();
    }

    bool operator<=(const Cursor& other) const
    {
        return _data == other._data && pos() <= other.pos();
    }

    bool operator>=(const Cursor& other) const
    {
        return _data == other._data && pos() >= other.pos();
    }

    bool operator==(const Cursor& other) const
    {
        return _data == other._data && pos() == other.pos();
    }

    bool operator!=(const Cursor& other) const
    {
        return !(*this == other);
    }
};

template <class Data, class Stream>
Cursor<Data>
makeCursor(Stream* stream)
{
    return Cursor<Data>(
        StreamIterator<Data, Stream>(stream),
        StreamIterator<Data, Stream>()
    );
}

template <class Data>
Cursor<Data> makeCursor(const std::string* stream)
{
    return Cursor<Data>(
        new StdStringCursorData(*stream)
    );
}

template <class Data>
Cursor<Data> makeCursor(std::string* stream)
{
    return Cursor<Data>(
        new StdStringCursorData(*stream)
    );
}

template <class Data>
Cursor<Data> makeCursor(const char* stream)
{
    return Cursor<Data>(stream, stream + strlen(stream));
}

template <class Data>
Cursor<Data> makeCursor(char* stream)
{
    return Cursor<Data>(stream, stream + strlen(stream));
}

} // namespace sprout

#ifdef HAVE_QT_CORE

#include <QString>

namespace sprout {

template <class Data>
Cursor<Data> makeCursor(const QString* stream)
{
    return Cursor<Data>(new QStringCursorData(*stream));
}

template <class Data>
Cursor<Data> makeCursor(QString* stream)
{
    return Cursor<Data>(new QStringCursorData(*stream));
}

} // namespace sprout

#endif

#endif // SPROUT_CURSOR_HEADER

// vim: set ft=cpp ts=4 sw=4 :
