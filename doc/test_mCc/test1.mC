void print_int_nl(int i)
{
	print_int(i);
	print_nl();
}
void print_float_nl(float f)
{
	print_float(f);
	print_nl();
}
void print_str_nl(string str)
{
	print(str);
	print_nl();
}

void print_two_str(string a, string b)
{
	print(a);
	print_str_nl(b);
}

void print_pair_int(int a, string op, int b)
{
	print_int(a);
	print(" ");
	print(op);
	print(" ");
	print_int(b);
	print_nl();
}

void print_tripple_int(int a, string op, int b, string op2, int c)
{
	print_int(a);
	print(" ");
	print(op);
	print(" ");
	print_int(b);
	print(" ");
	print(op2);
	print(" ");
	print_int(c);
	print_nl();
}

void print_pair_float(float a, string op, float b)
{
	print_float(a);
	print(" ");
	print(op);
	print(" ");
	print_float(b);
	print_nl();
}

void print_tripple_float(float a, string op, float b, string op2, float c)
{
	print_float(a);
	print(" ");
	print(op);
	print(" ");
	print_float(b);
	print(" ");
	print(op2);
	print(" ");
	print_float(c);
	print_nl();
}

void assert_eq_int(int a, int b)
{
	if (a != b) {
		print("FAIL: ");
		print_pair_int(a, "==", b);
	}
}

void assert_ne_int(int a, int b)
{
	if (a == b) {
		print("FAIL: ");
		print_pair_int(a, "!=", b);
	}
}

void assert_gt_int(int a, int b)
{
	if (a <= b) {
		print("FAIL: ");
		print_pair_int(a, ">", b);
	}
}

void assert_gte_int(int a, int b)
{
	if (a < b) {
		print("FAIL: ");
		print_pair_int(a, ">=", b);
	}
}

void assert_lt_int(int a, int b)
{
	if (a >= b) {
		print("FAIL: ");
		print_pair_int(a, "<", b);
	}
}

void assert_lte_int(int a, int b)
{
	if (a > b) {
		print("FAIL: ");
		print_pair_int(a, "<=", b);
	}
}

void assert_between_int(int a, int b, int c)
{ /*a < b < c*/
	if (!(a < b && b < c)) {
		print("FAIL: ");
		print_tripple_int(a, "<", b, "<", c);
	}
}

void assert_between_eq_int(int a, int b, int c)
{ /*a <= b <= c*/
	if (!(a <= b && b <= c)) {
		print("FAIL: ");
		print_tripple_int(a, "<=", b, "<=", c);
	}
}

void assert_eq_float(float a, float b)
{
	if (a != b) {
		print("FAIL: ");
		print_pair_float(a, "==", b);
	}
}

void assert_ne_float(float a, float b)
{
	if (a == b) {
		print("FAIL: ");
		print_pair_float(a, "!=", b);
	}
}

void assert_gt_float(float a, float b)
{
	if (a <= b) {
		print("FAIL: ");
		print_pair_float(a, ">", b);
	}
}

void assert_gte_float(float a, float b)
{
	if (a < b) {
		print("FAIL: ");
		print_pair_float(a, ">=", b);
	}
}

void assert_lt_float(float a, float b)
{
	if (a >= b) {
		print("FAIL: ");
		print_pair_float(a, "<", b);
	}
}
void assert_lte_float(float a, float b)
{
	if (a > b) {
		print("FAIL: ");
		print_pair_float(a, "<=", b);
	}
}

void assert_between_float(float a, float b, float c)
{ /*a < b < c*/
	if (!(a < b && b < c)) {
		print("FAIL: ");
		print_tripple_float(a, "<", b, "<", c);
	}
}

void assert_between_eq_float(float a, float b, float c)
{ /*a < b < c*/
	if (!(a <= b && b <= c)) {
		print("FAIL: ");
		print_tripple_float(a, "<=", b, "<=", c);
	}
}

void assert_eq_bool(bool a, bool b)
{
	if (a != b) {
		print("FAIL:");
		print_str_nl(" bools");
	}
}

void assert_ne_bool(bool a, bool b)
{
	if (a == b) {
		print("FAIL:");
		print_str_nl(" bools");
	}
}

void assert_msg_eq_int(string test_case, int a, int b)
{
	if (a == b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void assert_msg_ne_int(string test_case, int a, int b)
{
	if (a != b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void assert_msg_eq_float(string test_case, float a, float b)
{
	if (a == b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void assert_msg_ne_float(string test_case, float a, float b)
{
	if (a != b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void assert_msg_eq_bool(string test_case, bool a, bool b)
{
	if (a == b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void assert_msg_ne_bool(string test_case, bool a, bool b)
{
	if (a != b) {
		print_two_str("SUCC: ", test_case);
	} else {
		print_two_str("FAIL: ", test_case);
	}
}

void test_asserts()
{
	assert_eq_int(1, 1);
	assert_eq_int(-21, -21);
	assert_ne_int(3, 4);
	assert_ne_int(-3, 3);
	assert_gt_int(4, 3);
	assert_lt_int(3, 4);
	assert_gte_int(3, 3);
	assert_gte_int(7, 3);
	assert_between_eq_int(3, 4, 7);
	assert_between_eq_int(3, 3, 3);
	assert_between_eq_int(-1, 0, 1);
	assert_between_eq_int(-1, 0, 0);
	assert_between_int(3, 4, 7);
	assert_between_int(-1, 0, 1);
	print_str_nl("Finished test_asserts");
}

void test_int()
{
	int a;
	a = 3;
	assert_eq_int(a, 3);
	assert_ne_int(a, 4);
	a = 4;
	assert_ne_int(a, 3);
	assert_eq_int(a, 4);
	a = a + 2;
	assert_eq_int(a, 6);
	a = a - 3;
	assert_eq_int(a, 3);
	a = a * 4;
	assert_eq_int(a, 12);
	a = a / 6;
	assert_eq_int(a, 2);
	a = a + a;
	assert_eq_int(a, 4);
	a = 2 * a + 3 * a;
	assert_eq_int(a, 20);
	a = 0;
	assert_eq_int(a, 0);
	a = -1;
	assert_eq_int(a, -1);
	assert_ne_int(a, 0);
	assert_ne_int(a, 1);
	a = a - 2;
	assert_eq_int(a, -3);
	a = a * -1;
	assert_eq_int(a, 3);
	a = a - 5;
	assert_eq_int(a, -2);

	print_str_nl("Finished test_int");
}

void main()
{
	test_asserts();
	test_int();
}