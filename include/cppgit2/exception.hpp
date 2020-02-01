#pragma once
#include <exception>
#include <git2.h>
#include <string>

namespace cppgit2 {

class exception : public std::exception {
public:
  exception() {
    auto error = git_error_last();
    message_ = error ? error->message : "unknown error";
  }
  exception(const char *message) : message_(message) {}
  exception(const std::string &message) : message_(message) {}
  virtual ~exception() throw() {}
  virtual const char *what() const throw() { return message_.c_str(); }

protected:
  std::string message_;
};

} // namespace cppgit2
