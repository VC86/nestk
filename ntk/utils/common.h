#ifndef   	NTK_UTILS_COMMON_H_
# define   	NTK_UTILS_COMMON_H_

# include <ntk/core.h>
# include <exception>

#  if defined __GNUC__
#    define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  endif

#define ntk_ptr_typedefs(Class) \
typedef ntk::Ptr<Class> Class##Ptr; \
typedef ntk::Ptr<const Class> Class##ConstPtr;

#define ntk_stringify(Code) #Code

#if (defined WIN32 || defined _WIN32 || defined WINCE)
    #define NTK_EXPORTS __declspec(dllexport)
#else
    #define NTK_EXPORTS
#endif

#if defined _WIN32 && defined _MSC_VER
# define ntk_isfinite(x) _finite(x)
# define ntk_isnan(x) _isnan(x)
# define ntk_isinf(x) (!_finite(x))
#else
# define ntk_isfinite(x) std::isfinite(x)
# define ntk_isnan(x) std::isnan(x)
# define ntk_isinf(x) std::isinf(x)
#endif

namespace cv {
  CV_EXPORTS void* fastMalloc(size_t);
  CV_EXPORTS void fastFree(void* ptr);
}

namespace ntk
{
  template<class T> inline void checked_delete(T * x);

  // Implementation taken from OpenCV
  template<typename _Tp> class CV_EXPORTS Ptr
  {
  public:
    struct ConstCastTag {};
    struct DynamicCastTag {};
    struct ReinterpretCastTag {};
  public:
    Ptr();
    Ptr(_Tp* _obj);
    ~Ptr();
    Ptr(const Ptr& ptr);
    template <class U>  Ptr(const Ptr<U>& ptr);
    template <class U>  Ptr(const Ptr<U>& ptr, ConstCastTag);
    template <class U>  Ptr(const Ptr<U>& ptr, DynamicCastTag);
    template <class U>  Ptr(const Ptr<U>& ptr, ReinterpretCastTag);
    Ptr& operator = (const Ptr& ptr);
    void addref();
    void release();
    void delete_obj();
    bool empty() const;

    _Tp* operator -> () const;
    operator _Tp* () const;

  private:
    template<typename _Up> friend class Ptr;

  protected:
    _Tp* obj;
    int* refcount;
  };

  template<typename _Tp> inline Ptr<_Tp>::Ptr() : obj(0), refcount(0) {}
  template<typename _Tp> inline Ptr<_Tp>::Ptr(_Tp* _obj) : obj(_obj)
  {
      if(obj)
      {
        refcount = (int*)cv::fastMalloc(sizeof(*refcount));
          *refcount = 1;
      }
      else
          refcount = 0;
  }

  template<typename _Tp> inline void Ptr<_Tp>::addref()
  { if( refcount ) CV_XADD(refcount, 1); }

  template<typename _Tp> inline void Ptr<_Tp>::release()
  {
      if( refcount && CV_XADD(refcount, -1) == 1 )
      {
          delete_obj();
          cv::fastFree(refcount);
      }
      refcount = 0;
      obj = 0;
  }

  template<typename _Tp> inline void Ptr<_Tp>::delete_obj()
  {
      if( obj )
          checked_delete<_Tp>(obj);
          // delete obj;
  }

  template<typename _Tp> inline Ptr<_Tp>::~Ptr() { release(); }

  template<typename _Tp> inline Ptr<_Tp>::Ptr(const Ptr<_Tp>& ptr)
  {
      obj = ptr.obj;
      refcount = ptr.refcount;
      addref();
  }

  template<typename _Tp>
  template<typename _Up>
  inline Ptr<_Tp>::Ptr(const Ptr<_Up>& ptr)
  {
    obj = ptr.obj;
    refcount = ptr.refcount;
    addref();
  }

  template<typename _Tp>
  template <class _Up>
  inline Ptr<_Tp>::Ptr(const Ptr<_Up>& ptr, DynamicCastTag)
  {
    obj = dynamic_cast<_Tp*>(ptr.obj);
    if (obj)
    {
      refcount = ptr.refcount;
      addref();
    }
    else
    {
      refcount = 0;
    }
  }

  template<typename _Tp>
  template <class _Up>
  inline Ptr<_Tp>::Ptr(const Ptr<_Up>& ptr, ConstCastTag)
  {
    obj = const_cast<_Tp*>(ptr.obj);
    if (obj)
    {
      refcount = ptr.refcount;
      addref();
    }
    else
    {
      refcount = 0;
    }
  }

  template<typename _Tp>
  template <class _Up>
  inline Ptr<_Tp>::Ptr(const Ptr<_Up>& ptr, ReinterpretCastTag)
  {
    obj = reinterpret_cast<_Tp*>(ptr.obj);
    if (obj)
    {
      refcount = ptr.refcount;
      addref();
    }
    else
    {
      refcount = 0;
    }
  }

  template<typename _Tp> inline Ptr<_Tp>& Ptr<_Tp>::operator = (const Ptr<_Tp>& ptr)
  {
      int* _refcount = ptr.refcount;
      if( _refcount )
          CV_XADD(_refcount, 1);
      release();
      obj = ptr.obj;
      refcount = _refcount;
      return *this;
  }

  template<typename _Tp> inline _Tp* Ptr<_Tp>::operator -> () const { return obj; }

  template<typename _Tp> inline Ptr<_Tp>::operator _Tp* () const { return obj; }

  template<typename _Tp> inline bool Ptr<_Tp>::empty() const { return obj == 0; }

  template <typename _Tp, typename _Up>
  inline Ptr<_Tp> dynamic_Ptr_cast(const Ptr<_Up>& ptr)
  { return Ptr<_Tp>(ptr, typename Ptr<_Tp>::DynamicCastTag()); }

  template <typename _Tp, typename _Up>
  inline Ptr<_Tp> const_Ptr_cast(const Ptr<_Up>& ptr)
  { return Ptr<_Tp>(ptr, typename Ptr<_Tp>::ConstCastTag()); }

  template <typename _Tp, typename _Up>
  inline Ptr<_Tp> reinterpret_Ptr_cast(const Ptr<_Up>& ptr)
  { return Ptr<_Tp>(ptr, typename Ptr<_Tp>::ReinterpretCastTag()); }

}

namespace ntk
{
  template <class T>
  ntk::Ptr<T> toPtr(T* ptr) { return ntk::Ptr<T>(ptr); }

  template <class T>
  inline void delete_and_zero(T*& ptr) { delete ptr; ptr = 0; }

  template<class T> inline void checked_delete(T * x)
  {
      // intentionally complex - simplification causes regressions
      typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
      (void) sizeof(type_must_be_complete);
      delete x;
  }
}

#define stl_bounds(s) s.begin(), s.end()

#define foreach_idx(Idxname, Vector) \
 for (size_t Idxname = 0; Idxname < (size_t)(Vector).size(); ++Idxname)
    
#define foreach_uidx(Idxname, Vector) \
 for (unsigned Idxname = 0; Idxname < (unsigned)(Vector).size(); ++Idxname)
    
#define foreach_it(Itname, Instance, Type) \
 for (Type::iterator Itname = (Instance).begin(); Itname != (Instance).end(); ++Itname)

#define foreach_const_it(Itname, Instance, Type) \
 for (Type::const_iterator Itname = (Instance).begin(); Itname != (Instance).end(); ++Itname)

#define cat2(a, b) a,b
#define cat3(a, b, c) a,b,c
#define cat4(a,b,c,d) a,b,c,d

#define ntk_throw_exception(msg) \
{ \
  std::string s (PRETTY_FUNCTION); s = (s + ": ") + msg; \
  throw ntk::exception(s); \
}

#define ntk_throw_exception_if(cond, msg) \
{ \
  if (cond) \
  { \
    std::string s (PRETTY_FUNCTION); s = (s + ": ") + msg; \
    throw ntk::exception(s); \
  } \
}

namespace ntk
{

  class exception : public std::exception
  {
  public:
    exception(const std::string& what) throw() : m_what(what)
    {}

    virtual ~exception() throw() {}

    virtual const char* what() const throw()
    { return m_what.c_str(); }

  private:
    std::string m_what;
  };

} // end of ntk

#endif 	    /* !NTK_UTILS_COMMON_H_ */
