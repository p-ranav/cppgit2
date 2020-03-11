#include <cppgit2/diff.hpp>
#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {

    // Create new repo
    repository repo(argv[1], false);

    // Write repo index as tree
    auto index = repo.index();
    auto tree_oid = index.write_tree();

    // Prepare signatures
    auto author = signature("foobar", "foo.bar@baz.com");
    auto committer = signature("foobar", "foo.bar@baz.com");

    // Create commit
    auto commit_oid = repo.create_commit("HEAD", author, committer,
      "utf-8", "Initial commit", repo.lookup_tree(tree_oid), {});

    std::cout << "Created commit with ID: " << commit_oid.to_hex_string() << std::endl;
    
  } else {
    std::cout << "Usage: ./executable <new_repo_path>\n";
  }
}