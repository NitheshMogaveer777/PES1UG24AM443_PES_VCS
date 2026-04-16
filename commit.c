@@ -194,8 +194,36 @@ int head_update(const ObjectID *new_commit) {
//
// Returns 0 on success, -1 on error.
int commit_create(const char *message, ObjectID *commit_id_out) {
    // TODO: Implement commit creation
    // (See Lab Appendix for logical steps)
    (void)message; (void)commit_id_out;
    return -1;
}
    ObjectID tree_id;
    if (tree_from_index(&tree_id) != 0) {
        fprintf(stderr, "error: failed to build tree from index\n");
        return -1;
    }

    Commit commit;
    commit.tree = tree_id;

    if (head_read(&commit.parent) == 0) {
        commit.has_parent = 1;
    } else {
        commit.has_parent = 0;
    }

    snprintf(commit.author, sizeof(commit.author), "%s", pes_author());
    commit.timestamp = (uint64_t)time(NULL);
    snprintf(commit.message, sizeof(commit.message), "%s", message);

    void *commit_data;
    size_t commit_len;
    if (commit_serialize(&commit, &commit_data, &commit_len) != 0)
        return -1;

    if (object_write(OBJ_COMMIT, commit_data, commit_len, commit_id_out) != 0) {
        free(commit_data);
        return -1;
    }
    free(commit_data);

    return head_update(commit_id_out);
}
