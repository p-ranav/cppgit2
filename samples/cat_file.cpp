#include <cppgit2/repository.hpp>
#include <cstdio>
#include <iomanip>
#include <iostream>
using namespace cppgit2;

void print_signature(const std::string &header, const signature &sig) {
  char sign;
  auto offset = sig.offset();
  if (offset < 0) {
    sign = '-';
    offset = -offset;
  } else {
    sign = '+';
  }

  auto hours = offset / 60;
  auto minutes = offset % 60;

  std::cout << header << " "
    << sig.name() << " "
    << "<" << sig.email() << "> "
    << sig.time() << " "
    << sign;
  std::cout << std::setfill('0') << std::setw(2) << hours;
  std::cout << std::setfill('0') << std::setw(2) << minutes << std::endl;
}

// Printing out a blob is simple, get the contents and print
void show_blob(const blob &blob) {
  std::fwrite(blob.raw_contents(), blob.raw_size(), 1, stdout);
}

// Show each entry with its type, id and attributes
void show_tree(const tree &tree) {
  size_t count = tree.size();
  for (size_t i = 0; i < tree.size(); ++i) {
    auto entry = tree.lookup_entry_by_index(i);

    std::cout << std::setfill('0') << 
        std::oct << std::setw(6) << static_cast<git_filemode_t>(entry.filemode());
    std::cout << " " << object::object_type_to_string(entry.type())
        << " " << entry.id().to_hex_string() 
        << "\t" << entry.filename() << std::endl;
  }
}

// Commits and tags have a few interesting fields in their header.
void show_commit(const commit &commit) {
  std::cout << "tree " << commit.tree_id().to_hex_string() << std::endl;

  for (size_t i = 0; i < commit.parent_count(); ++i)
    std::cout << "parent " << commit.parent_id(i).to_hex_string() << std::endl;

  print_signature("author", commit.author());
  print_signature("committer", commit.committer());

  auto message = commit.message();
  if (!message.empty())
    std::cout << "\n" << message << std::endl;
}

void show_tag(const tag &tag) {
  std::cout << "object " << tag.id().to_hex_string() << std::endl;
  std::cout << "type " << object::object_type_to_string(tag.target_type()) << std::endl;
  std::cout << "tag " << tag.name() << std::endl;
  print_signature("tagger", tag.tagger());

  auto tag_message = tag.message();
  if (!tag_message.empty())
    std::cout << "\n" << tag_message << std::endl;
}

int main(int argc, char **argv) {
  if (argc == 3) {
    auto repo_path = repository::discover_path(".");
    auto repo = repository::open(repo_path);

    enum class actions { size, type, pretty };
    actions action;

    if (strncmp(argv[1], "-s", 2) == 0) {
      action = actions::size;
    } else if (strncmp(argv[1], "-t", 2) == 0) {
      action = actions::type;
    } else if (strncmp(argv[1], "-p", 2) == 0) {
      action = actions::pretty;
    }

    auto revision_str = argv[2];
    auto object = repo.revparse_to_object(revision_str);

    switch(action) {
    case actions::type:
        std::cout << object::object_type_to_string(object.type()) << std::endl;
        break;
    case actions::size:
        std::cout << repo.odb().read(object.id()).size() << std::endl;
        break;
    case actions::pretty:
        switch(object.type()) {
            case object::object_type::blob:
                show_blob(object.as_blob());
                break;
            case object::object_type::commit:
                show_commit(object.as_commit());
                break;
            case object::object_type::tree:
                show_tree(object.as_tree());
                break;
            case object::object_type::tag:
                show_tag(object.as_tag());
                break;
            default:
                std::cout << "unknown " << revision_str << std::endl;
                break;
        }
        break;
    }

  } else {
    std::cout << "Usage: ./executable (-s | -t | -p) <object>\n";
  }
}