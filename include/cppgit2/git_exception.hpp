#pragma once
#include <exception>
#include <git2.h>
#include <string>

namespace cppgit2 {

class git_exception : public std::exception {
public:
  git_exception() {
    auto error = git_error_last();
    message_ = error ? error->message : "unknown error";
  }
  git_exception(const char *message) : message_(message) {}
  git_exception(const std::string &message) : message_(message) {}
  virtual ~git_exception() throw() {}
  virtual const char *what() const throw() { return message_.c_str(); }

  static void clear() { git_error_clear(); }

protected:
  std::string message_;
};

} // namespace cppgit2
