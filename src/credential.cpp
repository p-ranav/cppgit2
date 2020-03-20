#include <cppgit2/credential.hpp>
using namespace cppgit2;

credential::credential() {
  if (git_credential_default_new(&c_ptr_))
    throw git_exception();
}

credential::credential(const std::string &username,
                       const std::string &password) {
  if (git_credential_userpass_plaintext_new(&c_ptr_, username.c_str(),
                                            password.c_str()))
    throw git_exception();
}

credential::credential(const std::string &username,
                       const std::string &public_key,
                       const std::string &private_key,
                       const std::string &passphrase) {
  if (git_credential_ssh_key_new(&c_ptr_, username.c_str(), public_key.c_str(),
                                 private_key.c_str(), passphrase.c_str()))
    throw git_exception();
}

credential::credential(const std::string &username) {
  if (git_credential_ssh_key_from_agent(&c_ptr_, username.c_str()))
    throw git_exception();
}

credential::credential(const std::string &username,
                       git_credential_ssh_interactive_cb prompt_callback,
                       void *payload) {
  if (git_credential_ssh_interactive_new(&c_ptr_, username.c_str(),
                                         prompt_callback, payload))
    throw git_exception();
}

credential::credential(const std::string &username,
                       const std::string &public_key,
                       git_credential_sign_cb sign_callback, void *payload) {
  if (git_credential_ssh_custom_new(&c_ptr_, username.c_str(),
                                    public_key.c_str(), public_key.size(),
                                    sign_callback, payload))
    throw git_exception();
}

credential::~credential() {
  if (c_ptr_)
    git_credential_free(c_ptr_);
}

std::string credential::username() const {
  auto ret = git_credential_get_username(c_ptr_);
  return ret ? std::string(ret) : "";
}

bool credential::has_username() const {
  return git_credential_has_username(c_ptr_);
}

const git_credential *credential::c_ptr() const { return c_ptr_; }