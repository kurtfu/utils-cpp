#ifndef MAYBE_HPP
#define MAYBE_HPP

/*****************************************************************************/
/*** HEADER INCLUDES *********************************************************/

/// \cond
#include <utility>

/// \endcond

/*****************************************************************************/
/*** MACRO DEFINITIONS *******************************************************/

#if __cplusplus > 201703L
    #define MAYBE_CONSTEXPR_DESTRUCTOR constexpr
#else
    #define MAYBE_CONSTEXPR_DESTRUCTOR
#endif  // __cplusplus

/*****************************************************************************/
/*** DATA TYPES **************************************************************/

namespace utils
{
    struct nothing_t
    {};

    struct something_t
    {};

    inline constexpr nothing_t nothing{};
    inline constexpr something_t something{};

    template <typename T>
    class maybe
    {
        static_assert(!std::is_same_v<std::remove_cv_t<T>, nothing_t>);
        static_assert(!std::is_same_v<std::remove_cv_t<T>, something_t>);

    public:
        using value_type = T;

        template <typename... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, bool> = true>
        constexpr maybe(Args&&... args)  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
            : m_storage(something, std::forward<Args>(args)...)
            , m_has_value(true)
        {}

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr maybe(utils::nothing_t /* unused */)
            : m_storage()
        {}

        constexpr maybe(const maybe& that)
        {
            if (that.m_has_value)
            {
                this->construct(that.get());
            }
        }

        constexpr maybe(maybe&& that) noexcept
        {
            if (that.m_has_value)
            {
                this->construct(std::move(that.get()));
                that.m_has_value = false;
            }
        }

        MAYBE_CONSTEXPR_DESTRUCTOR
        ~maybe()
        {
            reset();
        }

        constexpr maybe& operator=(const maybe& that)
        {
            if (this == std::addressof(that))
            {
                return *this;
            }

            if (this->m_has_value && that.m_has_value)
            {
                this->get() = that.get();
            }
            else
            {
                if (that.m_has_value)
                {
                    this->construct(that.get());
                }
                else
                {
                    this->reset();
                }
            }

            return *this;
        }

        constexpr maybe& operator=(maybe&& that) noexcept
        {
            if (this->m_has_value && that.m_has_value)
            {
                this->get() = std::move(that.get());
            }
            else
            {
                if (that.m_has_value)
                {
                    this->construct(std::move(that.get()));
                    that.m_has_value = false;
                }
                else
                {
                    this->reset();
                }
            }

            return *this;
        }

        constexpr maybe& operator=(nothing_t /* unused */) noexcept
        {
            this->reset();
            return *this;
        }

        [[nodiscard]] constexpr bool has_value() const
        {
            return m_has_value;
        }

        explicit constexpr operator bool() const noexcept
        {
            return m_has_value;
        }

        constexpr value_type& operator*() & noexcept
        {
            return this->get();
        }

        constexpr value_type&& operator*() && noexcept
        {
            m_has_value = true;
            return std::move(this->get());
        }

        constexpr value_type* operator->() noexcept
        {
            return std::addressof(this->get());
        }

        constexpr void reset()
        {
            if (m_has_value)
            {
                destroy();
            }
        }

        constexpr void swap(maybe<value_type>& that) noexcept
        {
            using std::swap;

            if (this->m_has_value && that.m_has_value)
            {
                swap(this->get(), that.get());
            }
            else if (this->m_has_value)
            {
                that.construct(std::move(this->get()));
                this->m_has_value = false;
            }
            else if (that.m_has_value)
            {
                this->construct(std::move(that.get()));
                that.m_has_value = false;
            }
        }

        constexpr friend void swap(maybe<value_type>& lhs, maybe<value_type>& rhs) noexcept
        {
            lhs.swap(rhs);
        }

    private:
        template <typename... Args>
        constexpr void construct(Args&&... args)
        {
            ::new (std::addressof(this->get())) value_type(std::forward<Args>(args)...);
            m_has_value = true;
        }

        constexpr void destroy()
        {
            m_has_value = false;
            this->get().~value_type();
        }

        constexpr value_type& get() noexcept
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
            return m_storage.value;
        }

        [[nodiscard]] constexpr const value_type& get() const noexcept
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
            return m_storage.value;
        }

        union storage
        {
            struct empty_t
            {};

            constexpr storage()
                : nothing()
            {}

            template <typename... Args>
            explicit constexpr storage(something_t /* unused */, Args&&... args)
                : value(std::forward<Args>(args)...)
            {}

            constexpr storage(const storage&) = delete;
            constexpr storage(storage&&) noexcept = delete;

            ~storage() {};

            constexpr storage& operator=(const storage&) = delete;
            constexpr storage& operator=(storage&&) noexcept = delete;

            value_type value;
            empty_t nothing;
        };

        storage m_storage;
        bool m_has_value = false;
    };

    template <typename T>
    maybe(T) -> maybe<T>;
}  // namespace utils

#undef MAYBE_CONSTEXPR_DESTRUCTOR

#endif  // MAYBE_HPP
