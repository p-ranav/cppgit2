#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    auto index = repo.index();
    auto tree_oid = index.write_tree();
    auto tree = repo.lookup_tree(tree_oid);

    tree.walk(tree::traversal_mode::preorder,
              [](const std::string &root, const tree::entry &entry) {
                auto type = entry.type();
                std::string type_string{""};
                switch (type) {
                case object::object_type::blob:
                  type_string = " - blob";
                  break;
                case object::object_type::tree:
                  type_string = "tree";
                  break;
                case object::object_type::commit:
                  type_string = " - commit";
                  break;
                default:
                  type_string = "other";
                  break;
                }
                std::cout << type_string << " [" << entry.id().to_hex_string(8)
                          << "] " << entry.filename() << std::endl;
              });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}