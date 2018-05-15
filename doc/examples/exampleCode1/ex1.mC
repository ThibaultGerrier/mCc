int exp(int n, int exp)
{
	if (exp == 0) {
		return 1;
	}

	int result;
	result=n;
	while (exp>0)
	{
        result=result*n;
        exp=exp-1;
	}

	return result;
}

void main()
{
	print("Please enter a number: ");

	int n;
	n = read_int();
	print_nl();

	print("Please enter an exponent: ");

    int exp1;
    exp1 = read_int();
    print_nl();

	int result;
	result = exp(n,exp1);

    print_int(n);
    print("^");
    print_int(exp);
	print("=");
	print_int(result);
	print_nl();
}
