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

void print_bool(bool b)
{
	if (b) {
		print("true");
	} else {
		print("false");
	}
}

void print_bool_nl(bool b)
{
	if (b) {
		print_str_nl("true");
	} else {
		print_str_nl("false");
	}
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

void print_pair_bool(bool a, string op, bool b)
{
	print_bool(a);
	print(" ");
	print(op);
	print(" ");
	print_bool(b);
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
		print_pair_bool(a, "==", b);
	}
}

void assert_ne_bool(bool a, bool b)
{
	if (a == b) {
		print("FAIL:");
		print_pair_bool(a, "!=", b);
	}
}

void assert_bool(bool a)
{
	if (!a) {
		print("FAIL:");
		print_bool_nl(a);
	}
}

void assert_not_bool(bool a)
{
	if (a) {
		print("FAIL:");
		print_bool_nl(a);
	}
}

void assert(bool a)
{
	if (!a) {
		print("FAIL:");
		print_bool_nl(a);
	}
}

void test_asserts()
{
	/* int */
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

	/* float */
	assert_eq_float(1.0, 1.0);
	assert_eq_float(1.3, 1.3);
	assert_eq_float(-2.3, -2.3);
	assert_ne_float(2.3, -2.3);
	assert_ne_float(1.0, -4.9);
	assert_gt_float(1.0, 0.6);
	assert_gt_float(1.0, -0.6);
	assert_gt_float(55.5, -0.6);
	assert_lt_float(3.3, 10.5);
	assert_lt_float(44.5, 100.4);
	assert_lt_float(-3.3, -0.6);
	assert_lt_float(-3.3, 10.6);
	assert_gte_float(3.3, 3.3);
	assert_gte_float(3.33, 3.3);
	assert_lte_float(3.3, 3.3);
	assert_lte_float(3.3, 3.33);
	assert_between_float(3.3, 3.31, 3.33);
	assert_between_float(-1.0, 0.0, 1.0);
	assert_between_eq_float(0.0, 0.0, 1.0);
	assert_between_eq_float(-1.0, -1.0, -1.0);

	/* bool */
	assert_eq_bool(true, true);
	assert_eq_bool(false, false);
	assert_ne_bool(false, true);
	assert_ne_bool(true, false);
	assert_bool(true);
	assert_not_bool(false);

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

void test_bool()
{
	bool b;
	b = true;
	assert_bool(b);
	assert_eq_bool(b, true);
	assert_ne_bool(b, false);

	b = false;
	assert_not_bool(b);
	assert_eq_bool(b, false);
	assert_ne_bool(b, true);

	b = false && false;
	assert_not_bool(b);
	b = false && true;
	assert_not_bool(b);
	b = true && false;
	assert_not_bool(b);
	b = true && true;
	assert_bool(b);

	b = false || false;
	assert_not_bool(b);
	b = false || true;
	assert_bool(b);
	b = true || false;
	assert_bool(b);
	b = true || true;
	assert_bool(b);

	b = !false;
	assert_bool(b);
	b = !true;
	assert_not_bool(b);

	bool c;
	c = true;
	b = !(false || true) && !(c && false);
	assert_not_bool(b);

	print_str_nl("Finished test_bool");
}

void test_float()
{
	float a;
	a = 3.3;
	assert_eq_float(a, 3.3);
	assert_ne_float(a, 4.2);
	a = 4.3;
	assert_ne_float(a, 3.4);
	assert_eq_float(a, 4.3);
	a = a + 2.0;
	assert_eq_float(a, 6.3);
	a = a - 3.1;
	assert_between_eq_float(3.19999, a, 3.20001); /* 3.2 */
	a = a * 4.0;
	assert_between_eq_float(12.7999, a, 12.80001); /* 12.8*/
	a = a / 6.0;
	assert_between_eq_float(2.1332, a, 2.13334); /* 2.1333 */
	a = a - (1.0 / 3.0) / 10.0 - 3.0;
	assert_between_eq_float(-1.00001, a, 0.9999); /* -1 */

	print_str_nl("Finished test_float");
}

void test_control()
{
	int a;
	a = 0;
	if (true) {
		a = a + 1;
	} else {
		a = a + 1;
		assert(false);
	}
	assert_eq_int(a, 1);

	if (false) {
		a = a + 1;
		assert(false);
	} else {
		a = a + 1;
	}
	assert_eq_int(a, 2);

	if (true) {
		a = a + 1;
	}
	assert_eq_int(a, 3);

	if (false) {
		assert(false);
		a = a + 1;
	}
	assert_eq_int(a, 3);

	int c;
	c = 3;
	while (c >= 0) {
		c = c - 1;
		a = a + 1;
	}
	assert_eq_int(a, 7);

	int d;
	d = 5;
	while (d > 0) {
		int m;
		m = d / 2; /* modulo */
		if (m * 2 == d) {
			a = a + 1;
		}
		d = d - 1;
	}

	assert_eq_int(a, 9);

	print_str_nl("Finished test_control");
}

int returns_int_3()
{
	return 3;
}
float returns_float_3d4()
{
	return 3.4;
}
bool returns_bool_false()
{
	return false;
}
void gets_int_3(int a)
{
	assert_eq_int(a, 3);
}
void gets_float_3d4(float f)
{
	assert_eq_float(f, 3.4);
}
void gets_bool_false(bool b)
{
	assert_eq_bool(b, false);
}
int inc_int(int i)
{
	return i + 1;
}
float inc_float(float i)
{
	return i + 1.0;
}
bool switch_bool(bool b)
{
	return !b;
}
int get_first_int(int a, int b, int c)
{
	return a;
}
int get_second_int(int a, int b, int c)
{
	return b;
}
int get_third_int(int a, int b, int c)
{
	return c;
}

int cond_return(int a)
{
	if (a > 5) {
		return 1;
	} else {
		if (a < 3) {
			return 2;
		} else {
			return 3;
		}
	}
}

void test_functions()
{
	int r;
	r = returns_int_3();
	float f;
	f = returns_float_3d4();
	bool b;
	b = returns_bool_false();
	assert_eq_int(r, 3);
	assert_eq_int(returns_int_3(), 3);
	assert_eq_float(f, 3.4);
	assert_eq_float(returns_float_3d4(), 3.4);
	assert_eq_bool(b, false);
	assert_eq_bool(returns_bool_false(), false);

	gets_int_3(3);
	gets_float_3d4(3.4);
	gets_bool_false(false);

	assert_eq_int(inc_int(inc_int(3)), 5);
	assert_eq_float(inc_float(inc_float(3.3)), 5.3);
	assert_eq_bool(switch_bool(switch_bool(true)), true);

	assert_eq_int(get_first_int(3, 4, 5), 3);
	assert_eq_int(get_second_int(3, 4, 5), 4);
	assert_eq_int(get_third_int(3, 4, 5), 5);

	assert_eq_int(cond_return(1), 2);
	assert_eq_int(cond_return(6), 1);
	assert_eq_int(cond_return(4), 3);

	print_str_nl("Finished test_functions");
}

void test_arrays()
{
	int[3] i_arr;
	i_arr[0] = 1;
	i_arr[1] = 2;
	i_arr[2] = 3;
	assert_eq_int(i_arr[0], 1);
	assert_eq_int(i_arr[1], 2);
	assert_eq_int(i_arr[2], 3);

	assert_eq_int(i_arr[0] + i_arr[1] + i_arr[2], 6);

	bool[3] b_arr;
	b_arr[0] = true;
	b_arr[1] = false;
	b_arr[2] = true || false;
	assert_eq_bool(b_arr[0], true);
	assert_eq_bool(b_arr[1], false);
	assert_eq_bool(b_arr[2], true);

	assert_eq_bool(b_arr[0] && b_arr[1] && b_arr[2], false);

	float[3] f_arr;
	f_arr[0] = 1.1;
	f_arr[1] = 22.2;
	f_arr[2] = 36.9;
	assert_eq_float(f_arr[0], 1.1);
	assert_eq_float(f_arr[1], 22.2);
	assert_eq_float(f_arr[2], 36.9);

	assert_eq_bool(b_arr[i_arr[0]], false);
	assert_eq_float(f_arr[i_arr[1]], 36.9);

	print_str_nl("Finished test_arrays");
}

void main()
{
	test_asserts();
	test_int();
	test_bool();
	test_float();
	test_control();
	test_functions();
	test_arrays();
}