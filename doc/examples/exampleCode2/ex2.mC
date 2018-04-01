float calcArea(float r)
{
    float pi;
    pi=3.14;
    float result;
    result=r*r*pi;

	return result;
}

int calcCircumference(float r)
{
    float pi;
    pi=3.14;
    float result;
    result=2*r*pi;

    return result;
}

void main()
{

	print("Please enter the radius : ");

	float r;
	r = read_float();
	print_nl();
    float area;
    area = calcArea(r);
	float circumference;
	circumference=calcCircumference(r);
	print("The area of your circle : ");
	print_float(area);
	print_nl();
	print("The circumference of your circle : ");
    print_float(circumference);
    print_nl();

}
