@@ -600,3 +600,45 @@ The following questions cover filesystem concepts beyond the implementation scop
- **Git Internals** (Pro Git book): https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain
- **Git from the inside out**: https://codewords.recurse.com/issues/two/git-from-the-inside-out
- **The Git Parable**: https://tom.preston-werner.com/2009/05/19/the-git-parable.html

## Phase 5: Written Questions

### Q5.1: Branching and Checkout
**How would you implement `pes checkout <branch>`?**

To checkout a branch:
1. Read the target branch file from `.pes/refs/heads/<branch>` to get its commit hash
2. Update `.pes/HEAD` to `ref: refs/heads/<branch>`
3. Read the commit object → get the tree hash → recursively read the tree
4. Update the working directory files to match the tree snapshot
5. Update the index to match the new tree

**Complexity**: The hard part is handling conflicts — if the user has uncommitted changes to files that differ between branches, checkout must refuse. You'd also need to handle file creation/deletion when switching between trees with different file sets.

### Q5.2: Dirty Working Directory Detection
**How to detect uncommitted changes using only the index and object store?**

1. For each entry in the index, `stat()` the working directory file
2. Compare `mtime` and `size` from stat with the index entry's stored values
3. If metadata differs, re-hash the file contents and compare with the stored blob hash
4. If the hash differs, the file has uncommitted changes → refuse checkout
5. Also check for deleted files (stat fails) and new untracked files

### Q5.3: Detached HEAD
**What happens with commits in detached HEAD state?**

In detached HEAD, `HEAD` contains a commit hash directly (not `ref: refs/heads/...`). New commits still work — each new commit's hash replaces the old hash in HEAD. However, if you then checkout a branch, the detached commits become "orphaned" — no branch points to them. They're unreachable but still exist in the object store. To recover: note the commit hash before switching, then create a branch pointing to it: `git branch rescue-branch <hash>`.

### Q6.1: Garbage Collection Algorithm
**Describe an algorithm to find and delete unreachable objects.**

1. **Mark phase**: Start from all branch tips (files in `refs/heads/`). For each reachable commit, mark it and follow: commit → tree → recursively mark all subtrees and blobs. Use a hash set to track reachable hashes.
2. **Sweep phase**: Walk all objects in `.pes/objects/`. Delete any object whose hash is NOT in the reachable set.
3. **Estimation**: For 100K commits with 50 branches, you'd visit ~100K commits + their trees. If each commit has ~100 files, that's ~10M tree/blob references to check (but deduplication means far fewer unique objects).

### Q6.2: GC Race Condition
**Why is concurrent GC + commit dangerous?**

**Race condition**: A commit operation creates objects in stages — first blobs, then trees, then the commit object, then updates the ref. If GC runs between "write tree" and "update ref," it could see the new tree as unreachable (no ref points to it yet) and delete it. When the commit operation tries to update the ref, it points to a deleted object.

**Git's solution**: Git uses a lock file (`.git/gc.lock`) and a grace period — GC only deletes objects older than 2 weeks by default (`gc.pruneExpire`). This ensures in-progress operations complete before their objects become eligible for deletion.
