#include <cppgit2/repository.hpp>
#include <fstream>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {

    // Create new repo
    repository repo(argv[1], /* bare = */false);

    // Write README file
    std::ofstream readme;
    readme.open(std::string{argv[1]} + "/README.md");
    readme << "Hello, World!\n";
    readme.close();

    // Get repo index and write as tree
    auto index = repo.index();
    index.add_entry_by_path("README.md");
    index.write();
    auto tree_oid = index.write_tree();

    // Prepare signatures
    auto author = signature("foobar", "foo.bar@baz.com");
    auto committer = signature("foobar", "foo.bar@baz.com");

    // Create commit
    auto commit_oid = repo.create_commit("HEAD", author, committer, "utf-8", 
          "Update README", repo.lookup_tree(tree_oid), {});

    std::cout << commit_oid.to_hex_string() << std::endl;

  } else {
    std::cout << "Usage: ./executable <new_repo_path>\n";
  }
}



