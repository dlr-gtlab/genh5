
#include "gtest/gtest.h"
#include "testhelper.h"

#include <QDebug>
#include <QPoint>
#include <QPointF>

#include "genh5.h"
#include "genh5_data.h"

class Tests : public testing::Test
{

};

TEST_F(Tests, filterDeflate)
{
    bool isAvail = H5Zfilter_avail(H5Z_FILTER_DEFLATE);
    qDebug() << "H5Zfilter_avail(H5Z_FILTER_DEFLATE):" << isAvail;
#if 0
    EXPECT_TRUE(isAvail);
#endif
}

#if 0
TEST_F(Tests, test)
{
    H5::DSetCreatPropList props;
    props.setDeflate(9);

    uint flags{};       // 0 = mandatory, 1 = optional in pipeline
    size_t len{1};      // length of the filter data, for "deflate" we require only one entry
    uint compression{}; // "compression level" is the first entry of the user data
    char name[30];      // name of filter
    uint config{};      // filter config - not relevant?

    int nfilter = H5Pget_nfilters(props.getId());

    qDebug() << "Number of filters:" << nfilter;

    herr_t err = H5Pget_filter_by_id(props.getId(), H5Z_FILTER_DEFLATE, &flags,
                                     &len, &compression, 30, name, &config);

    qDebug() << "Error:" << err;
    qDebug() << "Is Optional:" << flags;
    qDebug() << "Compression:" << compression;
    qDebug() << "Filter Name:" << name;
    qDebug() << "Config:" << config;
}

#endif

TEST_F(Tests, varlen_test)
{
    using GenH5::VarLen;
    using GenH5::Array;

    GenH5::Data<VarLen<char>> data{};
    data.append(VarLen<char>{0, 2, 4});

    QByteArray filePath = h5TestHelper->tempPath().toUtf8() + "/varlen_test.h5";

    GenH5::File file{filePath, GenH5::Overwrite};

    file.root().writeDataSet("test", data);

    auto s = file.root().readDataSet<VarLen<char>>("test");

    EXPECT_EQ(s.values(), data.values());
}

TEST_F(Tests, fixed_string)
{
    QByteArray filePath = h5TestHelper->tempPath().toUtf8() + "/fixed_string_test.h5";

    GenH5::File file{filePath, GenH5::Overwrite};

    file.root().writeDataSet("test", GenH5::makeFixedStr("Hello World"));

    GenH5::FixedString0D s = file.root().readDataSet<GenH5::FixedString0D>("test");

    EXPECT_STREQ(s.data(), "Hello World");
}

#if 0 // VarLen test

namespace GenH5
{

template<typename T>
class VarLen
{
public:

    using value_type            = T;
    using container_type        = Vector<value_type>;

    using iterator              = typename container_type::iterator;
    using const_iterator        = typename container_type::const_iterator;

    using pointer               = value_type*;
    using const_pointer         = value_type const*;
    using reference             = value_type&;
    using const_reference       = value_type const&;
    using size_type             = typename container_type::size_type;

    VarLen() = default;
    VarLen(std::initializer_list<T> list) : m_data{std::move(list)} {}
    VarLen(T t) : m_data{std::move(t)} {}
    VarLen(T&& t) : m_data{std::move(t)} {}
    template<typename Iter>
    VarLen(Iter begin, Iter end) : m_data{begin, end} {};

    /** index operator **/
    reference operator[](size_type i) { return m_data[i]; }

    void push_back(T const& t) { m_data.push_back(t); }
    void push_back(T&& t) { m_data.push_back(std::move(t)); }
    void push_back(container_type const& c) { m_data.append(c); }
    void push_back(container_type&& c) { m_data.append(std::move(c)); }

    /** STL **/
    void clear() { m_data.clear(); }
    bool empty() const { return m_data.empty(); }
    size_type size() const { return m_data.size(); }
    size_type capacity() const { return m_data.capacity(); }

    void reserve(size_type len) { return m_data.reserve(len); }

    value_type* data() { return m_data.data(); }
    value_type const* data() const { return m_data.data(); }

    reference front() { return m_data.front(); }
    const_reference front() const { return m_data.front(); }
    reference back() { return m_data.back(); }
    const_reference back() const { return m_data.back(); }

    void remove(size_type i, uint n = 1) { m_data.remove(i, n); }
    void insert(size_type i, value_type const& v) { m_data.insert(i, v); }
    void insert(size_type i, value_type&& v) { m_data.insert(i, std::move(v)); }

    iterator begin() { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.cbegin(); }
    iterator rbegin() noexcept { return m_data.rbegin(); }
    const_iterator rbegin() const noexcept { return m_data.rbegin(); }
    const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    const_iterator crbegin() const noexcept { return m_data.crbegin(); }

    iterator end() { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.cend(); }
    iterator rend() noexcept { return m_data.rend(); }
    const_iterator rend() const noexcept { return m_data.rend(); }
    const_iterator cend() const noexcept { return m_data.cend(); }
    const_iterator crend() const noexcept { return m_data.crbegin(); }

    reference at(size_type idx) {
        assert(idx < size());
        return operator[](idx);
    }

    /** Qt **/
    bool isEmpty() const { return empty(); }
    size_type length() const { return size(); }

    value_type const* constData() const { return data(); }

    reference first() { return front(); }
    const_reference first() const { return front(); }
    reference last() { return back(); }
    const_reference last() const { return back(); }

    template <typename U>
    void append(U&& arg) { push_back(std::forward<U>(arg)); }

    container_type mid(int pos, int len = -1) const
    {
        return m_data.mid(pos, len);
    }

private:

    /// data
    container_type m_data;
};

} // namespace GenH5

#endif
