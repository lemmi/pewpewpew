#ifndef LIST_H
#define LIST_H

typedef struct {
	void **data;
	size_t size;
	size_t max;
	void *self;  // this pointer is used as a marker to remove an element
	             // since it is more likely to put NULL into a list then the
		     // list itself.
} list_t;

static inline list_t *list(size_t size) {
	list_t *l = malloc(sizeof(list_t));
	l->size = 0;
	l->max = size;
	l->data = malloc(sizeof(void *) * size);
	l->self = l;

	return l;
}

static inline void free_list(list_t *l) {
	free(l->data);
	free(l);
}

static inline void list_add(list_t *l, void *d) {
	l->data[l->size++] = d;
}

static inline void *list_get(list_t *l, int i) {
	return l->data[i];
}

static inline void list_remove(list_t *l, int i) {
	l->data[i] = l->data[--l->size];
}

// This function only removes the _first_ occurrence of d. It returns true if
// such a d is found, false otherwise.
static inline bool list_remove_by_address(list_t *l, void *d) {
	for (int i = 0; i < l->size; i++) {
		if (list_get(l, i) == d) {
			list_remove(l, i);
			return true;
		}
	}
	return false;
}

// You may use this function for marking entries for deleting and delete them
// with list_clean_up(). Be careful, though: the behavior you get when
// requesting an element that is marked for deletion is undefined. There is no
// problem with marking an element for removal several times, though.
static inline void list_mark_for_removal(list_t *l, uint32_t i) {
	l->data[i] = l->self;
}

// This function cleans up a list by removing all the elements marked for
// removal. After this all elements of the list between 0 and size are
// accessible
static inline void list_clean_up(list_t *l) {
	while (list_remove_by_address(l, l->self)) { }
}

static inline void *list_get_last(list_t *l) {
	return l->data[l->size - 1];
}

static inline bool list_is_full(list_t *l) {
	return l->size >= l->max;
}

#endif /* end of include guard: LIST_H */
