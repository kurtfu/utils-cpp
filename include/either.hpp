#ifndef EITHER_HPP
#define EITHER_HPP
// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

/*****************************************************************************/
/*** HEADER INCLUDES *********************************************************/

#if !defined(__cpp_exceptions)
    #include <iostream>
#endif  // __cpp_exceptions

#include <memory>
#include <stdexcept>

/*****************************************************************************/
/*** MACRO DEFINITIONS *******************************************************/

#if __cplusplus < 201703L
    #define SUCCESS_NODISCARD
    #define FAIL_NODISCARD
    #define EITHER_NODISCARD
#else
    #define SUCCESS_NODISCARD [[nodiscard]]
    #define FAIL_NODISCARD    [[nodiscard]]
    #define EITHER_NODISCARD  [[nodiscard]]
#endif  // __cplusplus

#if __cplusplus > 201703L
    #define EITHER_CONSTEXPR_DESTRUCTOR constexpr
#else
    #define EITHER_CONSTEXPR_DESTRUCTOR
#endif  // __cplusplus

/*****************************************************************************/
/*** DATA TYPES **************************************************************/

namespace utils
{
    namespace detail
    {
#if __cplusplus < 201703L
        template <typename Fn, typename... Args>
        using result_of = std::result_of_t<Fn(Args...)>;
#else
        template <typename Fn, typename... Args>
        using result_of = std::invoke_result_t<Fn, Args...>;
#endif  // __cplusplus

        inline void throw_or_mimic(const char* text)
        {
#if defined(__cpp_exceptions)
            throw std::logic_error(text);
#else
            std::cerr << text << '\n';
            std::terminate();
#endif  // __cpp_exceptions
        }
    }  // namespace detail

    template <typename T>
    class success
    {
        template <typename Value, typename Error, typename>
        friend class either;

    public:
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;

        template <typename... Args>
        explicit success(Args... args)
            : m_storage(std::forward<Args>(args)...)
        {}

        reference value()
        {
            return m_storage;
        }

        SUCCESS_NODISCARD
        const_reference value() const
        {
            return m_storage;
        }

        constexpr reference operator*()
        {
            return m_storage;
        }

        SUCCESS_NODISCARD
        constexpr const_reference operator*() const
        {
            return m_storage;
        }

    private:
        value_type m_storage;
    };

    template <typename T>
    class fail
    {
        template <typename Value, typename Error, typename>
        friend class either;

    public:
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;

        template <typename... Args>
        explicit fail(Args... args)
            : m_storage(std::forward<Args>(args)...)
        {}

        reference value()
        {
            return m_storage;
        }

        FAIL_NODISCARD
        const_reference value() const
        {
            return m_storage;
        }

        constexpr reference operator*()
        {
            return m_storage;
        }

        FAIL_NODISCARD
        constexpr const_reference operator*() const
        {
            return m_storage;
        }

    private:
        value_type m_storage;
    };

    template <typename Value, typename Error, typename = void>
    class either;

    template <typename Value, typename Error>
    class either<Value, Error, typename std::enable_if<!std::is_void<Value>::value>::type>
    {
    public:
        using value_type = Value;
        using error_type = Error;

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr either(success<value_type> item)
            : m_has_value(true)
        {
            ::new (std::addressof(m_value)) value_type(std::move(item.m_storage));
        }

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr either(fail<error_type> item)
            : m_has_value(false)
        {
            ::new (std::addressof(m_error)) error_type(std::move(item.m_storage));
        }

        constexpr either(const either& that)
            : m_has_value(that.m_has_value)
        {
            if (m_has_value)
            {
                ::new (std::addressof(m_value)) value_type(that.m_value);
            }
            else
            {
                ::new (std::addressof(m_error)) error_type(that.m_error);
            }
        }

        constexpr either(either&& that) noexcept
            : m_has_value(that.m_has_value)
        {
            if (m_has_value)
            {
                ::new (std::addressof(m_value)) value_type(std::move(that.m_value));
            }
            else
            {
                ::new (std::addressof(m_error)) error_type(std::move(that.m_error));
            }
        }

        EITHER_CONSTEXPR_DESTRUCTOR
        ~either()
        {
            if (m_has_value)
            {
                m_value.~value_type();
            }
            else
            {
                m_error.~error_type();
            }
        }

        either& operator=(const either& that)
        {
            if (this != &that)
            {
                m_has_value = that.m_has_value;

                if (m_has_value)
                {
                    ::new (std::addressof(m_value)) value_type(that.m_value);
                }
                else
                {
                    ::new (std::addressof(m_error)) error_type(that.m_error);
                }
            }

            return *this;
        }

        either& operator=(either&& that) noexcept
        {
            if (this != &that)
            {
                m_has_value = that.m_has_value;

                if (m_has_value)
                {
                    ::new (std::addressof(m_value)) value_type(std::move(that.m_value));
                }
                else
                {
                    ::new (std::addressof(m_error)) error_type(std::move(that.m_error));
                }
            }

            return *this;
        }

        value_type& value()
        {
            if (!m_has_value)
            {
                detail::throw_or_mimic("either has no value type!");
            }

            return m_value;
        }

        EITHER_NODISCARD
        const value_type& value() const
        {
            if (!m_has_value)
            {
                detail::throw_or_mimic("either has no value type!");
            }

            return m_value;
        }

        error_type& error()
        {
            if (m_has_value)
            {
                detail::throw_or_mimic("either has no error type!");
            }

            return m_error;
        }

        EITHER_NODISCARD
        const error_type& error() const
        {
            if (m_has_value)
            {
                detail::throw_or_mimic("either has no error type!");
            }

            return m_error;
        }

        EITHER_NODISCARD
        bool has_value() const
        {
            return m_has_value;
        }

        template <typename F>
        constexpr auto and_then(F&& func)
        {
            using U = detail::result_of<F, value_type>;

            if (!m_has_value)
            {
                return U(utils::fail<error_type>(m_error));
            }

            return std::forward<F>(func)(m_value);
        }

        template <typename F>
        constexpr auto or_else(F&& func)
        {
            using U = detail::result_of<F, error_type>;

            if (m_has_value)
            {
                return U(utils::fail<error_type>(m_error));
            }

            return std::forward<F>(func)(m_error);
        }

        explicit operator bool() const noexcept
        {
            return m_has_value;
        }

        constexpr value_type& operator*()
        {
            return m_value;
        }

        EITHER_NODISCARD
        constexpr const value_type& operator*() const
        {
            return m_value;
        }

    private:
        union
        {
            value_type m_value;
            error_type m_error;
        };

        bool m_has_value;
    };

    template <typename Value, typename Error>
    class either<Value, Error, typename std::enable_if<std::is_void<Value>::value>::type>
    {
    public:
        using value_type = Value;
        using error_type = Error;

        constexpr either()
            : m_has_value(true)
        {}

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr either(fail<error_type> item)
            : m_has_value(false)
        {
            ::new (std::addressof(m_error)) error_type(std::move(item.m_storage));
        }

        constexpr either(const either& that)
            : m_has_value(that.m_has_value)
        {
            if (!m_has_value)
            {
                ::new (std::addressof(m_error)) error_type(that.m_error);
            }
        }

        constexpr either(either&& that) noexcept
            : m_has_value(that.m_has_value)
        {
            if (!m_has_value)
            {
                ::new (std::addressof(m_error)) error_type(std::move(that.m_error));
            }
        }

        EITHER_CONSTEXPR_DESTRUCTOR
        ~either()
        {
            if (!m_has_value)
            {
                m_error.~error_type();
            }
        }

        either& operator=(const either& that)
        {
            if (this != &that)
            {
                m_has_value = that.m_has_value;

                if (!m_has_value)
                {
                    ::new (std::addressof(m_error)) error_type(that.m_error);
                }
            }

            return *this;
        }

        either& operator=(either&& that) noexcept
        {
            if (this != &that)
            {
                m_has_value = that.m_has_value;

                if (!m_has_value)
                {
                    ::new (std::addressof(m_error)) error_type(std::move(that.m_error));
                }
            }

            return *this;
        }

        constexpr void value() const
        {
            if (!m_has_value)
            {
                detail::throw_or_mimic("either has no value type!");
            }
        }

        constexpr error_type& error()
        {
            if (m_has_value)
            {
                detail::throw_or_mimic("either has no error type!");
            }

            return m_error;
        }

        EITHER_NODISCARD
        constexpr const error_type& error() const
        {
            if (m_has_value)
            {
                detail::throw_or_mimic("either has no error type!");
            }

            return m_error;
        }

        EITHER_NODISCARD
        constexpr bool has_value() const
        {
            return m_has_value;
        }

        template <typename F>
        constexpr auto and_then(F&& func)
        {
            using U = detail::result_of<F>;

            if (!m_has_value)
            {
                return U(utils::fail<error_type>(m_error));
            }

            return std::forward<F>(func)();
        }

        template <typename F>
        constexpr auto or_else(F&& func)
        {
            using U = detail::result_of<F, error_type>;

            if (m_has_value)
            {
                return U(utils::fail<error_type>(m_error));
            }

            return std::forward<F>(func)(m_error);
        }

        explicit operator bool() const noexcept
        {
            return m_has_value;
        }

        constexpr void operator*() const
        {}

    private:
        error_type m_error;

        bool m_has_value;
    };

#if __cplusplus >= 201703L
    template <typename T>
    success(T) -> success<T>;

    template <typename T>
    fail(T) -> fail<T>;
#endif  // __cplusplus

}  // namespace utils

#undef SUCCESS_NODISCARD
#undef FAIL_NODISCARD
#undef EITHER_NODISCARD

#undef EITHER_CONSTEXPR_DESTRUCTOR

// NOLINTEND(cppcoreguidelines-pro-type-union-access)
#endif  // EITHER_HPP
