@@ -18,7 +18,7 @@ pes: $(OBJS)
test_objects: test_objects.o object.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_tree: test_tree.o object.o tree.o
test_tree: test_tree.o object.o tree.o index.o
	$(CC) -o $@ $^ $(LDFLAGS)
