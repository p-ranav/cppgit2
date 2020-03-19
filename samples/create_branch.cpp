#include <cppgit2/repository.hpp>
#include <fstream>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    // Create new repo
    auto repo = repository::init(argv[1], false);

    std::cout << "Initialized repository " << argv[1] << std::endl;

    // Write README file
    std::ofstream readme;
    readme.open(std::string{argv[1]} + "/README.md");
    readme << "Hello, World!\n";
    readme.close();

    std::cout << "Created README file" << std::endl;

    // Get repo index and write as tree
    auto index = repo.index();
    index.add_entry_by_path("README.md");
    index.write();
    auto tree_oid = index.write_tree();

    std::cout << "Staged README.md" << std::endl;

    // Prepare signatures
    auto author = signature("foobar", "foo.bar@baz.com");
    auto committer = signature("foobar", "foo.bar@baz.com");

    // Create commit
    auto commit_oid =
        repo.create_commit("HEAD", author, committer, "utf-8", "Initial README",
                           repo.lookup_tree(tree_oid), {});
    auto commit = repo.lookup_commit(commit_oid);

    std::cout << "Created commit " << commit_oid.to_hex_string(8)
              << " in the master branch" << std::endl;

    // Create branch - update_readme
    auto branch_name = "update_readme";
    auto branch_ref = repo.create_branch(branch_name, commit, false);

    std::cout << "Created branch " << branch_name << std::endl;

    // Checkout branch - update_readme
    auto treeish = repo.revparse_to_object(branch_name);
    repo.checkout_tree(treeish);
    repo.set_head(branch_ref.name());

    std::cout << "Checked out " << branch_name << std::endl;

    // Update the README file
    readme.open(std::string{argv[1]} + "/README.md");
    readme << "Hello, World!\nUpdated README";
    readme.close();

    std::cout << "Updated README.md" << std::endl;

    // Get repo index and write as tree
    index = repo.index();
    index.add_entry_by_path("README.md");
    index.write();
    tree_oid = index.write_tree();

    std::cout << "Staged README.md\n";

    // Create commit in update_readme branch
    auto commit_oid_2 = repo.create_commit(
        branch_ref.name(), author, committer, "utf-8", "Update README",
        repo.lookup_tree(tree_oid), {commit});
    auto commit_2 = repo.lookup_commit(commit_oid_2);

    std::cout << "Created commit " << commit_oid_2.to_hex_string(8) << "in "
              << branch_name << std::endl;

  } else {
    std::cout << "Usage: ./executable <new_repo_path>\n";
  }
}