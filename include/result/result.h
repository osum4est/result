#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "google-explicit-constructor"

#ifndef RESULT_RESULT_H
#define RESULT_RESULT_H

#include <backward.hpp>
#include <string>
#include <utility>

namespace result {

namespace _ {

template <typename T>
struct result_ok {
    T value;
    // TODO: Make private and add a friend declaration
};

template <typename E>
struct result_err {
    E error;
    backward::StackTrace stack_trace;
};

template <typename E>
struct error_wrapper {
    E error;
    backward::StackTrace stack_trace;
    error_wrapper(result_err<E>&& err) : error(std::move(err.error)), stack_trace(std::move(err.stack_trace)) { }
};

template <typename T, typename E>
class result_base {
  protected:
    bool _ok;
    union {
        T _value;
        error_wrapper<E> _error;
    };

    inline void ensure_ok() const {
        if (_ok) return;
        std::stringstream ss;
        ss << "Unhandled error result: " << _error.error << std::endl;

        backward::Printer printer;
        printer.object = true;
        printer.color_mode = backward::ColorMode::automatic;
        printer.address = true;
        printer.print(_error.stack_trace, ss);

        throw std::runtime_error(ss.str());
    }

  public:
    inline explicit result_base(T&& value) : _ok(true), _value(std::move(value)) { }
    inline explicit result_base(error_wrapper<E>&& error) : _ok(false), _error(std::move(error)) { }
    result_base(const result_base&) = delete;
    inline ~result_base() {
        if (_ok) _value.~T();
        else _error.~error_wrapper<E>();
    }

    [[nodiscard]] inline bool is_ok() const { return _ok; }
    [[nodiscard]] inline bool is_err() const { return !_ok; }
    [[nodiscard]] inline E err() {
        if (!_ok) return _error.error;
        throw std::runtime_error("Cannot get error from ok result.");
    }
    [[nodiscard]] inline result_err<E> forward() {
        if (!_ok) return result_err<E> {_error.error, _error.stack_trace};
        throw std::runtime_error("Cannot get error from ok result.");
    }
};

template <typename T, typename E>
class val : public result_base<T, E> {
    typedef result_base<T, E> base;

  public:
    val(result_ok<T>&& result) : base(std::move(result.value)) { }
    val(result_ok<T&>&& result) : base(std::move(result.value)) { }
    val(result_err<E>&& result) : base(error_wrapper {std::forward<result_err<E>>(result)}) { }

    [[nodiscard]] inline T& get() {
        base::ensure_ok();
        return base::_value;
    }
};

template <typename T, typename E>
class ref : public result_base<T*, E> {
    typedef result_base<T*, E> base;

  public:
    ref(result_ok<T*>&& result) : base(result.value) { }
    ref(result_ok<T&>&& result) : base(&result.value) { }
    ref(result_err<E>&& result) : base(error_wrapper {std::forward<result_err<E>>(result)}) { }

    [[nodiscard]] inline T& get() {
        base::ensure_ok();
        return *base::_value;
    }
};

template <typename T, typename E>
class ref<const T, E> : public result_base<const T*, E> {
    typedef result_base<const T*, E> base;

  public:
    ref(result_ok<T*>&& result) : base(result.value) { }
    ref(result_ok<T&>&& result) : base(&result.value) { }
    ref(result_ok<const T&>&& result) : base(&result.value) { }
    ref(result_err<E>&& result) : base(error_wrapper {std::forward<result_err<E>>(result)}) { }

    [[nodiscard]] inline const T& get() {
        base::ensure_ok();
        return *base::_value;
    }
};

template <typename T, typename E>
class ptr : public result_base<std::unique_ptr<T>, E> {
    typedef result_base<std::unique_ptr<T>, E> base;
    bool _got_value = false;

  public:
    ptr(result_ok<T*>&& result) : base(std::unique_ptr<T>(result.value)) { }
    template <std::derived_from<T> D>
    ptr(result_ok<D*>&& result) : base(std::unique_ptr<D>(result.value)) { }
    ptr(result_err<E>&& result) : base(error_wrapper {std::forward<result_err<E>>(result)}) { }

    [[nodiscard]] inline std::unique_ptr<T> get() {
        if (_got_value) throw std::runtime_error("Cannot get unique_ptr from result twice.");
        base::ensure_ok();
        _got_value = true;
        return std::move(base::_value);
    }
};

} // namespace _

template <typename T>
inline _::result_ok<T> ok(T&& value) {
    return _::result_ok<T> {std::forward<T>(value)};
}

inline _::result_err<const std::string> err(const std::string&& error) {
    auto stack = backward::StackTrace();
    stack.load_here(32);
    stack.skip_n_firsts(3);

    return _::result_err<const std::string> {std::forward<const std::string>(error), stack};
}

template <typename T, typename E = const std::string>
using val = _::val<T, E>;

template <typename T, typename E = const std::string>
using ref = _::ref<T, E>;

template <typename T, typename E = const std::string>
using ptr = _::ptr<T, E>;

} // namespace result

#define GET_OR_FORWARD(result)                                                                                         \
    ({                                                                                                                 \
        auto&& _result = (result);                                                                                     \
        if (_result.is_err()) return _result.forward();                                                                \
        _result.get();                                                                                                 \
    })

#endif

#pragma clang diagnostic pop