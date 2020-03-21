#include <cppgit2/repository.hpp>
using namespace cppgit2;
#include <functional>

revwalk::revwalk()
    : done_(false), c_ptr_(nullptr), owner_(ownership::libgit2) {}

// Construct from libgit2 C ptr
revwalk::revwalk(git_revwalk *c_ptr, ownership owner)
    : done_(false), c_ptr_(c_ptr), owner_(owner) {}

// Cleanup revwalk
revwalk::~revwalk() {
  if (c_ptr_ && owner_ == ownership::user)
    git_revwalk_free(c_ptr_);
}

void revwalk::add_hide_callback(std::function<int(const oid &)> callback) {
  struct visitor_wrapper {
    std::function<int(const oid &)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = callback;

  auto callback_c = [](const git_oid *commit_id, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    return wrapper->fn(oid(commit_id));
  };

  if (git_revwalk_add_hide_cb(c_ptr_, callback_c, (void *)(&wrapper)))
    throw git_exception();
}

bool revwalk::done() const { return done_; }

void revwalk::hide(const oid &commit_id) {
  if (git_revwalk_hide(c_ptr_, commit_id.c_ptr()))
    throw git_exception();
}

void revwalk::hide_glob(const std::string &glob) {
  if (git_revwalk_hide_glob(c_ptr_, glob.c_str()))
    throw git_exception();
}

void revwalk::hide_head() {
  if (git_revwalk_hide_head(c_ptr_))
    throw git_exception();
}

void revwalk::hide_reference(const std::string &ref) {
  if (git_revwalk_hide_ref(c_ptr_, ref.c_str()))
    throw git_exception();
}

oid revwalk::next() {
  oid result;
  if (git_revwalk_next(result.c_ptr(), c_ptr_) == GIT_ITEROVER)
    done_ = true;
  return result;
}

void revwalk::push(const oid &id) {
  if (git_revwalk_push(c_ptr_, id.c_ptr()))
    throw git_exception();
}

void revwalk::push_glob(const std::string &glob) {
  if (git_revwalk_push_glob(c_ptr_, glob.c_str()))
    throw git_exception();
}

void revwalk::push_head() {
  if (git_revwalk_push_head(c_ptr_))
    throw git_exception();
}

void revwalk::push_range(const std::string &range) {
  if (git_revwalk_push_range(c_ptr_, range.c_str()))
    throw git_exception();
}

void revwalk::push_reference(const std::string &ref) {
  if (git_revwalk_push_ref(c_ptr_, ref.c_str()))
    throw git_exception();
}

cppgit2::repository revwalk::repository() {
  return cppgit2::repository(git_revwalk_repository(c_ptr_));
}

void revwalk::reset() {
  done_ = false;
  if (git_revwalk_reset(c_ptr_))
    throw git_exception();
}

void revwalk::set_sorting_mode(revwalk::sort sorting_mode) {
  if (git_revwalk_sorting(c_ptr_, static_cast<unsigned int>(sorting_mode)))
    throw git_exception();
}

void revwalk::simplify_first_parent() {
  if (git_revwalk_simplify_first_parent(c_ptr_))
    throw git_exception();
}

const git_revwalk *revwalk::c_ptr() const { return c_ptr_; }