#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

/*****************************************************************************/
/*  HEADER INCLUDES                                                          */
/*****************************************************************************/

#include <array>    // IWYU pragma: export
#include <cstddef>  // IWYU pragma: export

/*****************************************************************************/
/*  MACRO DEFINITIONS                                                        */
/*****************************************************************************/

#if __cplusplus < 201703L
    #define CIRCULAR_BUFFER_NODISCARD
#else
    #define CIRCULAR_BUFFER_NODISCARD [[nodiscard]]
#endif  // __cplusplus

#if __cplusplus > 201703L
    #define CIRCULAR_BUFFER_CONSTEXPR_DESTRUCTOR constexpr
#else
    #define CIRCULAR_BUFFER_CONSTEXPR_DESTRUCTOR
#endif  // __cplusplus

/*****************************************************************************/
/*  DATA TYPES                                                               */
/*****************************************************************************/

namespace utils
{
    template <typename T, std::size_t N>
    class circular_buffer
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using pointer = value_type*;
        using const_pointer = value_type const*;

        using reference = value_type&;
        using const_reference = value_type const&;

        using iterator = typename std::array<T, N>::iterator;
        using const_iterator = typename std::array<T, N>::const_iterator;

        circular_buffer() = default;

        void push(value_type const& item)
        {
            push_impl<value_type const&>(item);
        }

        void push(value_type&& item)
        {
            push_impl<value_type&&>(std::move(item));
        }

        void pop()
        {
            if (m_size != 0)
            {
                --m_size;
                advance(m_read);
            }
        }

        CIRCULAR_BUFFER_NODISCARD
        reference front()
        {
            return *m_read;
        }

        CIRCULAR_BUFFER_NODISCARD
        const_reference front() const
        {
            return *m_read;
        }

        CIRCULAR_BUFFER_NODISCARD
        bool empty() const
        {
            return m_size == 0;
        }

        CIRCULAR_BUFFER_NODISCARD
        std::size_t size() const
        {
            return m_size;
        }

    private:
        template <typename U = T>
        void push_impl(U&& item)
        {
            replace(m_write, std::forward<U>(item));
            advance(m_write);

            if (m_size == N)
            {
                m_read = m_write;
            }
            else
            {
                ++m_size;
            }
        }

        void replace(iterator iter, value_type const& item)
        {
            *iter = item;
        }

        void replace(iterator iter, value_type&& item)
        {
            *iter = std::move(item);
        }

        void advance(iterator& iter)
        {
            std::advance(iter, 1);

            if (iter == m_buffer.end())
            {
                iter = m_buffer.begin();
            }
        }

        std::array<T, N> m_buffer{};

        iterator m_read = m_buffer.begin();
        iterator m_write = m_buffer.begin();

        std::size_t m_size{};
    };
}  // namespace utils

#undef CIRCULAR_BUFFER_NODISCARD
#undef CIRCULAR_BUFFER_CONSTEXPR_DESTRUCTOR

#endif  // CIRCULAR_BUFFER_HPP
