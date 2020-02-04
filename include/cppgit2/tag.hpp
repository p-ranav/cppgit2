#pragma once
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/signature.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

  class tag {
  public:
    tag() : c_ptr_(nullptr), owner_(ownership::libgit2) {
      git_libgit2_init();
    }
    
    tag(git_tag * c_ptr, ownership owner = ownership::libgit2) :
      c_ptr_(c_ptr), owner_(owner) {
      git_libgit2_init();
    }
    
    ~tag() {
      if (c_ptr_ && owner_ == ownership::user)
	git_tag_free(c_ptr_);
      git_libgit2_shutdown();
    }

    // Create an in-memory copy of a tag
    tag copy() const {
      tag result;
      if (git_tag_dup(&result.c_ptr_, c_ptr_))
	throw exception();
      return result;
    }

    // Id of the tag
    oid id() const { return oid(git_tag_id(c_ptr_)); }

    std::string message() const {
      auto ret = git_tag_message(c_ptr_);
      if (ret)
	return std::string(ret);
      else return "";
    }

    std::string name() const {
      auto ret = git_tag_name(c_ptr_);
      if (ret)
	return std::string(ret);
      else
	return "";      
    }

    object peel() const {
      object result;
      if (git_tag_peel(result.c_ptr_ptr(), c_ptr_))
	throw exception();
      return result;
    }

    signature tagger() const {
      return signature(git_tag_tagger(c_ptr_));
    }

    object target() const {
      object result;
      if (git_tag_targert(result.c_ptr_ptr(), c_ptr_))
	throw exception();
      return result;
    }

    object::type target_type() const {
      return static_cast<object::type>(git_tag_target_type(c_ptr_));
    }

    git_tag * c_ptr() { return c_ptr_; }

    const git_tag * c_ptr() const { return c_ptr_; }

  private:
    git_tag * c_ptr_;
    ownership owner_;
  };
  
}
