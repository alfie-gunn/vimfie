#include <stdlib.h>

#include "minunit.h"
#include "../file_contents.h"

MU_TEST(test_empty_line_data)
{
	line_data_t *empty = empty_line_data();
	mu_check(empty->len == 0);
	free_line_data(empty);
}

MU_TEST(test_str_to_line_data)
{
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);
	free_line_data(ld);
	ld = str_to_line_data(NULL);
	mu_check(ld == NULL);
}

MU_TEST(test_resize_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to double, check values
	int status = resize_line_data(ld, 10);
	mu_check(status == 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize back to original, check values
	status = resize_line_data(ld, 5);
	mu_check(status == 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to smaller, confirm no changes
	status = resize_line_data(ld, 4);
	mu_check(status != 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize null, confirm proper handling
	status = resize_line_data(NULL, 10);
	mu_check(status != 0);

	free_line_data(ld);
}

MU_TEST(test_standard_resize_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to less than double, expect double resize
	int status = standard_upsize_line_data(ld, 3);
	mu_check(status == 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to more than double, expect specific resize
	status = standard_upsize_line_data(ld, 15);
	mu_check(status == 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 25);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize a null pointer
	status = standard_upsize_line_data(NULL, 5);
	mu_check(status != 0);

	// Resize to negative "extra"
	status = standard_upsize_line_data(ld, -5);
	mu_check(status != 0);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 25);
	mu_assert_string_eq(buffer, ld->line_contents);

	free_line_data(ld);
}

MU_TEST(test_insert_char_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_check(ld->len == 5);
	mu_check(ld->capacity == 5);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Insert at end
	int status = insert_char_line_data(ld, 5, 'f');
	mu_check(status == 0);
	mu_check(ld->len == 6);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq("hellof", ld->line_contents);

	// Insert at in middle
	status = insert_char_line_data(ld, 3, 'f');
	mu_check(status == 0);
	mu_check(ld->len == 7);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq("helflof", ld->line_contents);

	// Insert at start
	status = insert_char_line_data(ld, 0, 'f');
	mu_check(status == 0);
	mu_check(ld->len == 8);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to negative index
	status = insert_char_line_data(ld, -1, 'f');
	mu_check(status != 0);
	mu_check(ld->len == 8);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to oob index
	status = insert_char_line_data(ld, 9, 'f');
	mu_check(status != 0);
	mu_check(ld->len == 8);
	mu_check(ld->capacity == 10);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to null
	status = insert_char_line_data(NULL, 0, 'f');
	mu_check(status != 0);

	free_line_data(ld);

	line_data_t *empty = empty_line_data();
	mu_check(empty != NULL);
	mu_check(empty->line_contents != NULL);
	mu_check(empty->len == 0);
	mu_check(empty->capacity == 0);
	mu_check(empty->line_contents[0] == '\0');

	status = insert_char_line_data(empty, 0, 'h');
	mu_check(status == 0);
	mu_check(empty->len == 1);
	mu_check(empty->capacity == 1);
	mu_assert_string_eq("h", empty->line_contents);

	free_line_data(empty);
}

MU_TEST_SUITE(test_ld_suite)
{
	MU_RUN_TEST(test_empty_line_data);
	MU_RUN_TEST(test_str_to_line_data);
	MU_RUN_TEST(test_resize_line_data);
	MU_RUN_TEST(test_standard_resize_line_data);
	MU_RUN_TEST(test_insert_char_line_data);
}

int main(int argc, char *argv[])
{
	MU_RUN_SUITE(test_ld_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}