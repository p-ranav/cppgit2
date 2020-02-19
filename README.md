<p align="center">
  <img height="100" src="img/logo.png"/>  
</p>

`cppgit2` is a `libgit2` wrapper library for use in modern C++ `( >= C++11)`. See the [Build and Integration](#build-and-integration) section of details on how to build and integrate `cppgit2` in your projects.

## Note to self:

Use inotifywait for demos

```
while inotifywait --quiet -e close_write main.cpp; do clear && g++ -o main main.cpp && ./main; done
```

Rainglow theme VSCode: Rainbow contrast (rainglow)

## Build and Integration

Run the following commands to build `cppgit2`. 

**NOTE**: This also builds `libgit2` from source. `libgit2` is a submodule in the `ext/` directory that points to a stable release commit, e.g., [v0.28.4](https://github.com/libgit2/libgit2/releases/tag/v0.28.4).

```bash
git clone --recurse-submodules -j8 https://github.com/p-ranav/cppgit2
cd cppgit2
mkdir build && cd build
cmake .. && make
```

The build output is in three directories: `include`, `lib`, and `samples`:

```
include/
├── cppgit2/
├── git2/
└── git2.h
lib/
├── libcppgit2.so -> libcppgit2.so.1
├── libcppgit2.so.0.1.0
├── libcppgit2.so.1 -> libcppgit2.so.0.1.0
├── libcppgit2.static.a
├── libgit2_clar
├── libgit2.pc
├── libgit2.so -> libgit2.so.28
├── libgit2.so.0.28.4
├── libgit2.so.28 -> libgit2.so.0.28.4
└── ...
samples/
test/
```

For integration in your projects,

* Add `build/include` to your `include_directories`
* Add `build/lib` to your `link_directories`
* Build your application, linking with `cppgit2`
* Add `build/lib` to your `LD_LIBRARY_PATH` to load the shared libraries at runtime. 

Here's an example using `g++`:

```bash
g++ -std=c++11 -Ibuild/include -Lbuild/lib -o my_sample my_sample.cpp -lcppgit2
export LD_LIBRARY_PATH=build/lib:$LD_LIBRARY_PATH
./my_sample
```

and the same example with `CMake`:

```cmake
PROJECT(my_sample)
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

INCLUDE_DIRECTORIES("build/include")
ADD_EXECUTABLE(my_sample my_sample.cpp)
find_library(CPPGIT2_LIBRARY cppgit2 HINTS ./build/lib)
TARGET_LINK_LIBRARIES(my_sample ${CPPGIT2_LIBRARY})
SET_PROPERTY(TARGET my_sample PROPERTY CXX_STANDARD 11)
```

## API Coverage

### annotated
```diff
git_annotated_commit_free
git_annotated_commit_from_fetchhead
git_annotated_commit_from_ref
git_annotated_commit_from_revspec
git_annotated_commit_id
git_annotated_commit_lookup
git_annotated_commit_ref
```

### apply
```diff
git_apply
git_apply_to_tree
```

### attr
```diff
git_attr_add_macro
git_attr_cache_flush
git_attr_foreach
git_attr_get
git_attr_get_many
git_attr_value
```

### blame
```diff
git_blame_buffer
git_blame_file
git_blame_free
git_blame_get_hunk_byindex
git_blame_get_hunk_byline
git_blame_get_hunk_count
git_blame_init_options
git_blame_options_init
```

### blob
```diff
git_blob_create_from_buffer
git_blob_create_from_disk
git_blob_create_from_stream
git_blob_create_from_stream_commit
git_blob_create_from_workdir
git_blob_create_fromworkdir
+ git_blob_dup
git_blob_filter
git_blob_filtered_content
+ git_blob_free
+ git_blob_id
+ git_blob_is_binary
git_blob_lookup
git_blob_lookup_prefix
git_blob_owner
+ git_blob_rawcontent
+ git_blob_rawsize
```

### branch
```diff
git_branch_create
git_branch_create_from_annotated
git_branch_delete
git_branch_is_checked_out
git_branch_is_head
git_branch_iterator_free
git_branch_iterator_new
git_branch_lookup
git_branch_move
git_branch_name
git_branch_next
git_branch_remote_name
git_branch_set_upstream
git_branch_upstream
git_branch_upstream_name
git_branch_upstream_remote
```

### buf
```diff
+ git_buf_contains_nul
+ git_buf_dispose
+ git_buf_free
+ git_buf_grow
+ git_buf_is_binary
+ git_buf_set
```

### checkout
```diff
git_checkout_head
git_checkout_index
git_checkout_options_init
git_checkout_tree
```

### cherrypick
```diff
git_cherrypick
git_cherrypick_commit
git_cherrypick_options_init
```

### clone
```diff
git_clone
git_clone_options_init
```

### commit
```diff
+ git_commit_amend
+ git_commit_author
git_commit_author_with_mailmap
+ git_commit_body
+ git_commit_committer
git_commit_committer_with_mailmap
git_commit_create
git_commit_create_buffer
git_commit_create_v
git_commit_create_with_signature
+ git_commit_dup
git_commit_extract_signature
+ git_commit_free
+ git_commit_header_field
+ git_commit_id
git_commit_lookup
git_commit_lookup_prefix
+ git_commit_message
+ git_commit_message_encoding
+ git_commit_message_raw
+ git_commit_nth_gen_ancestor
git_commit_owner
+ git_commit_parent
+ git_commit_parent_id
+ git_commit_parentcount
+ git_commit_raw_header
+ git_commit_summary
+ git_commit_time
+ git_commit_time_offset
+ git_commit_tree
+ git_commit_tree_id
```

### config
```diff
+ git_config_add_file_ondisk
- git_config_backend_foreach_match
+ git_config_delete_entry
+ git_config_delete_multivar
+ git_config_entry_free
+ git_config_find_global
+ git_config_find_programdata
+ git_config_find_system
+ git_config_find_xdg
+ git_config_foreach
- git_config_foreach_match
+ git_config_free
+ git_config_get_bool
+ git_config_get_entry
+ git_config_get_int32
+ git_config_get_int64
- git_config_get_mapped
- git_config_get_multivar_foreach
+ git_config_get_path
+ git_config_get_string
+ git_config_get_string_buf
+ git_config_iterator_free
- git_config_iterator_glob_new
+ git_config_iterator_new
+ git_config_lock
- git_config_lookup_map_value
- git_config_multivar_iterator_new
- git_config_new
+ git_config_next
+ git_config_open_default
+ git_config_open_global
+ git_config_open_level
- git_config_open_ondisk
+ git_config_parse_bool
+ git_config_parse_int32
+ git_config_parse_int64
+ git_config_parse_path
+ git_config_set_bool
+ git_config_set_int32
+ git_config_set_int64
+ git_config_set_multivar
+ git_config_set_string
+ git_config_snapshot
```

### cred
```diff
git_cred_default_new
git_cred_free
git_cred_get_username
git_cred_has_username
git_cred_ssh_custom_new
git_cred_ssh_interactive_new
git_cred_ssh_key_from_agent
git_cred_ssh_key_memory_new - Appears to be the same as git_cred_ssh_key_new
git_cred_ssh_key_new
git_cred_username_new
git_cred_userpass
git_cred_userpass_plaintext_new
```

### describe
```diff
git_describe_commit
git_describe_format
git_describe_format_options_init
git_describe_options_init
git_describe_result_free
git_describe_workdir
```

### diff
```diff
git_diff_blob_to_buffer
+ git_diff_blobs
git_diff_buffers
git_diff_commit_as_email
git_diff_find_options_init
git_diff_find_similar
git_diff_foreach
git_diff_format_email
git_diff_format_email_options_init
+ git_diff_free
git_diff_from_buffer
+ git_diff_get_delta
git_diff_get_stats
git_diff_index_to_index
git_diff_index_to_workdir
+ git_diff_is_sorted_icase
+ git_diff_merge
+ git_diff_num_deltas
+ git_diff_num_deltas_of_type
+ git_diff_options_init
git_diff_patchid
git_diff_patchid_options_init
git_diff_print
git_diff_stats_deletions
git_diff_stats_files_changed
git_diff_stats_free
git_diff_stats_insertions
git_diff_stats_to_buf
+ git_diff_status_char
+ git_diff_to_buf
git_diff_tree_to_index
git_diff_tree_to_tree
git_diff_tree_to_workdir
git_diff_tree_to_workdir_with_index
```

### error
```diff
git_error_clear
git_error_last
git_error_set_oom
git_error_set_str
```

### fetch
```diff
git_fetch_options_init
```

### filter
```diff
git_filter_list_apply_to_blob
git_filter_list_apply_to_data
git_filter_list_apply_to_file
git_filter_list_contains
git_filter_list_free
git_filter_list_load
git_filter_list_stream_blob
git_filter_list_stream_data
git_filter_list_stream_file
```

### giterr
```diff
giterr_clear
giterr_last
giterr_set_oom
giterr_set_str
```

### graph
```diff
git_graph_ahead_behind
git_graph_descendant_of
```

### ignore
```diff
git_ignore_add_rule
git_ignore_clear_internal_rules
git_ignore_path_is_ignored
```

### index
```diff
+ git_index_add
+ git_index_add_all
+ git_index_add_bypath
+ git_index_add_from_buffer
+ git_index_caps
+ git_index_checksum
+ git_index_clear
+ git_index_conflict_add
+ git_index_conflict_cleanup
git_index_conflict_get
+ git_index_conflict_iterator_free
+ git_index_conflict_iterator_new
+ git_index_conflict_next
+ git_index_conflict_remove
+ git_index_entry_is_conflict
+ git_index_entry_stage
+ git_index_entrycount
+ git_index_find
+ git_index_find_prefix
+ git_index_free
+ git_index_get_byindex
+ git_index_get_bypath
+ git_index_has_conflicts
+ git_index_iterator_free
+ git_index_iterator_new
+ git_index_iterator_next
+ git_index_new
git_index_open
git_index_owner
+ git_index_path
+ git_index_read
+ git_index_read_tree
+ git_index_remove
+ git_index_remove_all
+ git_index_remove_bypath
+ git_index_remove_directory
+ git_index_set_caps
+ git_index_set_version
+ git_index_update_all
+ git_index_version
+ git_index_write
+ git_index_write_tree
+ git_index_write_tree_to
```

### indexer
```diff
git_indexer_append
git_indexer_commit
git_indexer_free
git_indexer_hash
git_indexer_new
git_indexer_options_init
```

### libgit2
```diff
git_libgit2_features
git_libgit2_init
git_libgit2_opts
git_libgit2_shutdown
git_libgit2_version
```

### merge
```diff
git_merge
git_merge_analysis
git_merge_analysis_for_ref
git_merge_base
git_merge_base_many
git_merge_base_octopus
git_merge_bases
git_merge_bases_many
git_merge_commits
git_merge_file
git_merge_file_from_index
git_merge_file_input_init
git_merge_file_options_init
git_merge_file_result_free
git_merge_options_init
git_merge_trees
```

### message
```diff
git_message_prettify
git_message_trailer_array_free
git_message_trailers
```

### note
```diff
+ git_note_author
git_note_commit_create
git_note_commit_iterator_new
git_note_commit_read
git_note_commit_remove
+ git_note_committer
git_note_create
git_note_default_ref
git_note_foreach
+ git_note_free
+ git_note_id
git_note_iterator_free
git_note_iterator_new
+ git_note_message
git_note_next
git_note_read
git_note_remove
```

### object
```diff
- git_object__size
+ git_object_dup
+ git_object_free
+ git_object_id
git_object_lookup
git_object_lookup_bypath
git_object_lookup_prefix
git_object_owner
+ git_object_peel
+ git_object_short_id
+ git_object_string2type
+ git_object_type
+ git_object_type2string
+ git_object_typeisloose
```

### db
```diff
git_odb_add_alternate
git_odb_add_backend
git_odb_add_disk_alternate
git_odb_backend_loose
git_odb_backend_one_pack
git_odb_backend_pack
git_odb_exists
git_odb_exists_prefix
git_odb_expand_ids
git_odb_foreach
git_odb_free
git_odb_get_backend
git_odb_hash
git_odb_hashfile
git_odb_new
git_odb_num_backends
git_odb_object_data
git_odb_object_dup
git_odb_object_free
git_odb_object_id
git_odb_object_size
git_odb_object_type
git_odb_open
git_odb_open_rstream
git_odb_open_wstream
git_odb_read
git_odb_read_header
git_odb_read_prefix
git_odb_refresh
git_odb_stream_finalize_write
git_odb_stream_free
git_odb_stream_read
git_odb_stream_write
git_odb_write
git_odb_write_pack
```

### oid
```diff
+ git_oid_cmp
+ git_oid_cpy
+ git_oid_equal
+ git_oid_fmt
+ git_oid_fromraw
+ git_oid_fromstr
+ git_oid_fromstrn
- git_oid_fromstrp
+ git_oid_is_zero
+ git_oid_iszero
+ git_oid_ncmp
+ git_oid_nfmt
+ git_oid_pathfmt
+ git_oid_shorten_add
+ git_oid_shorten_free
+ git_oid_shorten_new
+ git_oid_strcmp
+ git_oid_streq
+ git_oid_tostr
+ git_oid_tostr_s
```

### oidarray
```diff
git_oidarray_free
```

### packbuilder
```diff
git_packbuilder_foreach
git_packbuilder_free
git_packbuilder_hash
git_packbuilder_insert
git_packbuilder_insert_commit
git_packbuilder_insert_recur
git_packbuilder_insert_tree
git_packbuilder_insert_walk
git_packbuilder_new
git_packbuilder_object_count
git_packbuilder_set_callbacks
git_packbuilder_set_threads
git_packbuilder_write
git_packbuilder_write_buf
git_packbuilder_written
```

### patch
```diff
git_patch_free
git_patch_from_blob_and_buffer
git_patch_from_blobs
git_patch_from_buffers
git_patch_from_diff
git_patch_get_delta
git_patch_get_hunk
git_patch_get_line_in_hunk
git_patch_line_stats
git_patch_num_hunks
git_patch_num_lines_in_hunk
git_patch_print
git_patch_size
git_patch_to_buf
```

### pathspec
```diff
git_pathspec_free
git_pathspec_match_diff
git_pathspec_match_index
git_pathspec_match_list_diff_entry
git_pathspec_match_list_entry
git_pathspec_match_list_entrycount
git_pathspec_match_list_failed_entry
git_pathspec_match_list_failed_entrycount
git_pathspec_match_list_free
git_pathspec_match_tree
git_pathspec_match_workdir
git_pathspec_matches_path
git_pathspec_new
```

### proxy
```diff
git_proxy_options_init
```

### push
```diff
git_push_options_init
```

### rebase
```diff
git_rebase_abort
git_rebase_commit
git_rebase_finish
git_rebase_free
git_rebase_init
git_rebase_inmemory_index
git_rebase_next
git_rebase_onto_id
git_rebase_onto_name
git_rebase_open
git_rebase_operation_byindex
git_rebase_operation_current
git_rebase_operation_entrycount
git_rebase_options_init
git_rebase_orig_head_id
git_rebase_orig_head_name
```

### refdb
```diff
git_refdb_compress
git_refdb_free
git_refdb_new
git_refdb_open
```

### reference
```diff
+ git_reference_cmp
git_reference_create
git_reference_create_matching
+ git_reference_delete
+ git_reference_dup
git_reference_dwim
git_reference_ensure_log
git_reference_foreach
git_reference_foreach_glob
git_reference_foreach_name
+ git_reference_free
git_reference_has_log
+ git_reference_is_branch
+ git_reference_is_note
+ git_reference_is_remote
+ git_reference_is_tag
+ git_reference_is_valid_name
git_reference_iterator_free
git_reference_iterator_glob_new
git_reference_iterator_new
git_reference_list
git_reference_lookup
+ git_reference_name
git_reference_name_to_id
git_reference_next
git_reference_next_name
git_reference_normalize_name
+ git_reference_owner
+ git_reference_peel
git_reference_remove
git_reference_rename
+ git_reference_resolve
git_reference_set_target
+ git_reference_shorthand
git_reference_symbolic_create
git_reference_symbolic_create_matching
git_reference_symbolic_set_target
git_reference_symbolic_target
+ git_reference_target
+ git_reference_target_peel
+ git_reference_type
```

### reflog
```diff
+ git_reflog_append
git_reflog_delete
+ git_reflog_drop
+ git_reflog_entry_byindex
+ git_reflog_entry_committer
+ git_reflog_entry_id_new
+ git_reflog_entry_id_old
+ git_reflog_entry_message
+ git_reflog_entrycount
+ git_reflog_free
git_reflog_read
git_reflog_rename
+ git_reflog_write
```

### refspec
```diff
git_refspec_direction
git_refspec_dst
git_refspec_dst_matches
git_refspec_force
git_refspec_free
git_refspec_parse
git_refspec_rtransform
git_refspec_src
git_refspec_src_matches
git_refspec_string
git_refspec_transform
```

### remote
```diff
git_remote_add_fetch
git_remote_add_push
git_remote_autotag
git_remote_connect
git_remote_connected
git_remote_create
git_remote_create_anonymous
git_remote_create_detached
git_remote_create_options_init
git_remote_create_with_fetchspec
git_remote_create_with_opts
git_remote_default_branch
git_remote_delete
git_remote_disconnect
git_remote_download
git_remote_dup
git_remote_fetch
git_remote_free
git_remote_get_fetch_refspecs
git_remote_get_push_refspecs
git_remote_get_refspec
git_remote_init_callbacks
git_remote_is_valid_name
git_remote_list
git_remote_lookup
git_remote_ls
git_remote_name
git_remote_owner
git_remote_prune
git_remote_prune_refs
git_remote_push
git_remote_pushurl
git_remote_refspec_count
git_remote_rename
git_remote_set_autotag
git_remote_set_pushurl
git_remote_set_url
git_remote_stats
git_remote_stop
git_remote_update_tips
git_remote_upload
git_remote_url
```

### repository
```diff
+ git_repository_commondir
+ git_repository_config
+ git_repository_config_snapshot
+ git_repository_detach_head
+ git_repository_discover
git_repository_fetchhead_foreach
+ git_repository_free
+ git_repository_get_namespace
+ git_repository_hashfile
git_repository_head
+ git_repository_head_detached
+ git_repository_head_detached_for_worktree
git_repository_head_for_worktree
+ git_repository_head_unborn
+ git_repository_ident
+ git_repository_index
+ git_repository_init
git_repository_init_ext
git_repository_init_options_init
+ git_repository_is_bare
+ git_repository_is_empty
+ git_repository_is_shallow
+ git_repository_is_worktree
+ git_repository_item_path
git_repository_mergehead_foreach
+ git_repository_message
+ git_repository_message_remove
git_repository_odb
+ git_repository_open
+ git_repository_open_bare
git_repository_open_ext
git_repository_open_from_worktree
+ git_repository_path
git_repository_refdb
+ git_repository_set_head
+ git_repository_set_head_detached
git_repository_set_head_detached_from_annotated
+ git_repository_set_ident
+ git_repository_set_namespace
+ git_repository_set_workdir
+ git_repository_state
+ git_repository_state_cleanup
+ git_repository_workdir
git_repository_wrap_odb
```

### reset
```diff
git_reset
git_reset_default
git_reset_from_annotated
```

### revert
```diff
git_revert
git_revert_commit
git_revert_options_init
```

### revparse
```diff
git_revparse
git_revparse_ext
git_revparse_single
```

### revwalk
```diff
git_revwalk_add_hide_cb
git_revwalk_free
git_revwalk_hide
git_revwalk_hide_glob
git_revwalk_hide_head
git_revwalk_hide_ref
git_revwalk_new
git_revwalk_next
git_revwalk_push
git_revwalk_push_glob
git_revwalk_push_head
git_revwalk_push_range
git_revwalk_push_ref
git_revwalk_repository
git_revwalk_reset
git_revwalk_simplify_first_parent
git_revwalk_sorting
```

### signature
```diff
git_signature_default
+ git_signature_dup
+ git_signature_free
+ git_signature_from_buffer
+ git_signature_new
+ git_signature_now
```

### stash
```diff
git_stash_apply
git_stash_apply_options_init
git_stash_drop
git_stash_foreach
git_stash_pop
git_stash_save
```

### status
```diff
git_status_byindex
git_status_file
git_status_foreach
git_status_foreach_ext
git_status_list_entrycount
git_status_list_free
git_status_list_new
git_status_options_init
git_status_should_ignore
```

### strarray
```diff
+ git_strarray_copy
+ git_strarray_free
```

### submodule
```diff
git_submodule_add_finalize
git_submodule_add_setup
git_submodule_add_to_index
git_submodule_branch
git_submodule_clone
git_submodule_fetch_recurse_submodules
git_submodule_foreach
git_submodule_free
git_submodule_head_id
git_submodule_ignore
git_submodule_index_id
git_submodule_init
git_submodule_location
git_submodule_lookup
git_submodule_name
git_submodule_open
git_submodule_owner
git_submodule_path
git_submodule_reload
git_submodule_repo_init
git_submodule_resolve_url
git_submodule_set_branch
git_submodule_set_fetch_recurse_submodules
git_submodule_set_ignore
git_submodule_set_update
git_submodule_set_url
git_submodule_status
git_submodule_sync
git_submodule_update
git_submodule_update_options_init
git_submodule_update_strategy
git_submodule_url
- git_submodule_wd_id
```

### tag
```diff
git_tag_annotation_create
git_tag_create
git_tag_create_from_buffer
git_tag_create_lightweight
git_tag_delete
+ git_tag_dup
git_tag_foreach
+ git_tag_free
+ git_tag_id
git_tag_list
git_tag_list_match
git_tag_lookup
git_tag_lookup_prefix
+ git_tag_message
+ git_tag_name
git_tag_owner
+ git_tag_peel
+ git_tag_tagger
+ git_tag_target
+ git_tag_target_id
+ git_tag_target_type
```

### trace
```diff
- git_trace_set
```

### transaction
```diff
+ git_transaction_commit
+ git_transaction_free
+ git_transaction_lock_ref
git_transaction_new
+ git_transaction_remove
+ git_transaction_set_reflog
+ git_transaction_set_symbolic_target
+ git_transaction_set_target
```

### tree
```diff
git_tree_create_updated
+ git_tree_dup
+ git_tree_entry_byid
+ git_tree_entry_byindex
+ git_tree_entry_byname
+ git_tree_entry_bypath
+ git_tree_entry_cmp
+ git_tree_entry_dup
+ git_tree_entry_filemode
+ git_tree_entry_filemode_raw
+ git_tree_entry_free
+ git_tree_entry_id
+ git_tree_entry_name
git_tree_entry_to_object
+ git_tree_entry_type
+ git_tree_entrycount
+ git_tree_free
+ git_tree_id
git_tree_lookup
git_tree_lookup_prefix
git_tree_owner
git_tree_walk
```

### treebuilder
```diff
+ git_treebuilder_clear
+ git_treebuilder_entrycount
+ git_treebuilder_filter
+ git_treebuilder_free
+ git_treebuilder_get
+ git_treebuilder_insert
+ git_treebuilder_new
+ git_treebuilder_remove
+ git_treebuilder_write
+ git_treebuilder_write_with_buffer
```

### worktree
```diff
git_worktree_add
git_worktree_add_options_init
+ git_worktree_free
+ git_worktree_is_locked
+ git_worktree_is_prunable
git_worktree_list
+ git_worktree_lock
git_worktree_lookup
+ git_worktree_name
git_worktree_open_from_repository
+ git_worktree_path
+ git_worktree_prune
+ git_worktree_prune_options_init
+ git_worktree_unlock
+ git_worktree_validate
```
