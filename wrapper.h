// #include "utils.h"

#include <type_traits>

namespace wrapper {

//Is Callable Object
template<typename Functor, typename sfinae = void>
struct is_functor : std::false_type {};

template<typename Functor>
struct is_functor<Functor, std::void_t<decltype(&Functor::operator())>> : std::true_type {};

template<typename Functor>
inline constexpr bool is_functor_v = is_functor<Functor>::value;

template<typename Function>
using is_functor_t = typename is_functor<Function>::type;

//Is Function
template<typename Function, typename sfinae = void>
struct is_function : std::false_type {};

template<typename Function>
struct is_function<Function, std::void_t<decltype(std::is_function_v<Function>)>> : std::true_type {};

template<typename Function>
inline constexpr bool is_function_v = is_function<Function>::value;

//Is Method ver1
template<typename T>
struct Trait {};

template<typename T, typename U>
struct Trait<U T::*> {
  using type = U;
};

template<typename ReturnT>
struct MethodClassType {};

template<typename ReturnT, typename classT>
struct MethodClassType<ReturnT classT::*> {
  using type = ReturnT;
};

template<typename DecayedF>
using method_class_type = typename MethodClassType<DecayedF>::type;

//Is Method ver2
template<typename Method, typename sfinae = void>
struct is_method : std::false_type {};

template<typename Method>
struct is_method<Method, std::void_t<decltype(std::is_member_function_pointer_v<Method>)>> : std::true_type {};

template<typename Method>
inline constexpr bool is_method_class_v = is_method<Method>::value;

template<typename Method>
using is_method_class_t = typename is_method<Method>::type;

//Typelist
template <typename T> 
struct TypeList {};

// Use for ExtractArgs and ExtractReturnType
// For EX: with method int (Test::*)(int, int);
// ExtractArgsImpl will return with ReturnType = int and 
// ArgTypes is typelist like Typelist<Test*, int, int>

template<typename Signature>
struct ExtractArgsImpl;

//For
template<typename R, typename... Args>
struct ExtractArgsImpl<R(Args...)> {
  using ReturnType = R;
  using ArgsList = TypeList<Args...>;
};

// template<typename R, typename... Args>
// struct ExtractArgsImpl<R (&)(Args...)> {
//   using ReturnType = R;
//   using ArgsList = TypeList<Args...>;
// };

template<typename Signature>
using ExtractArgs = typename ExtractArgsImpl<Signature>::ArgsList;

template<typename Singature>
using ExtractReturnType = typename ExtractArgsImpl<Singature>::ReturnType;

template<bool B>
using EnableIf = std::enable_if_t<B, bool>;

template <typename F>
const bool& IsMemFunPtr =
    std::is_member_function_pointer<F>::value;

template <typename F>
const bool &IsMemObjPtr = std::is_member_object_pointer<F>::value;

template<typename Signature, typename sfinae = void>
struct FunctorsTrait;

// For functions
template<typename R, typename... Args>
struct FunctorsTrait<R (*)(Args...)> {
  using ReturnType = R;

  template<typename Function, typename... RunArgs>
  static ReturnType wrapper_impl(Function&& function, RunArgs&&... args) {
    std::function<decltype(function(args...)())> func = std::bind(
        std::forward<Function>(function), std::forward<Args>(args)...);
    return func;
  }
};

// For functions
template<typename R, typename... Args>
struct FunctorsTrait<R (&)(Args...)> {
  using ReturnType = R;

  template<typename Function, typename... RunArgs>
  static ReturnType wrapper_impl(Function&& function, RunArgs&&... args) {
    std::function<decltype(function(args...)())> func = std::bind(
        std::forward<Function>(function), std::forward<Args>(args)...);
    return func;
  }
};

//For methods
template<typename R, typename Receiver, typename... Args>
struct FunctorsTrait<R (Receiver::*)(Args...)> {
  using ReturnType = R;

  template<typename Method, typename ReceiverPtr, typename... RunArgs>
  static decltype(auto) wrapper_impl(Method&& method, ReceiverPtr&& receiver_ptr, RunArgs&&... args) {
    std::function<R()> func = std::bind(
        std::forward<Method>(method), 
        std::forward<ReceiverPtr>(receiver_ptr),
        std::forward<RunArgs>(args)...);

    return func;
  }
};

//For const methods
template<typename R, typename Receiver, typename... Args>
struct FunctorsTrait<R (Receiver::*)(Args...) const> {
  using ReturnType = R;

  template<typename Method, typename ReceiverPtr, typename... RunArgs>
  static decltype(auto) wrapper_impl(Method&& method, ReceiverPtr&& receiver_ptr, RunArgs&&... args) {
    std::function<R()> func = std::bind(
        std::forward<Method>(method), 
        std::forward<ReceiverPtr>(receiver_ptr),
        std::forward<RunArgs>(args)...);

    return func;
  }
};

//For excepts methods
template<typename R, typename Receiver, typename... Args>
struct FunctorsTrait<R (Receiver::*)(Args...) noexcept> {
  using ReturnType = R;

  template<typename Method, typename ReceiverPtr, typename... RunArgs>
  static decltype(auto) wrapper_impl(Method&& method, ReceiverPtr&& receiver_ptr, RunArgs&&... args) {
    std::function<R()> func = std::bind(
        std::forward<Method>(method), 
        std::forward<ReceiverPtr>(receiver_ptr),
        std::forward<RunArgs>(args)...);

    return func;
  }
};

//For const excepts methods
template<typename R, typename Receiver, typename... Args>
struct FunctorsTrait<R (Receiver::*)(Args...) const noexcept> {
  using ReturnType = R;

  template<typename Method, typename ReceiverPtr, typename... RunArgs>
  static decltype(auto) wrapper_impl(Method&& method, ReceiverPtr&& receiver_ptr, RunArgs&&... args) {
    std::function<R()> func = std::bind(
        std::forward<Method>(method), 
        std::forward<ReceiverPtr>(receiver_ptr),
        std::forward<RunArgs>(args)...);

    return func;
  }
};

//Functions and Callable Objects helper.
template <typename F, typename... Args,
          EnableIf<is_function_v<F> || is_functor_v<F>> = true>
constexpr decltype(auto) wrapper_impl(F &&f, Args &&...args) {
  std::function<decltype(f(args...))()> func = 
    std::bind(std::forward<F>(f), std::forward<Args>(args)...);

  //Why Error???
  // auto func = FunctorsTrait<F>::wrapper_impl(
      // std::forward<F>(f), std::forward<Args>(args)...);
  return func;
}

//Methods helper.
template <typename F, typename T, typename... Args,
          EnableIf<IsMemFunPtr<F> || IsMemObjPtr<F>> = true>
constexpr decltype(auto) wrapper_impl(F &&f, T &&t, Args &&...args) {
  auto func = FunctorsTrait<F>::wrapper_impl(
      std::forward<F>(f), std::forward<T>(t), std::forward<Args>(args)...);

  return func;
}

//Get return type of function, callable objects, methods.
template<typename F>
using ReturnType = typename FunctorsTrait<F>::ReturnType;

//Wrapper for callable object, functions, methods.
template<typename F, typename... Args>
constexpr decltype(auto) wrapper(F&& f, Args&&... args) {
  return wrapper_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace