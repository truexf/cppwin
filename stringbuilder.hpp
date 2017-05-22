#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include <string>
#include <deque>

template <typename chr>
class StringBuilder {
    typedef std::basic_string<chr> string_t;
    typedef std::deque<string_t> container_t;
    typedef typename string_t::size_type size_type;
    container_t m_Data;
    size_type m_totalSize;
    void append(const string_t &src) {
        m_Data.push_back(src);
        m_totalSize += src.size();
    }
    // No copy constructor, no assignement.
    StringBuilder(const StringBuilder &);
    StringBuilder & operator = (const StringBuilder &);
public:
    StringBuilder(const string_t &src) {
        if (!src.empty()) {
            m_Data.push_back(src);
        }
        m_totalSize = src.size();
    }
    StringBuilder() {
        m_totalSize = 0;
    }

    StringBuilder & Append(const string_t &src) {
        append(src);
        return *this; // allow chaining.
    }
        // This one lets you add any STL container to the string builder. 
    template<class inputIterator>
    StringBuilder & Add(const inputIterator &first, const inputIterator &afterLast) {
        // std::for_each and a lambda look like overkill here.
                // <b>Not</b> using std::copy, since we want to update m_totalSize too.
        for (inputIterator f = first; f != afterLast; ++f) {
            append(*f);
        }
        return *this; // allow chaining.
    }

    // Like C# StringBuilder.ToString()
    // Note the use of reserve() to avoid reallocations. 
    string_t ToString() const {
        string_t result;
        // The whole point of the exercise!
        // If the container has a lot of strings,
        // reallocation (each time the result grows) will take a serious toll,
        // both in performance and chances of failure.
        // I measured (in code I cannot publish) fractions
        // of a second using 'reserve', and almost two minutes using +=.
        result.reserve(m_totalSize + 1);
        //    result = std::accumulate(m_Data.begin(), m_Data.end(), result);
              // This would lose the advantage of 'reserve'

        for (container::iterator iter = m_Data.begin(); iter != m_Data.end(); ++iter) { 
            result += *iter;
        }
        return result;
    }

    // like javascript Array.join()
    string_t Join(const string_t &delim) const {
        if (delim.empty()) {
            return ToString();
        }
        string_t result;
        if (m_Data.empty()) {
            return result;
        }
        // Hope we don't overflow the size type.
        size_type st = (delim.size() * (m_Data.size() - 1)) + m_totalSize + 1;
        result.reserve(st);
        container_t::iterator iter = m_Data.begin();
        result += *iter; 
        for(; iter != m_Data.end(); ++iter)
        {
            result += delim;
            result += *iter;
        }
        return result;
    }

}; // class StringBuilder

#endif