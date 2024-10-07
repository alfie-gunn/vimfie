#include <stdlib.h>
#include <stdint.h>

#include "minunit.h"
#include "../file_contents.h"
#include "../cursor.h"
#include "../vimfie.h"

MU_TEST(test_free_line_data)
{
	free_line_data(NULL);
	line_data_t *half_empty = calloc(1, sizeof(line_data_t));
	free_line_data(half_empty);
	line_data_t *empty = empty_line_data();
	free_line_data(empty);
}

MU_TEST(test_empty_line_data)
{
	line_data_t *empty = empty_line_data();
	mu_assert_int_eq(0, empty->len);
	free_line_data(empty);
}

MU_TEST(test_str_to_line_data)
{
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);
	free_line_data(ld);
	ld = str_to_line_data(NULL);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)ld);
}

MU_TEST(test_resize_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to double, check values
	int status = resize_line_data(ld, 10);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize back to original, check values
	status = resize_line_data(ld, 5);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to smaller, confirm no changes
	status = resize_line_data(ld, 4);
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize null, confirm proper handling
	status = resize_line_data(NULL, 10);
	mu_assert_int_eq(-1, status);

	free_line_data(ld);
}

MU_TEST(test_standard_resize_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to less than double, expect double resize
	int status = standard_upsize_line_data(ld, 3);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize to more than double, expect specific resize
	status = standard_upsize_line_data(ld, 15);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(25, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Resize a null pointer
	status = standard_upsize_line_data(NULL, 5);
	mu_assert_int_eq(-1, status);

	// Resize to negative "extra"
	status = standard_upsize_line_data(ld, -5);
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(25, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	free_line_data(ld);
}

MU_TEST(test_insert_char_line_data)
{
	// Double check str_to_line_data works
	char buffer[] = "hello";
	line_data_t *ld = str_to_line_data(buffer);
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	// Insert at end
	int status = insert_char_line_data(ld, 5, 'f');
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(6, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("hellof", ld->line_contents);

	// Insert at in middle
	status = insert_char_line_data(ld, 3, 'f');
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(7, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("helflof", ld->line_contents);

	// Insert at start
	status = insert_char_line_data(ld, 0, 'f');
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(8, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to negative index
	status = insert_char_line_data(ld, -1, 'f');
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(8, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to oob index
	status = insert_char_line_data(ld, 9, 'f');
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(8, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("fhelflof", ld->line_contents);

	// Insert to null
	status = insert_char_line_data(NULL, 0, 'f');
	mu_assert_int_eq(-1, status);

	free_line_data(ld);

	line_data_t *empty = empty_line_data();
	mu_assert(empty != NULL, "Expected empty non-null, found NULL");
	mu_assert(empty->line_contents != NULL, "Expected empty->line_contents non-null, found NULL");
	mu_assert_int_eq(0, ld->len);
	mu_assert_int_eq(0, ld->capacity);
	mu_assert_int_eq((char)0, empty->line_contents[0]);

	status = insert_char_line_data(empty, 0, 'h');
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(1, ld->len);
	mu_assert_int_eq(1, ld->capacity);
	mu_assert_string_eq("h", empty->line_contents);

	free_line_data(empty);
}

MU_TEST(test_insert_str_line_data)
{
	char buffer[] = "hello";
	char buffer2[] = "hello";

	line_data_t *ld = str_to_line_data(buffer);

	mu_assert(ld != NULL, "Expected ld non-null, found NULL");
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	int status = insert_str_line_data(ld, 1, buffer2);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(10, ld->len);
	mu_assert_int_eq(10, ld->capacity);
	mu_assert_string_eq("hhelloello", ld->line_contents);

	status = insert_str_line_data(ld, 1, NULL);
	mu_assert_int_eq(-1, status);

	free_line_data(ld);

	status = insert_str_line_data(NULL, 1, buffer);
	mu_assert_int_eq(-1, status);
}

MU_TEST(test_copy_line_data)
{
	char buffer[] = "hello";

	line_data_t *ld = str_to_line_data(buffer);

	mu_assert(ld != NULL, "Expected ld non-null, found NULL");
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(buffer, ld->line_contents);

	line_data_t *newline = copy_line_data(ld);
	mu_assert(newline != NULL, "Expected newline non-null, found NULL");
	mu_assert_int_eq(5, ld->len);
	mu_assert_int_eq(5, ld->capacity);
	mu_assert_string_eq(ld->line_contents, newline->line_contents);

	free_line_data(ld);
	free_line_data(newline);

	line_data_t *null_line = copy_line_data(NULL);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)null_line);
}

MU_TEST_SUITE(test_ld_suite)
{
	MU_RUN_TEST(test_free_line_data);
	MU_RUN_TEST(test_empty_line_data);
	MU_RUN_TEST(test_str_to_line_data);
	MU_RUN_TEST(test_resize_line_data);
	MU_RUN_TEST(test_standard_resize_line_data);
	MU_RUN_TEST(test_insert_char_line_data);
	MU_RUN_TEST(test_insert_str_line_data);
	MU_RUN_TEST(test_copy_line_data);
}

MU_TEST(test_free_line)
{
	free_line(NULL);
	line_t *half_empty = calloc(1, sizeof(line_t));
	free_line(half_empty);
	line_t *empty = empty_line();
	free_line(empty);
}

MU_TEST(test_empty_line)
{
	line_t *empty = empty_line();
	mu_assert(empty != NULL, "Expected empty non-null, found NULL");

	mu_assert(empty->data != NULL, "Expected empty->data non-null, found NULL");
	mu_assert_int_eq(0, empty->data->capacity);
	mu_assert_int_eq(0, empty->data->len);
	mu_assert(empty->data->line_contents != NULL, "Expected empty->data->line_contents non-null, found NULL");
	mu_assert_int_eq((char)0, empty->data->line_contents[0]);

	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)empty->next);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)empty->prev);

	free_line(empty);
}

MU_TEST(test_str_to_line)
{
	char buffer[] = "hello";
	line_t *non_empty = str_to_line(buffer);
	mu_assert(non_empty != NULL, "Expected non_empty non-null, found NULL");
	mu_assert(non_empty->data != NULL, "Expected non_empty->data non-null, found NULL");
	mu_assert_string_eq(non_empty->data->line_contents, buffer);
	free_line(non_empty);
}

MU_TEST(test_insert_line)
{
	char buffer[] = "hello";
	char buffer2[] = "hello2";
	char buffer3[] = "hello3";

	line_t *first = str_to_line(buffer);
	line_t *second = str_to_line(buffer2);
	line_t *third = str_to_line(buffer3);

	int status = insert_line(first, third);

	mu_assert_int_eq(0, status);
	mu_assert_int_eq((uintptr_t)third, (uintptr_t)first->next);
	mu_assert_int_eq((uintptr_t)first, (uintptr_t)third->prev);

	status = insert_line(first, second);

	mu_assert_int_eq(0, status);
	mu_assert_int_eq((uintptr_t)second, (uintptr_t)first->next);
	mu_assert_int_eq((uintptr_t)first, (uintptr_t)second->prev);
	mu_assert_int_eq((uintptr_t)third, (uintptr_t)second->next);
	mu_assert_int_eq((uintptr_t)second, (uintptr_t)third->prev);

	status = insert_line(first, NULL);
	mu_assert_int_eq(-1, status);

	status = insert_line(first, second);
	mu_assert_int_eq(-1, status);

	status = insert_line(NULL, second);
	mu_assert_int_eq(-1, status);

	free_line(first);
	free_line(second);
	free_line(third);
}

MU_TEST(test_split_str)
{
	char buffer[] = "hello";
	int return_size = 0;
	char **test_array = split_str(buffer, '\n', &return_size);
	mu_assert_int_eq(1, return_size);
	mu_assert_string_eq(buffer, test_array[0]);
	free(test_array[0]);
	free(test_array);

	char buffer2[] = "hello\nhello";
	return_size = 0;
	test_array = split_str(buffer2, '\n', &return_size);
	mu_assert_int_eq(2, return_size);
	mu_assert_string_eq("hello\n", test_array[0]);
	mu_assert_string_eq("hello", test_array[1]);
	free(test_array[0]);
	free(test_array[1]);
	free(test_array);

	char buffer3[] = "hello\n\nhello\nhello";
	return_size = 0;
	test_array = split_str(buffer3, '\n', &return_size);
	mu_assert_int_eq(4, return_size);
	mu_assert_string_eq("hello\n", test_array[0]);
	mu_assert_string_eq("\n", test_array[1]);
	mu_assert_string_eq("hello\n", test_array[2]);
	mu_assert_string_eq("hello", test_array[3]);
	free(test_array[0]);
	free(test_array[1]);
	free(test_array[2]);
	free(test_array[3]);
	free(test_array);

	return_size = 0;
	test_array = split_str(NULL, '\n', &return_size);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)test_array);
	mu_assert_int_eq(0, return_size);

	return_size = 0;
	test_array = split_str(buffer3, '\n', NULL);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)test_array);
	mu_assert_int_eq(0, return_size);

	return_size = 0;
	test_array = split_str("", '\n', &return_size);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)test_array);
	mu_assert_int_eq(0, return_size);
}

MU_TEST(test_iteratively_free_lines)
{
	line_t *third = str_to_line("third");
	line_t *second = str_to_line("second");
	line_t *head = str_to_line("head");

	insert_line(head, second);
	insert_line(second, third);

	iteratively_free_lines(head);
}

MU_TEST(test_parse_str_to_lines)
{
	char buffer[] = "hello";
	line_t *head = parse_str_to_lines(buffer);
	mu_assert(head != NULL, "Expected head non-null, found NULL");
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head->prev);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head->next);
	mu_assert_string_eq(buffer, head->data->line_contents);
	free_line(head);

	char buffer2[] = "hello2\nhello";
	head = parse_str_to_lines(buffer2);
	mu_assert(head != NULL, "Expected head non-null, found NULL");
	mu_assert(head->next != NULL, "Expected head->next non-null, found NULL");
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head->next->next);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head->prev);
	mu_assert_string_eq("hello2", head->data->line_contents);
	mu_assert_string_eq("hello", head->next->data->line_contents);
	iteratively_free_lines(head);

	char buffer3[] = "head!\nsecond!\n\nfourth!\n";
	head = parse_str_to_lines(buffer3);
	mu_assert(head != NULL, "Expected head non-null, found NULL");
	mu_assert(head->next != NULL, "Expected head->next non-null, found NULL");
	mu_assert(head->next->next != NULL, "Expected head->next->next non-null, found NULL");
	mu_assert(head->next->next->next != NULL, "Expected head->next->next->next non-null, found NULL");
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head->next->next->next->next);

	mu_assert_string_eq("head!", head->data->line_contents);
	mu_assert_string_eq("second!", head->next->data->line_contents);
	mu_assert_string_eq("", head->next->next->data->line_contents);
	mu_assert_string_eq("fourth!", head->next->next->next->data->line_contents);

	iteratively_free_lines(head);
}

MU_TEST(test_write_line)
{
	int fd[2];
	pipe(fd);

	line_t *head = str_to_line("hello, me!");
	write_line(head, fd[1]);
	char *buffer = calloc(20, sizeof(char));
	read(fd[0], buffer, 19);

	mu_assert_string_eq("hello, me!", buffer);
	iteratively_free_lines(head);
}

MU_TEST(test_write_lines_from_head)
{
	int fd[2];
	pipe(fd);

	char firstbuf[] = "hello!\nI am a string of lines!\nGoodbye!\n";
	line_t *head = parse_str_to_lines(firstbuf);
	write_lines_from_head(head, fd[1]);

	char *secondbuf = calloc(sizeof(firstbuf), sizeof(char));
	read(fd[0], secondbuf, sizeof(firstbuf));

	mu_assert_string_eq(firstbuf, secondbuf);
}

MU_TEST(test_parse_file_to_lines)
{
	FILE *file = fopen("testfile", "r");
	mu_assert(file != NULL, "Expected file non-null, found NULL");

	line_t *head = parse_file_to_lines(file);
	mu_assert(head != NULL, "Expected head non-null, found NULL");

	mu_assert_string_eq("this is a test file", head->data->line_contents);
	mu_assert_string_eq("it is used for testing", head->next->data->line_contents);
	mu_assert_string_eq("do not touch!", head->next->next->data->line_contents);

	iteratively_free_lines(head);
	fclose(file);

	head = parse_file_to_lines(NULL);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)head);
}

MU_TEST(test_write_lines_to_file)
{
	FILE *file = fopen("testfile", "a");
	mu_assert(file != NULL, "Expected file non-null, found NULL");

	line_t *head = parse_str_to_lines("newline!\nnewline!\n");
	mu_assert(head != NULL, "Expected head non-null, found NULL");

	int status = write_lines_to_file(head, file);
	mu_assert_int_eq(0, status);

	iteratively_free_lines(head);
	fclose(file);

	file = fopen("testfile", "r");
	mu_assert(file != NULL, "Expected file non-null, found NULL");
	head = parse_file_to_lines(file);
	mu_assert(head != NULL, "Expected head non-null, found NULL");

	fclose(file);

	mu_assert(head != NULL, "Expected head non-null, found NULL");
	mu_assert(head->next != NULL, "Expected head->next non-null, found NULL");
	mu_assert(head->next->next != NULL, "Expected head->next->next non-null, found NULL");
	mu_assert(head->next->next->next != NULL, "Expected head->next->next->next non-null, found NULL");
	mu_assert(head->next->next->next->next != NULL, "Expected head->next->next->next->next non-null, found NULL");
	mu_assert_string_eq("this is a test file", head->data->line_contents);
	mu_assert_string_eq("it is used for testing", head->next->data->line_contents);
	mu_assert_string_eq("do not touch!", head->next->next->data->line_contents);
	mu_assert_string_eq("newline!", head->next->next->next->data->line_contents);
	mu_assert_string_eq("newline!", head->next->next->next->next->data->line_contents);

	iteratively_free_lines(head);

	fopen("testfile", "w");
	mu_assert(file != NULL, "Expected file non-null, found NULL");

	head = parse_str_to_lines("this is a test file\nit is used for testing\ndo not touch!\n");
	mu_assert(head != NULL, "Expected head non-null, found NULL");


	status = write_lines_to_file(head, file);
	mu_assert_int_eq(0, status);

	fclose(file);

	iteratively_free_lines(head);
}

MU_TEST_SUITE(test_line_suite)
{
	MU_RUN_TEST(test_free_line);
	MU_RUN_TEST(test_empty_line);
	MU_RUN_TEST(test_str_to_line);
	MU_RUN_TEST(test_insert_line);
	MU_RUN_TEST(test_split_str);
	MU_RUN_TEST(test_iteratively_free_lines);
	MU_RUN_TEST(test_parse_str_to_lines);
	MU_RUN_TEST(test_write_line);
	MU_RUN_TEST(test_write_lines_from_head);
	MU_RUN_TEST(test_parse_file_to_lines);
	MU_RUN_TEST(test_write_lines_to_file);
}

MU_TEST(test_new_cursor)
{
	cursor_t *cursor = new_cursor(-1, 10);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)cursor);
	cursor = new_cursor(10, -1);
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)cursor);

	cursor = new_cursor(10, 10);
	mu_assert(cursor != NULL, "Expected cursor non-null, found NULL");
	mu_assert_int_eq(1, cursor->x);
	mu_assert_int_eq(1, cursor->y);
	mu_assert_int_eq(10, cursor->x_bound);
	mu_assert_int_eq(10, cursor->y_bound);

	free(cursor);
}

MU_TEST(test_cursor_updates)
{
	cursor_t *cursor = new_cursor(20, 20);
	mu_assert(cursor != NULL, "Expected cursor non-null, found NULL");

	int status = update_x(cursor, 10);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(10, cursor->x);

	status = update_x(cursor, 25);
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(20, cursor->x);

	status = update_y(cursor, 10);
	mu_assert_int_eq(0, status);
	mu_assert_int_eq(10, cursor->y);

	status = update_y(cursor, 25);
	mu_assert_int_eq(-1, status);
	mu_assert_int_eq(10, cursor->y);
}

MU_TEST_SUITE(test_cursor_suite)
{
	MU_RUN_TEST(test_new_cursor);
	MU_RUN_TEST(test_cursor_updates);
}

MU_TEST(test_new_vimfie)
{
	vimfie_t *vf = new_vimfie(NULL);
	mu_assert(vf != NULL, "Expected vf non-null, found NULL");
	mu_assert(vf->command != NULL, "Expected vf->command non-null, found NULL");
	mu_assert(vf->cursor != NULL, "Expected vf->cursor non-null, found NULL");
	mu_assert(vf->head != NULL, "Expected vf->cursor non-null, found NULL");
	mu_assert_int_eq((uintptr_t)NULL, (uintptr_t)vf->filename);
	free_vimfie(vf);

	vf = new_vimfie("testfile");
	mu_assert(vf != NULL, "Expected vf non-null, found NULL");
	mu_assert(vf->command != NULL, "Expected vf->command non-null, found NULL");
	mu_assert(vf->cursor != NULL, "Expected vf->cursor non-null, found NULL");
	mu_assert(vf->head != NULL, "Expected vf->cursor non-null, found NULL");
	mu_assert_string_eq("testfile", vf->filename);
	free_vimfie(vf);
}

MU_TEST_SUITE(test_vf_suite)
{
	MU_RUN_TEST(test_new_vimfie);
}

int main(int argc, char *argv[])
{
	MU_RUN_SUITE(test_ld_suite);
	MU_RUN_SUITE(test_line_suite);
	MU_RUN_SUITE(test_cursor_suite);
	MU_RUN_SUITE(test_vf_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}