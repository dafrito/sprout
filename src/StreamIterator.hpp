#ifndef SPROUT_STREAMITERATOR_HEADER
#define SPROUT_STREAMITERATOR_HEADER

#include "config.hpp"

#ifdef HAVE_QT_CORE
#include <QTextStream>
#endif 

namespace sprout {

template <class Stream>
struct Evaluator
{
    /**
     * Whether the end returns true for the last element (e.g., Qt), or one
     * past the last element (e.g., STL)
     */
    static bool isEndInclusive()
    {
        return false;
    }

    /**
     * Returns true if the stream is at the end, according to the stream.
     */
    static bool atEnd(Stream& stream)
    {
        return !stream;
    }
};

#ifdef HAVE_QT_CORE

template <>
struct Evaluator<QTextStream>
{
    static bool isEndInclusive()
    {
        return true;
    }

    static bool atEnd(QTextStream& stream)
    {
        return stream.atEnd();
    }
};


#endif

enum class StreamIteratorState
{
    /**
     * We still have at least one element that is available for reading, beyond our current
     * position.
     */
    Normal,

    /**
     * We're currently on the last element, so no more elements can be read. STL streams (and
     * streams that behave like the STL) do not use this state, since they indicate the "end"
     * once they've iterated past the end of the stream.
     */
    AtLastElement,

    /**
     * We have no more elements to read, and the iterator is essentially invalid.
     */
    PastLastElement
};

template <class Data, class Stream>
class StreamIterator
{
    Stream* _stream;
    StreamIteratorState _state;
    Data _data;

    void fetch()
    {
        if (!_stream || _state == StreamIteratorState::PastLastElement) {
            return;
        }

        *_stream >> _data;

        if (Evaluator<Stream>::atEnd(*_stream)) {
            switch (_state) {
                case StreamIteratorState::Normal:
                    if (Evaluator<Stream>::isEndInclusive()) {
                        _state = StreamIteratorState::AtLastElement;
                    } else {
                        _state = StreamIteratorState::PastLastElement;
                    }
                    break;
                case StreamIteratorState::AtLastElement:
                    _state = StreamIteratorState::PastLastElement;
                    break;
                case StreamIteratorState::PastLastElement:
                    throw std::logic_error("State must never be in PastLastElement and succeed in a fetch");
                default:
                    throw std::logic_error("Impossible state");
            }
        } else {
            _state = StreamIteratorState::Normal;
        }
    }

public:
    StreamIterator() :
        _stream(nullptr),
        _state(StreamIteratorState::PastLastElement)
    {
    }

    StreamIterator(Stream* stream) :
        _stream(stream)
    {
        if (Evaluator<Stream>::atEnd(*_stream)) {
            _state = StreamIteratorState::PastLastElement;
        } else {
            _state = StreamIteratorState::Normal;
            fetch();
        }
    }

    bool operator==(const StreamIterator& other) const
    {
        if (!_stream) {
            return !other._stream || !other;
        }
        if (!other._stream) {
            return !(*this);
        }
        return _stream == other._stream;
    }

    operator bool() const
    {
        if (!_stream) {
            return false;
        }
        return _state != StreamIteratorState::PastLastElement;
    }

    Data operator*()
    {
        if (_state == StreamIteratorState::PastLastElement) {
            throw std::runtime_error("No more data is available");
        }
        return _data;
    }

    StreamIterator& operator++()
    {
        fetch();
        return *this;
    }
};

} // namespace sprout

#endif // SPROUT_STREAMITERATOR_HEADER

// vim: set ft=cpp ts=4 sw=4 :
